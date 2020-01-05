// Casting.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <cmath>

#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

namespace CastingInternals
{
	// Global statics for use, rather than the stack (main does not need to be reentrant)
	uint8_t  x_uint8[8];
	uint8_t  x;
	uint16_t x_uint16[4];
	uint32_t x_uint32[2];
	uint64_t x_uint64;

	float    x_float32[2];
	double   x_float64;

}
using namespace CastingInternals;

int main()
{
	printf("Welcome to Casting Examples\n");

	printf("x_uint8[0..7]   is @ %p\n", &x_uint8[0]);
	printf("x               is @ %p\n", &x);
	printf("x_uint16[0..3]  is @ %p\n", &x_uint16[0]);
	printf("x_uint32[0..1]  is @ %p\n", &x_uint32[0]);
	printf("x_uint64        is @ %p\n", &x_uint64);
	printf("x_float32[0..1] is @ %p\n", &x_float32[0]);
	printf("x_float64       is @ %p\n", &x_float64);

	x_uint64 = 0x0123456789ABCDEF;
	x_uint8[0] = static_cast<uint8_t>(x_uint64);
	x_uint16[0] = static_cast<uint16_t>(x_uint64);
	x_uint32[0] = static_cast<uint32_t>(x_uint64);
	printf("x_uint8[0]  =   %2.2X\n", x_uint8[0]);
	printf("x_uint16[0] = %4.4X\n", x_uint16[0]);
	printf("x_uint32[0] = %8.8X\n", x_uint32[0]);


	printf("\n");
	printf("x_uint64 = %" PRIX64 "\n", x_uint64);
	memcpy(x_uint8, &x_uint64, sizeof(x_uint8));
	for (unsigned i = 0; i < sizeof(x_uint8); ++i)
	{
		printf("x_uint8[%u] = %2.2X\n", i, x_uint8[i]);
	}

	uint8_t* p = reinterpret_cast<uint8_t*>(&x_uint64);
	printf("\n");
	for (unsigned i = 0; i < sizeof(x_uint64); ++i)
	{
		printf("p[%u] = %2.2X\n", i, p[i]);
	}
	p[3] = 0xAA;
	for (unsigned i = 0; i < sizeof(x_uint64); ++i)
	{
		printf("p[%u] = %2.2X\n", i, p[i]);
	}

	x_float64 = static_cast<double>(x_uint64);
	printf("x_uint64  = %llu\n", x_uint64);
	printf("x_float64 = %20.15f\n", x_float64);

	x_float64 = *reinterpret_cast<double*>(&x_uint64);
	printf("x_float64 = %20.15f\n", x_float64);

	x_float64 = acos(-1.0);
	x_uint64 = *reinterpret_cast<uint64_t*>(&x_float64);
	printf("x_float64 = %20.15f\n", x_float64);
	printf("x_int64 = %" PRIi64 "\n", x_int64);

	for (unsigned i = 0; i < sizeof(x_uint64); ++i)
	{
		printf("p[%u] = %2.2X\n", i, p[i]);
	}

	p[7] += 0x80;

	for (unsigned i = 0; i < sizeof(x_uint64); ++i)
	{
		printf("p[%u] = %2.2X\n", i, p[i]);
	}

	x_float64 = *reinterpret_cast<double*>(&x_uint64);
	printf("x_uint64 = %" PRIX64 "\n", x_uint64);
	printf("x_float64 = %20.15f\n", x_float64);

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
