#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int LED = 128; // Using 128-bit key

const unsigned char MixColMatrix[4][4] = {
	{4,  1, 2, 2},
	{8,  6, 5, 6},
	{11,14,10, 9},
	{2,  2,15,11},
};
const unsigned char sbox[16] = {12, 5, 6, 11, 9, 0, 10, 13, 3, 14, 15, 8, 4, 7, 1, 2};
const unsigned char WORDFILTER = 0xF;

unsigned char FieldMult(unsigned char a, unsigned char b)
{
	const unsigned char ReductionPoly = 0x3;
	unsigned char x = a, ret = 0;
	int i;
	for(i = 0; i < 4; i++) {
		if((b>>i)&1) ret ^= x;
		if(x&0x8) {
			x <<= 1;
			x ^= ReductionPoly;
		}
		else x <<= 1;
	}
	return ret&WORDFILTER;
}

void AddKey(unsigned char state[4][4], unsigned char* keyBytes, int step)
{
	int i, j;
    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            state[i][j] ^= keyBytes[(4*i+j+step*16)%(LED/4)];
}

void AddConstants(unsigned char state[4][4], int r)
{
	const unsigned char RC[48] = {
		0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3E, 0x3D, 0x3B, 0x37, 0x2F,
		0x1E, 0x3C, 0x39, 0x33, 0x27, 0x0E, 0x1D, 0x3A, 0x35, 0x2B,
		0x16, 0x2C, 0x18, 0x30, 0x21, 0x02, 0x05, 0x0B, 0x17, 0x2E,
		0x1C, 0x38, 0x31, 0x23, 0x06, 0x0D, 0x1B, 0x36, 0x2D, 0x1A,
		0x34, 0x29, 0x12, 0x24, 0x08, 0x11, 0x22, 0x04
	};
	state[1][0] ^= 1;
	state[2][0] ^= 2;
	state[3][0] ^= 3;

	state[0][0] ^= (LED>>4)&0xf;
	state[1][0] ^= (LED>>4)&0xf;
	state[2][0] ^= LED & 0xf;
	state[3][0] ^= LED & 0xf;

	unsigned char tmp = (RC[r] >> 3) & 7;
	state[0][1] ^= tmp;
	state[2][1] ^= tmp;
	tmp =  RC[r] & 7;
	state[1][1] ^= tmp;
	state[3][1] ^= tmp;
}

void SubCell(unsigned char state[4][4])
{
	int i,j;
	for(i = 0; i < 4; i++)
		for(j = 0; j <  4; j++)
			state[i][j] = sbox[state[i][j]];
}

void ShiftRow(unsigned char state[4][4])
{
	int i, j;
	unsigned char tmp[4];
	for(i = 1; i < 4; i++) {
		for(j = 0; j < 4; j++)
			tmp[j] = state[i][j];
		for(j = 0; j < 4; j++)
			state[i][j] = tmp[(j+i)%4];
	}
}

void MixColumn(unsigned char state[4][4])
{
	int i, j, k;
	unsigned char tmp[4];
	for(j = 0; j < 4; j++){
		for(i = 0; i < 4; i++) {
			unsigned char sum = 0;
			for(k = 0; k < 4; k++)
				sum ^= FieldMult(MixColMatrix[i][k], state[k][j]);
			tmp[i] = sum;
		}
		for(i = 0; i < 4; i++)
			state[i][j] = tmp[i];
	}
}

void LED_enc(unsigned char* input, const unsigned char* userkey, int ksbits)
{
	unsigned char state[4][4];
	unsigned char keyNibbles[32];
	int i;
	
	// Convert input bytes to state matrix (nibble-wise)
	for(i = 0; i < 16; i++) {
		if(i%2) state[i/4][i%4] = input[i>>1]&0xF;
		else state[i/4][i%4] = (input[i>>1]>>4)&0xF;
	}

	// Convert key bytes to nibbles
	memset(keyNibbles, 0, 32);
	for(i = 0; i < ksbits/4; i++){
		if(i%2) keyNibbles[i] = userkey[i>>1]&0xF;
		else keyNibbles[i] = (userkey[i>>1]>>4)&0xF;
	}
	
	LED = ksbits;
	int RN = 48; // For 128-bit key, use 48 rounds
	if(LED <= 64)
		RN = 32;
	
	int j;
	AddKey(state, keyNibbles, 0);
	
	for(i = 0; i < RN/4; i++){
		for(j = 0; j < 4; j++)
		{
			AddConstants(state, i*4+j);
			SubCell(state);
			ShiftRow(state);
			MixColumn(state);
		}
		AddKey(state, keyNibbles, i+1);
	}
	
	// Convert state back to bytes
	for(i = 0; i < 8; i++)
		input[i] = ((state[(2*i)/4][(2*i)%4] & 0xF) << 4) | (state[(2*i+1)/4][(2*i+1)%4] & 0xF);
}

void generate_random_64bit(unsigned char* data)
{
	for(int i = 0; i < 8; i++) {
		data[i] = rand() & 0xFF;
	}
}

void print_hex(unsigned char* data, int len)
{
	for(int i = 0; i < len; i++) {
		printf("%02X", data[i]);
	}
}

int main(int argc, char* argv[])
{
	srand(time(NULL));
	
	// Fixed 128-bit key
	unsigned char fixed_key[16] = {
		0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
		0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
	};
	
	printf("Fixed 128-bit Key: ");
	print_hex(fixed_key, 16);
	printf("\n\n");
	
	// Open CSV file for writing
	FILE* csvfile = fopen("led_ciphertexts0.csv", "w");
	if (!csvfile) {
		printf("Error: Could not create CSV file\n");
		return 1;
	}
	
	// Write CSV header
	fprintf(csvfile, "plaintext,ciphertext,label\n");
	
	// Generate 2^17 samples
	const int NUM_SAMPLES = 131072;
	printf("Generating %d ciphertext samples...\n", NUM_SAMPLES);
	
	for(int sample = 0; sample < NUM_SAMPLES; sample++) {
		// Generate random 64-bit plaintext
		unsigned char plaintext[8];
		generate_random_64bit(plaintext);
		unsigned char ciphertext[8];
		memcpy(ciphertext, plaintext, 8);
		
		// Encrypt using LED-128
		LED_enc(ciphertext, fixed_key, 128);
		
		// Write to CSV file
		// Format: plaintext (hex string), ciphertext (hex string), label
		for(int i = 0; i < 8; i++) {
			fprintf(csvfile, "%02X", plaintext[i]);
		}
		fprintf(csvfile, ",");
		
		for(int i = 0; i < 8; i++) {
			fprintf(csvfile, "%02X", ciphertext[i]);
		}
		fprintf(csvfile, ",0\n");
		
		// Progress indicator
		if ((sample + 1) % 10000 == 0) {
			printf("Generated %d samples...\n", sample + 1);
		}
	}
	
	fclose(csvfile);
	printf("\nGenerated and saved %d (2^17) ciphertext samples in 'led_ciphertexts0.csv'.\n", NUM_SAMPLES);
	
	// Showing first few samples (for verification)
	printf("\nFirst few samples:\n");
	printf("Plaintext -> Ciphertext (Label)\n");
	
	// Regenerating first few for displaying
	srand(time(NULL));
	for(int i = 0; i < 3; i++) {
		unsigned char pt[8], ct[8];
		generate_random_64bit(pt);
		memcpy(ct, pt, 8);
		LED_enc(ct, fixed_key, 128);
		
		print_hex(pt, 8);
		printf(" -> ");
		print_hex(ct, 8);
		printf(" (0)\n");
	}
	
	return 0;
}