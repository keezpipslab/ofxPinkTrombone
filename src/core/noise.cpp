//
//  noise.cpp
//  PinkTrombone - VST3
//
//  Created by Samuel Tarakajian on 8/29/19.
//

#include "noise.h"
#include <ctime>
#include <sys/time.h>
#include <math.h>

typedef struct Grad {
	sample_t x;
	sample_t y;
	sample_t z;
} Grad;

static sample_t dot2(Grad *gin, sample_t x, sample_t y) {
	return gin->x * x + gin->y * y;
}

//Grad.prototype.dot3 = function(x, y, z) {
//	return this.x*x + this.y*y + this.z*z;
//};

static Grad grad3[] = {{1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
	{1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
	{0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1}};

static short p[] = {151,160,137,91,90,15,
	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180};
// To remove the need for index wrapping, sample_t the permutation table length
static int perm[512];
static Grad gradP[512];
static bool didseed = false;

// This isn't a very good seeding function, but it works ok. It supports 2^16
// different seed values. Write something better if you need more seeds.
static void seed(short seed) {
	if(seed < 256) {
		seed |= seed << 8;
	}
	
	for(int i = 0; i < 256; i++) {
		short v;
		if (i & 1) {
			v = p[i] ^ (seed & 255);
		} else {
			v = p[i] ^ ((seed>>8) & 255);
		}
		
		perm[i] = perm[i + 256] = v;
		gradP[i] = gradP[i + 256] = grad3[v % 12];
	}
};

short timeseed() {
	struct timeval tp;
	gettimeofday(&tp, NULL);
	long long mslong = (long long) tp.tv_sec * 1000L + tp.tv_usec / 1000; //get current timestamp in milliseconds
	return mslong;
}

/*
 for(var i=0; i<256; i++) {
 perm[i] = perm[i + 256] = p[i];
 gradP[i] = gradP[i + 256] = grad3[perm[i] % 12];
 }*/

// Skewing and unskewing factors for 2, 3, and 4 dimensions
static sample_t F2 = 0.5 * (sqrt(3.0) - 1.0);
static sample_t G2 = (3 - sqrt(3.0)) / 6.0;

// 2D simplex noise
sample_t simplex2(sample_t xin, sample_t yin) {
	
	if (!didseed) {
		seed(timeseed());
		didseed = true;
	}
	
	sample_t n0, n1, n2; // Noise contributions from the three corners
	// Skew the input space to determine which simplex cell we're in
	sample_t s = (xin + yin) * F2; // Hairy factor for 2D
	int i = floor(xin + s);
	int j = floor(yin + s);
	sample_t t = (i + j) * G2;
	sample_t x0 = xin - i + t; // The x,y distances from the cell origin, unskewed.
	sample_t y0 = yin - j + t;
	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
	if(x0 > y0) { // lower triangle, XY order: (0,0)->(1,0)->(1,1)
		i1 = 1; j1 = 0;
	} else {    // upper triangle, YX order: (0,0)->(0,1)->(1,1)
		i1 = 0; j1 = 1;
	}
	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6
	sample_t x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
	sample_t y1 = y0 - j1 + G2;
	sample_t x2 = x0 - 1 + 2 * G2; // Offsets for last corner in (x,y) unskewed coords
	sample_t y2 = y0 - 1 + 2 * G2;
	// Work out the hashed gradient indices of the three simplex corners
	i &= 255;
	j &= 255;
	Grad gi0 = gradP[i + perm[j]];
	Grad gi1 = gradP[i + i1 + perm[j + j1]];
	Grad gi2 = gradP[i + 1 + perm[j + 1]];
	// Calculate the contribution from the three corners
	sample_t t0 = 0.5 - x0 * x0 - y0 * y0;
	if (t0 < 0) {
		n0 = 0;
	} else {
		t0 *= t0;
		n0 = t0 * t0 * dot2(&gi0, x0, y0);  // (x,y) of grad3 used for 2D gradient
	}
	sample_t t1 = 0.5 - x1 * x1 - y1 * y1;
	if (t1 < 0) {
		n1 = 0;
	} else {
		t1 *= t1;
		n1 = t1 * t1 * dot2(&gi1, x1, y1);
	}
	sample_t t2 = 0.5 - x2 * x2 - y2 * y2;
	if (t2 < 0) {
		n2 = 0;
	} else {
		t2 *= t2;
		n2 = t2 * t2 * dot2(&gi2, x2, y2);
	}
	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return 70 * (n0 + n1 + n2);
};

sample_t simplex1(sample_t xin) {
	return simplex2(xin * 1.2, -xin * 0.7);
}
