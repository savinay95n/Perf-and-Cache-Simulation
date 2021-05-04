#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> 

typedef struct divOutput {
	signed int quotient;
	signed int remainder;
} divOutput;

divOutput divide (signed a , signed b) {
	divOutput res;
	signed int sign;
	if(b!=0) {
		if (a*b > 0) {
			sign =1;
		}
		else {
			sign = -1;
		}
		a = abs(a);
		b = abs(b);
		res.quotient = 0;
		while (a>=b)
		{
			a = a-b;
			res.quotient++;
		}
		res.quotient *= sign;
		res.remainder = a;
	}
	else
	{
		res.quotient = 0;
		res.remainder = 0;
	}
	return res;
}

int main() {
	divOutput num;
	signed int a;
	signed int b;
	for(int i = 0; i < 10000000; i++) {
		a = (rand() % 2000) - 1000;
		b = (rand() % 2000) - 1000;
		num = divide(a,b);
		if(i % 100000 == 0)
		printf("%d/%d = %d\tremainder=%d\n",
				a, b, num.quotient,
				num.remainder);
	}
}
