/*
 * argc = 5
 * argv[1] = orig_file
 * argv[2] = final_file
 * argv[3] = key_file
 * argv[4] = key_len
 * argv[5] = e/d (encrypt/decrypt)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "aes.h"
#define MAX_SZ 32

long get_file_sz(FILE *f);

int main(int argc, char* argv[])
{
	byte orig_text[TEXT_LEN];
	byte final_text[TEXT_LEN];
	byte *cipher_key = NULL;
	FILE *orig_file = NULL;
	FILE *final_file = NULL;
	FILE *key_file = NULL;
	int key_len = 0;
	long plain_size = 0;

	if(argc < 5)
	{
		printf("too few arguments!\n");
		exit(1);
	}
	orig_file = fopen(argv[1], "r");
	final_file = fopen(argv[2], "w");
	key_file = fopen(argv[3], "r");
	if(!orig_file || !final_file || !key_file)
	{
		perror("args");
		exit(1);
	}

	key_len = atoi(argv[4]);
	if(!(128 == key_len || 192 == key_len || 256 == key_len))
	{
		printf("wrong key length!\n");
		exit(1);
	}

	if('e' == *argv[5])	//encrypt
	{
		plain_size = get_file_sz(orig_file);
		char size_buffer[MAX_SZ];
		sprintf(size_buffer, "%ld", plain_size);
		if(fwrite(size_buffer, MAX_SZ, 1, final_file) < 0)
		{
			perror("write size to final file");
			exit(1);
		}
	}
	else if('d' == *argv[5])	//decrypt
	{
		char size_buffer[MAX_SZ];
		if(fread(size_buffer, MAX_SZ, 1, orig_file) < 0)
		{
			perror("read orignal file");
			exit(1);
		}
		plain_size = atol(size_buffer);
	}
	else
	{
		printf("encrypt or decrypt?\n");
		exit(1);
	}

	cipher_key = malloc(key_len);
	int key_len_read = fread(cipher_key, 1, key_len, key_file);
	//printf("%d\n", key_len_read);
	if(key_len_read < 0)
	{
		printf("wrong key file!\n");
		exit(1);
	}
	memset(cipher_key + key_len_read, 0, key_len - key_len_read);
	//printf("%s\n", cipher_key);

	int text_len_read = fread(orig_text, 1, TEXT_LEN, orig_file);
	while(TEXT_LEN == text_len_read)
	{
		if('e' == *argv[5])
			encrypt(orig_text, final_text, cipher_key, key_len);
		else if('d' == *argv[5])
			decrypt(orig_text, final_text, cipher_key, key_len);
		if(fwrite(final_text, 1, TEXT_LEN, final_file) != TEXT_LEN)
		{
			printf("wrong final file!\n");
			exit(1);
		}
		text_len_read = fread(orig_text, 1, TEXT_LEN, orig_file);
	}
	//printf("%d\n", text_len_read);
	if(text_len_read < 0)
	{
		printf("wrong orignal file!\n");
		exit(1);
	}

	//encrypt the last bytes
	if(text_len_read > 0 && 'e' == *argv[5])
	{
		encrypt(orig_text, final_text, cipher_key, key_len);
		if(fwrite(final_text, 1, TEXT_LEN, final_file) != TEXT_LEN)
		{
			printf("wrong final file!\n");
			exit(1);
		}
	}

	fclose(orig_file);
	fclose(final_file);
	fclose(key_file);
	free(cipher_key);

	if('d' == *argv[5])
		truncate(argv[2], plain_size);
	printf("done!\n");
	return 0;
}

long get_file_sz(FILE *f)
{
	long len;
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	rewind(f);
	return len;
}

