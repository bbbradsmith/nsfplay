/*
  KMZ80 flag table builder
  by Mamiya
*/
#include <stdio.h>

#define CF (1 << 0)
#define NF (1 << 1)
#define PF (1 << 2)
#define XF (1 << 3)
#define HF (1 << 4)
#define YF (1 << 5)
#define ZF (1 << 6)
#define SF (1 << 7)

int main()
{
	int i, f;
	for (i = 0; i < 512; i++)
	{
		f = 0;
		if (i > 255) f += CF;
		f += NF;
		if (1 & (1 ^ i ^ (i >> 1) ^ (i >> 2) ^ (i >> 3) ^ (i >> 4) ^ (i >> 5) ^ (i >> 6) ^ (i >> 7))) f += PF;
		if (i & XF) f += XF;
		f += HF;
		if (i & YF) f += YF;
		if ((i & 255) == 0) f += ZF;
		if (i & 128) f += SF;
		if ((i & 7) == 0) printf("\t");
		printf("0x%02x,", f);
		if ((i & 7) == 7)
			printf("\n");
		else
			printf(" ");
	}
	return 0;
}
