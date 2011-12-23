#ifndef AES_H_
#define AES_H_

#define TEXT_LEN 4*4
typedef unsigned char byte;

void SetNbNkNr(int key_len);
void AddRoundKey(int round);	//轮密钥加
void SubBytes();		//S盒字节代换
void InvSubBytes();
void ShiftRows();		//行移位
void InvShiftRows();
void MixColumns();		//列混淆
void InvMixColumns();
byte gfmultby01(byte b);
byte gfmultby02(byte b);
byte gfmultby03(byte b);
byte gfmultby09(byte b);
byte gfmultby0b(byte b);
byte gfmultby0d(byte b);
byte gfmultby0e(byte b);
void KeyExpansion(const byte *cipher_key);	//密钥扩展
byte* SubWord(byte* word);	//密钥S盒字代换
byte* RotWord(byte* word);	//密钥移位

int encrypt(const byte plain_text[TEXT_LEN], byte cipher_text[TEXT_LEN], const byte *cipher_key, int key_len);	//加密
int decrypt(const byte cipher_text[TEXT_LEN], byte plain_text[TEXT_LEN], const byte *cipher_key, int key_len);	//解密

#endif

