// Mapping.cpp
//
// Experiments in storing an environment mapping in a memory limited
// environment (e.g., Arduino or similar)
//
// Both a word-oriented and a byte oriented "universe" is maintained.
//
// As the experiments continue we will explore speed and sizing issues
// to assess the types of storage appropriate for small memory models.
//
// A sparse-matrix storage technique may be considered if the overhead
// is sufficiently small for our needs.
//
// Until that time, the smallest storage will be a bit-oriented storage
// representing a grid of arbitrary size (although here we may just
// refer to them as square inches)
//
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// Copyright (c) 2014 - RocketRedNeck
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
#include <math.h>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
#include <time.h>
#include "Matrix.h"

using namespace std;


#define DIM(x) (sizeof(x)/sizeof(x[0]))

const double PI = 3.141592653589793;
const double DTR = PI / 180.0;          // Degrees to Radians
const double RTD = 1.0 / DTR;           // Radians to Degrees

// Example of collecting data from a sensor in polar coordinates
// and converting to a cartesian system for simple text display

// Create an array representing 1 degree increments from a minimum to maximum angle
// representing the detected radius
const int STOP_LIMIT_DEG = 20;
const int LOW_LIMIT_DEG = STOP_LIMIT_DEG;
const int UPPER_LIMIT_DEG = 180 - STOP_LIMIT_DEG;
const int ANGULAR_RANGE_DEG = UPPER_LIMIT_DEG - LOW_LIMIT_DEG;
int radius_inch[ANGULAR_RANGE_DEG];

// Creat an array to hold the Cartesian coordinates
// In this case we are storing as inches and want enough for
// building a map in front us.
// The map is really just a Boolean indication of detection
// at some point.
// We want efficient storage balanced with execution speed.
// We can either store the data that will be used for display
// in a literal grid or we can get fancier so we only store the
// data sparsely (i.e., only the points we can about)
// On a byte-oriented processor, execution efficiency is a single byte.
const int UPPER_RANGE_INCH = 64;                       // Make this an even number so we have an easy concept of center
                                                       // Make divisible by 8 to use less memory for simple presence detection
const int UPPER_INDEX = UPPER_RANGE_INCH + 1;
const int MIDDLE_INCH = UPPER_RANGE_INCH / 2;
const int MIDDLE_INDEX = MIDDLE_INCH;

typedef signed char byte;
typedef unsigned char unsignedByte;
byte universe[UPPER_INDEX][UPPER_INDEX];  // An (x,y) pair maps into this
                                                        // (0,0) is in the center of the universe
                                                        // but we want to keep track in an efficient
                                                        // manner.
                                                        // C/C++ is row-major meaning that the [i][j] decoding
                                                        // is most efficient if the [j] is incremented in any
                                                        // inner loop (this will be imporant when displaying)

byte simplerUniverse[UPPER_INDEX][UPPER_INDEX/8];   // A bit-oriented mapping

template <typename T>
T setBit(const T& aValue, unsigned int aBitNumLsb0, bool aBitValue = true)
{
    T lclMask = ((T)1 << aBitNumLsb0);
    return (aValue & ~lclMask) | (aBitValue?lclMask:0);
}
template <typename T>
T clrBit(const T& aValue, unsigned int aBitNumLsb0)
{
    return setBit(aValue, aBitNumLsb0, false);
}
template <typename T>
bool getBit(const T& aValue, unsigned int aBitNumLsb0)
{
    return 0 != (aValue & ((T)1 << aBitNumLsb0));
}


// Start in the bright center of our universe (0,0)
// NOTE: For this example we move the point as if we have been moving, tracking out position
double currentX_inch = 0.0;
double currentY_inch = 0.0;

// Define a unit vector representing the direction we are pointing
// This represents the center of the sweep
// NOTE: For this example we will change the initial pointing to be 45 degrees (up and right)
// so we can test the arithmetic
const double rotation_deg = 135.0;
const double cosine = cos(rotation_deg * DTR);
const double sine = sin(rotation_deg * DTR);
double unitX = cosine;
double unitY = sine;

// Simple template function for rounding
// NOT optimal and generally may be uncessary
template <typename T> T round(const T &x)
{
    T retval = x;
    if (retval > 0.0)
    {
        retval = floor(retval + 0.5);
    }
    else if (retval < 0.0)
    {
        retval = ceil(retval - 0.5);
    }

    return retval;
}

// From Rosetta Code - ray casting algorithm in C

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct { double x, y; } vec;
typedef struct { int n; vec* v; } polygon_t, *polygon;

#define BIN_V(op, xx, yy) vec v##op(vec a,vec b){vec c;c.x=xx;c.y=yy;return c;}
#define BIN_S(op, r) double v##op(vec a, vec b){ return r; }
BIN_V(sub, a.x - b.x, a.y - b.y);
BIN_V(add, a.x + b.x, a.y + b.y);
BIN_S(dot, a.x * b.x + a.y * b.y);
BIN_S(cross, a.x * b.y - a.y * b.x);

