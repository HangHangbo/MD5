
#include "Md5.h"


//��ʼ��
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


//����ÿ��chunk
void MD5::calculateMD5(size_t *chunk){
	size_t a = a_;
	size_t b = b_;
	size_t c = c_;
	size_t d = d_;

	size_t f, g;   //f��ϣ��������ֵ   chunk[g]


	//64�α任��4�ֲ�����ÿһ�ֲ�����16���Ӳ���
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


	//����buffer��ֵ
	a_ += a;
	b_ += b;
	c_ += c;
	d_ += d;
}

//�������һ��chunk
void MD5::calculateMD5Final(){
	//lastByte_:<64byte ���һ�����ݴ�С
	unsigned char *p = chunk_ + lastByte_;
	//���λ��ǰ��bitλ��1000 0000 0X80
	*p++ = 0x80;
	size_t remainFillByte = chunkByte_ - lastByte_ - 1;
	if (remainFillByte < 8){
		//����8�ֽڴ��ԭԭʼ�ĵ���bit����
		//�Ƚ��������Ԫ chunk_ ʣ�µĲ�����0�����
		memset(p, 0, remainFillByte);
		//�����chunk_���д���
		calculateMD5((size_t*)chunk_);
		//��д����һ��ȫ0 ��chunk_
		memset(chunk_, 0, chunkByte_);
	}
	//���һ��chunk_С��448bit
	else{
		memset(p, 0, remainFillByte);
	}
	//���˸��ֽڴ��ԭʼ�ĵ���bit����
	((unsigned long long*)chunk_)[7] = totalByte_ * 8;
	calculateMD5((size_t*)chunk_);
}




std::string MD5::getFileMd5(const char * filename){
	//�����ļ������������ƴ���
	std::ifstream fin(filename, std::ifstream::binary);
	if (fin.is_open()){
		while (!fin.eof()){
			//ÿ�ζ�64���ֽ�
			fin.read((char*)chunk_, chunkByte_);
			//gcount ��ǰ���˶����ֽ�
			//����64�����������һ���ˣ�����ѭ������������
			if (chunkByte_ != fin.gcount())
				break;
			//��ǰ����64�ֽ�
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
		//��Ϊÿ����ret����ӵ�һ���ֽڵ����ݣ�
		//����һ��ret���ĸ��ֽڣ�
		//������retƴ��ʱ��β��
		//������byteStr��ƴ��ʱ��ͷ��
		//����ÿ�ν���ѭ���Ƚ�bytStr���
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