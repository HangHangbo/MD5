#pragma once
#include <cmath>
#include <cstring>
#include <fstream>
#include <string>
#define N 64

class MD5{
public:
	MD5();
	std::string getFileMd5(const char * filename);
	std::string getStrMd5(const std::string &str);
private:

	void init();
	//����ÿ��chunk
	void calculateMD5(size_t* chunk);
	//�������һ��chunk
	void calculateMD5Final();

	//std::string getStrMd5(const std::string &str);
	std::string changeHex(size_t num);
	//ÿ��chunk ռ64��byte ÿ�ĸ��ֽڴ��� �㽫chunkתλsize_t 
	size_t F(size_t x, size_t y, size_t z){
		return (x & y) | ((~x) & z);
	}
	size_t G(size_t x, size_t y, size_t z){
		return (x & z) | (y & (~z));
	}
	size_t H(size_t x, size_t y, size_t z){
		return  x ^ y ^ z;
	}
	size_t I(size_t x, size_t y, size_t z){
		return	y ^ (x | (~z));
	}
	size_t shiftLeftRotate(size_t num, size_t n){
		return (num << n) | (num >> (32 - n));

	}
private:

	//��Ϊ����k_[N]��ʱ��Ҫ����2^32 int�治��
	size_t a_;
	size_t b_;
	size_t c_;
	size_t d_;
	/*
	word A: 01 23 45 67
	word B: 89 ab cd ef
	word C: fe dc ba 98
	word D: 76 54 32 10
	���ֽ���ǰ
	a= 0x67452301;
	b = 0xefcdab89;
	c = 0x98badcfe;
	d= 0x10325476;
	*/

	size_t k_[N];
	//ѭ������ƫ����
	size_t sft_[N];

	const size_t chunkByte_;
	unsigned char chunk_[N];	//ÿ��Ҫ����Ĵ�С

	size_t lastByte_;

	//ԭʼ�ļ����ֽڳ���
	unsigned long long totalByte_;
};