/* return a + s * b */
vec vmadd(vec a, double s, vec b)
{
	vec c;
	c.x = a.x + s * b.x;
	c.y = a.y + s * b.y;
	return c;
}

/* check if x0->x1 edge crosses y0->y1 edge. dx = x1 - x0, dy = y1 - y0, then
 solve  x0 + a * dx == y0 + b * dy with a, b in real
 cross both sides with dx, then: (remember, cross product is a scalar)
 x0 X dx = y0 X dx + b * (dy X dx)
 similarly,
 x0 X dy + a * (dx X dy) == y0 X dy
 there is an intersection iff 0 <= a <= 1 and 0 <= b <= 1

 returns: 1 for intersect, -1 for not, 0 for hard to say (if the intersect
 point is too close to y0 or y1)
 */
int intersect(vec x0, vec x1, vec y0, vec y1, double tol, vec *sect)
{
	vec dx = vsub(x1, x0), dy = vsub(y1, y0);
	double d = vcross(dy, dx), a;
	if (!d) return 0; /* edges are parallel */

	a = (vcross(x0, dx) - vcross(y0, dx)) / d;
	if (sect)
		*sect = vmadd(y0, a, dy);

	if (a < -tol || a > 1 + tol) return -1;
	if (a < tol || a > 1 - tol) return 0;

	a = (vcross(x0, dy) - vcross(y0, dy)) / d;
	if (a < 0 || a > 1) return -1;

	return 1;
}

/* distance between x and nearest point on y0->y1 segment.  if the point
 lies outside the segment, returns infinity */
double dist(vec x, vec y0, vec y1, double tol)
{
	vec dy = vsub(y1, y0);
	vec x1, s;
	int r;

	x1.x = x.x + dy.y; x1.y = x.y - dy.x;
	r = intersect(x, x1, y0, y1, tol, &s);
	if (r == -1) return HUGE_VAL;
	s = vsub(s, x);
	return sqrt(vdot(s, s));
}

#define for_v(i, z, p) for(i = 0, z = p->v; i < p->n; i++, z++)
/* returns 1 for inside, -1 for outside, 0 for on edge */
int inside(vec v, polygon p, double tol)
{
	/* should assert p->n > 1 */
	int i, k, crosses, intersectResult;
	vec *pv;
	double min_x, max_x, min_y, max_y;

	for (i = 0; i < p->n; i++) {
		k = (i + 1) % p->n;
		min_x = dist(v, p->v[i], p->v[k], tol);
		if (min_x < tol) return 0;
	}

	min_x = max_x = p->v[0].x;
	min_y = max_y = p->v[1].y;

	/* calculate extent of polygon */
	for_v(i, pv, p) {
		if (pv->x > max_x) max_x = pv->x;
		if (pv->x < min_x) min_x = pv->x;
		if (pv->y > max_y) max_y = pv->y;
		if (pv->y < min_y) min_y = pv->y;
	}
	if (v.x < min_x || v.x > max_x || v.y < min_y || v.y > max_y)
		return -1;

	max_x -= min_x; max_x *= 2;
	max_y -= min_y; max_y *= 2;
	max_x += max_y;

	vec e;
	while (1) {
		crosses = 0;
		/* pick a rand point far enough to be outside polygon */
		e.x = v.x + (1 + rand() / (RAND_MAX + 1.)) * max_x;
		e.y = v.y + (1 + rand() / (RAND_MAX + 1.)) * max_x;

		for (i = 0; i < p->n; i++) {
			k = (i + 1) % p->n;
			intersectResult = intersect(v, e, p->v[i], p->v[k], tol, 0);

			/* picked a bad point, ray got too close to vertex.
             re-pick */
			if (!intersectResult) break;

			if (intersectResult == 1) crosses++;
		}
		if (i == p->n) break;
	}
	return (crosses & 1) ? 1 : -1;
}

int init(Matrix<long double> &A)
{
    ifstream fin("matin.txt");
    int n,length,i,j;
    if(fin.is_open())
    {
        fin >> n;
        fin >> length;
        int k=0;
        while(k<length){ fin >> i >> j; fin >> A(i-1,j-1); k++; }
        fin.close();
    }
    else return 0;
    return n;
}

