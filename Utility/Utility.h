#pragma once

#include "../SRay/Common.h"

#include <string.h>
#include <cctype>
#include <stdarg.h>

namespace SRay
{
namespace Utility
{

char *trimwhitespace(char *str);

size_t trimwhitespace(char *out, size_t len, const char *str);

#if defined(_DEBUG) || defined(SR_LOG)
int debug_log(const char * format, ...);
#else
#define debug_log     (void)sizeof
#endif

char* ParseFloat(const char *str, double *out);

char* ParseInt(const char *str, int *out);

char* ParseTripletFloat(const char *str, double *out);

char* ParseTripletInt(const char *str, int *out);

#define TOLOWER(a) (a) |= (1 << 5)
#define TOUPPER(a) (a) &= ~(1 << 5)
void strtolower(char *p);
void strtoupper(char *p);

//http://www.graphics.cornell.edu/~bjw/rgbe/rgbe.c (Ward packed rgbe format)
/* standard conversion from float pixels to rgbe pixels */
/* note: you can remove the "inline"s if your compiler complains about it */
INLINE void float2rgbe(unsigned char rgbe[4], float red, float green, float blue)
{
	float v;
	int e;

	v = red;
	if (green > v) v = green;
	if (blue > v) v = blue;
	if (v < 1e-32) {
		rgbe[0] = rgbe[1] = rgbe[2] = rgbe[3] = 0;
	}
	else {
		v = (float)(frexp(v, &e) * 256.0 / v);
		rgbe[0] = (unsigned char)(red * v);
		rgbe[1] = (unsigned char)(green * v);
		rgbe[2] = (unsigned char)(blue * v);
		rgbe[3] = (unsigned char)(e + 128);
	}
}

/* standard conversion from rgbe to float pixels */
/* note: Ward uses ldexp(col+0.5,exp-(128+8)).  However we wanted pixels */
/*       in the range [0,1] to map back into the range [0,1].            */
INLINE void rgbe2float(float *red, float *green, float *blue, unsigned char rgbe[4])
{
	float f;

	if (rgbe[3]) {   /*nonzero pixel*/
		f = (float)ldexp(1.0, rgbe[3] - (int)(128 + 8));
		*red = rgbe[0] * f;
		*green = rgbe[1] * f;
		*blue = rgbe[2] * f;
	}
	else
		*red = *green = *blue = 0.0;
}

INLINE void Negative(color *c)
{
	c->r = ~c->r;
	c->g = ~c->g;
	c->b = ~c->b; }
}
}
