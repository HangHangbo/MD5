
#include "Md5.h"


//初始化
MD5::MD5()
:chunkByte_(N)
{
	init();

	memset(chunk_, 0, chunkByte_);
	totalByte_ = lastByte_ = 0;
}

void MD5::init(){
	a_ = 0x67452301;
	b_ = 0xefcdab89;
	c_ = 0x98badcfe;
	d_ = 0x10325476;

	size_t s[N] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
		5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
		4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
		6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };
	memcpy(sft_, s, sizeof(s));

	for (int i = 0; i < 64; i++){
		k_[i] = (size_t)(abs(sin(i + 1.0)) * pow(2.0, 32));
	}

}


//处理每个chunk
void MD5::calculateMD5(size_t *chunk){
	size_t a = a_;
	size_t b = b_;
	size_t c = c_;
	size_t d = d_;

	size_t f, g;   //f哈希函数返回值   chunk[g]


	//64次变换，4轮操作，每一轮操作：16个子操作
	for (size_t i = 0; i < 64; i++){
		if (0 <= i&&i < 16){
			f = F(b, c, d);
			g = i;
		}
		else if (16 <= i&&i < 32){
			f = G(b, c, d);
			g = (5 * i + 1) % 16;
		}
		else if (32 <= i&&i < 48){
			f = H(b, c, d);
			g = (3 * i + 5) % 16;
		}
		else{
			f = I(b, c, d);
			g = (7 * i) % 16;
		}

		size_t dtmp = d;
		d = c;
		c = b;
		b = b + shiftLeftRotate(a + f + k_[i] + chunk[g], sft_[i]);
		a = dtmp;
	}


	//更新buffer的值
	a_ += a;
	b_ += b;
	c_ += c;
	d_ += d;
}

//处理最后一个chunk
void MD5::calculateMD5Final(){
	//lastByte_:<64byte 最后一块数据大小
	unsigned char *p = chunk_ + lastByte_;
	//填充位的前八bit位：1000 0000 0X80
	*p++ = 0x80;
	size_t remainFillByte = chunkByte_ - lastByte_ - 1;
	if (remainFillByte < 8){
		//不够8字节存放原原始文档的bit长度
		//先将这个处理单元 chunk_ 剩下的部分用0填充满
		memset(p, 0, remainFillByte);
		//对这个chunk_进行处理
		calculateMD5((size_t*)chunk_);
		//重写开辟一个全0 的chunk_
		memset(chunk_, 0, chunkByte_);
	}
	//最后一块chunk_小于448bit
	else{
		memset(p, 0, remainFillByte);
	}
	//最后八个字节存放原始文档的bit长度
	((unsigned long long*)chunk_)[7] = totalByte_ * 8;
	calculateMD5((size_t*)chunk_);
}




std::string MD5::getFileMd5(const char * filename){
	//输入文件流，按二进制处理
	std::ifstream fin(filename, std::ifstream::binary);
	if (fin.is_open()){
		while (!fin.eof()){
			//每次读64个字节
			fin.read((char*)chunk_, chunkByte_);
			//gcount 当前读了多少字节
			//不顾64个，读到最后一块了，跳出循环，单独处理
			if (chunkByte_ != fin.gcount())
				break;
			//当前读到64字节
			totalByte_ += chunkByte_;
			calculateMD5((size_t*)chunk_);
		}
		lastByte_ = fin.gcount();
		totalByte_ += lastByte_;
		calculateMD5Final();
	}

	return changeHex(a_) + changeHex(b_) + changeHex(c_) + changeHex(d_);
}

std::string MD5::getStrMd5(const std::string &str){
	if (str.empty()){
		return "";
	}
	else{
		unsigned char*  pstr = (unsigned char *)str.c_str();
		size_t numChunk = str.size() / chunkByte_;
		for (size_t i = 0; i < numChunk; i++){
			totalByte_ += chunkByte_;
			calculateMD5((size_t*)(pstr + i*chunkByte_));
		}
		lastByte_ = str.size() % chunkByte_;
		memcpy(chunk_, pstr + totalByte_, lastByte_);
		totalByte_ += lastByte_;
		calculateMD5Final();
	}
	return changeHex(a_) + changeHex(b_) + changeHex(c_) + changeHex(d_);
}


std::string MD5::changeHex(size_t num){
	static std::string strMap = "0123456789abcdef";
	std::string ret;
	std::string byteStr;
	for (int i = 0; i < 4; i++){
		//因为每次向ret中添加的一个字节的内容，
		//但是一个ret是四个字节，
		//并且在ret拼接时是尾插
		//但是在byteStr中拼接时是头插
		//所以每次进入循环先将bytStr清空
		byteStr = "";
		size_t b = (num >> (i * 8)) & 0x000000ff;
		for (int j = 0; j < 2; ++j){
			byteStr.insert(0, 1, strMap[b % 16]);
			b /= 16;
		}
		ret += byteStr;
	}
	return ret;
}