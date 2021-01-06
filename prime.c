#include <math.h>
#include "prime.h"

int is_prime(const int x)
{
	if (x < 2) return -1; //undefined
	if (x < 4) return 1; //prime
	if (x % 2 == 0) return 0; //not prime
	for (int i = 3; i <= floor(sqrt((double)x)); i += 2) //floor()向下取整
		if ((x % i) == 0)
			return 0; //not prime
	return 1; //prime
}

int next_prime(int x)
{
	while (is_prime(x) != 1)
		x++;
	return x;
}