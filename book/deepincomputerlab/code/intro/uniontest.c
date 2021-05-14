#include <stdio.h>

unsigned long double2bits(double d)
{
	union {
		double d;
		unsigned long u;
	} temp;
	temp.d = d;
	return temp.u;
}

double uu2double(unsigned w1, unsigned w2)
{
	union{
		double d;
		unsigned u[2];
	}temp;
	temp.u[0] = w1;
	temp.u[1] = w2;
	return temp.d;
}

void big2small(long b)
{
	long s = 0;
	char *Bptr = &b;
	char *Sptr = &s;
	Sptr += 3;
	int i = 0;
	do{
		*Sptr = *Bptr;
		Bptr++;
		Sptr--;
	}while(i++<4);
	printf("small is :0x%x", s);
}

void main()
{
	long b = 0x12345678;
	printf("  big is :0x%x\n", b);
	big2small(b);
	return;
	double d = 1.01;
	unsigned long u = double2bits(d);
	printf("double is: %2.2x, bits is: %2.2x\n",d, u);
	unsigned w1 = 17, w2 = 16;
	double a1 = uu2double(w1, w2);
	printf("w1 bits is: %x, w2 is:%x, bits is: %x", w1, w2, a1);
}