int main(int argc, const char * argv[])
{

    // Clear the grid and set a line at some radius for a test (i.e., a semi-circle)

    for (int i = 0; i < UPPER_INDEX; ++i)
    {
        for (int j = 0; j < UPPER_INDEX; ++j)
        {
            // Create an artificial obstacle at the edge of the universe
            if ((i == 0) || (i == UPPER_RANGE_INCH) || (j == 0) || (j == UPPER_RANGE_INCH))
            {
                universe[i][j] = 127;

                simplerUniverse[i][j>>3] = setBit(simplerUniverse[i][j>>3],7-(j%8),true);
            }
            else
            {
                universe[i][j] = 0;

                simplerUniverse[i][j>>3] = setBit(simplerUniverse[i][j>>3],7-(j%8),false);

            }
        }
    }


    // Mark our current location
    // Round the indicies and offset to match our concept of "center"
    // i.e. in index form, 0 will be more negative and
    int currentJ = MIDDLE_INDEX + static_cast<int>(round(currentX_inch));
    int currentI = MIDDLE_INDEX - static_cast<int>(round(currentY_inch));
    universe[currentI][currentJ] = -1;

    // Assuming we are at (currentX, currentY) scan the region
    // around us
    // For this example we place an artificial return at 10 inches from our
    // current location
    for (unsigned int i = 0; i < DIM(radius_inch); ++i)
    {
        radius_inch[i] = 10;
    }

    // Now convert each radius into an (x,y) pair and tag that point in the grid
    for (unsigned int i = 0; i < DIM(radius_inch); ++i)
    {

        // First compute the Cartesian coordinates of the data in the sensor reference frame
        double this_angle_deg = static_cast<double>(LOW_LIMIT_DEG + i);
        double this_angle_radian = this_angle_deg * DTR;
        double this_radius_in = static_cast<double>(radius_inch[i]);
        double xs_inch = this_radius_in * cos(this_angle_radian);
        double ys_inch = this_radius_in * sin(this_angle_radian);

        // Rotate the coordinates from sensor reference to universe reference
        // This is done by multiplying the position vector of the sensed object
        // by the body to
        double xu_inch =  xs_inch * unitY + ys_inch * unitX;
        double yu_inch = -xs_inch * unitX + ys_inch * unitY;

        // Transverse the detected position relative to the body center
        // into the universe reference frame
        xu_inch += currentX_inch;
        yu_inch += currentY_inch;

        //printf("a = %f : r = %f : x = %f : y = %f\n", this_angle_deg, this_radius_in, xu_inch, yu_inch);

        // Round the indicies and offset to match our concept of "center"
        // i.e. in index form, 0 will be more negative and
        int j_index = MIDDLE_INDEX + static_cast<int>(round(xu_inch));
        int i_index = MIDDLE_INDEX - static_cast<int>(round(yu_inch));

        //printf("i = %d : j = %d\n", i_index, j_index);

        // Limit the values to fit within the display grid
        if (i_index < 0)
        {
            i_index = 0;
        }
        else if (i_index >= UPPER_INDEX)
        {
            i_index = UPPER_INDEX;
        }
        if (j_index < 0)
        {
            j_index = 0;
        }
        else if (j_index >= UPPER_INDEX)
        {
            j_index = UPPER_INDEX;
        }
        simplerUniverse[i_index][j_index>>3] = setBit(simplerUniverse[i_index][j_index>>3],7-(j_index%8),true);

        if (universe[i_index][j_index] < ((unsignedByte)-1)/2)
        {
            ++universe[i_index][j_index];
        }

        /// TODO: Still need to cast a ray from current position to the
        /// object we just found so we can clear any cells that may have
        /// been previously tagged as having an object. I.e., it is possible
        /// we filled in some cells that were later seen from a different
        /// angle and result in a lost detection.
        /// Conceptually, we can use the value as a weighting factor to
        /// let us know how confident we are that there is something in
        /// the specific cell... i.e., + for each detect and - for each loss
        /// but no lower than zero
    }

    // Now "display" the universe on the console
    for (int i = 0; i < UPPER_INDEX; ++i)
    {
        for (int j = 0; j < UPPER_INDEX; ++j)
        {
            //printf("%c ",universe[i][j]==-1?'X':universe[i][j]==127?'!':universe[i][j]>0?'*':'.');

            printf("%c ",getBit(simplerUniverse[i][j>>3],7-(j%8))?'*':'.');
        }
        printf("\n");
    }

    //printf("ux = %f : uy = %f\n",unitX, unitY);

    // Example ray casting
    	vec vsq[] = {	{0,0}, {10,0}, {10,10}, {0,10},
            {2.5,2.5}, {7.5,0.1}, {7.5,7.5}, {2.5,7.5}};

    	polygon_t sq = { 4, vsq }, /* outer square */
        sq_hole = { 8, vsq }; /* outer and inner square, ie hole */

    	vec c = { 10, 5 }; /* on edge */
    	vec d = { 5, 5 };

    	printf("%d\n", inside(c, &sq, 1e-10));
    	printf("%d\n", inside(c, &sq_hole, 1e-10));

    	printf("%d\n", inside(d, &sq, 1e-10));	/* in */
    	printf("%d\n", inside(d, &sq_hole, 1e-10));  /* out (in the hole) */


//    // matrix tests
//    Matrix<long double> A(1806);
//    vector<long double> x(1806,1), y;
//    long double sum;
//    int n,k=0;
//    clock_t t0,tf;
//    cout.precision(17);
//
//    n=init(A);
//
//    //timed multiplication
//    t0=clock();
//    for(int i=0;i<1000;i++){ y=A*x; }
//    tf=clock()-t0;
//
//    //output last element
//    cout << y[1805];
//    //output time to multiply
//    cout << "\n\n" << (double)tf/((double)(CLOCKS_PER_SEC)*(1000.0));
//
//    //do naive multiply for last row to check last element of y
//    long double tmp=0;
//    for(int i=0;i<1806;i++) tmp+=A(1805,i);
//    cout << "\n" << tmp <<"\n";

    return 0;
}

