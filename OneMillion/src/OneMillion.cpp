//============================================================================
// Name        : OneMillion.cpp
//
// Example of how floating point accuracy is related to precision
//
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// Copyright (c) 2016 - RocketRedNeck
//
// RocketRedNeck and MIT License
//
// RocketRedNeck hereby grants license for others to copy and modify this source code for
// whatever purpose other's deem worthy as long as RocketRedNeck is given credit where
// where credit is due and you leave RocketRedNeck out of it for all other nefarious purposes.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ****************************************************************************************************

#include <stdio.h>

int main()
{
	const float DX = 1.0e-6f;

	float x = 0.0;

	for (unsigned int i = 0; i < 1000000; ++i)
	{
		x += DX;
	}
	printf("DX  = %20.17f x  = %20.17f\n",DX,x);

	const double DX2 = 1.0e-6;

	double x2 = 0.0;

	for (unsigned int i = 0; i < 1000000; ++i)
	{
		x2 += DX2;
	}
	printf("DX2 = %20.17f x2 = %20.17f\n",DX2,x2);


	return 0;
}
