
#pragma once

#include <math.h>
#include <vector>
#include <stdexcept>

#include "../Utility/CodeStat.h"

//#define SR_LOG
#define N_AIR	1.0f							//indice de l'air
#define DEFAULT_FOV 45.0f				// champ de vision
#define NBR_SAMPLES_SHADOWS	8		// nombre de rayons d'ombrage par itération
#define NBR_RAND 2000000				// quantité de nombres aléatoires générés "à l'avance"

#define DEGRE(angle) (angle*180/PI)
#define RADIAN(angle) (angle*PI/180)
#define COLOR(r, g, b) (color)((char)r|(char)g<<8|(char)b<<16)
#define SATURATE(a) if (a>1.0f) a=1.0f;
#define SATURMAX(a, b) if (a>b) a=b;
#define MIN(a, b) (((a)<(b))?(a):(b))
#define MAX(a, b) (((a)>(b))?(a):(b))
#ifndef NULL
#define NULL	0
#endif

#ifdef _MSC_VER
#define INLINE __forceinline /* use __forceinline (VC++ specific) */
#else
#define INLINE inline        /* use standard inline */
#endif

namespace SRay
{
	typedef unsigned long ulong;
	typedef unsigned int uint;
	typedef unsigned short ushort;
	typedef unsigned char uchar;
	typedef float _float;
	typedef double dfloat;
	typedef float sfloat;

	static uint UITMP1 = 0, UITMP2 = 0;
	INLINE uint max(uint a, uint b);
	INLINE uint min(uint a, uint b);

typedef struct color
{
	uchar r, g, b;
	//INLINE operator fcolor() const { return pixel{ float(r) / 255.0f, float(g)/ 255.0f, float(b)/ 255.0f }; }
} pixel;
static float ftmp = 0;
struct fcolor
{
	float r, g, b;
#ifndef _DEBUG
	INLINE operator color() //const
	{
		/*r /= 1.834272f;
		g /= 1.834272f;
		b /= 1.834272f;*/

		/*ftmp = MAX(r, ftmp);
		ftmp = MAX(g, ftmp);
		ftmp = MAX(b, ftmp);*/
		/*float exposure = -1.834272f;// -0.66f;
		b = 1.0f -expf(
			b * exposure);
		r = 1.0f -expf(
			r * exposure);
		g = 1.0f -expf(
			g * exposure);*/
		/*if (r > 1.0f || g > 1.0f || b > 1.0f)
			return color{ 255, 0, 255 };*/

		return color{ static_cast<uchar>(min(uint(r * 255), 255)), static_cast<uchar>(min(uint(g * 255), 255)), static_cast<uchar>(min(uint(b * 255), 255)) };
	}
#else
	INLINE operator color() const { return color{ static_cast<uchar>(min(uint(r * 255), 255)), static_cast<uchar>(min(uint(g * 255), 255)), static_cast<uchar>(min(uint(b * 255), 255)) }; }
#endif
	INLINE void operator = (const color &col){ r = (float)col.r / 255.0f; g = (float)col.g / 255.0f; b = (float)col.b / 255.0f; }
	INLINE void operator *= (const float f) { r *= f; g *= f; b *= f; }
	INLINE void operator += (const fcolor& col) { r += col.r; g += col.g; b += col.b; }
	INLINE void operator -= (const fcolor& col) { r -= col.r; g -= col.g; b -= col.b; }
	INLINE friend fcolor operator + (const fcolor& col1, const fcolor& col2) { return fcolor{ col1.r + col2.r, col1.g + col2.g, col1.b + col2.b }; }
	INLINE friend fcolor operator - (const fcolor& col1, const fcolor& col2) { return fcolor{ col1.r - col2.r, col1.g - col2.g, col1.b - col2.b }; }
	INLINE friend fcolor operator * (const fcolor& col1, const fcolor& col2) { return fcolor{ col1.r * col2.r, col1.g * col2.g, col1.b * col2.b }; }
	INLINE friend fcolor operator * (const fcolor& col1, float f) { return fcolor{ col1.r* f, col1.g* f, col1.b* f }; }
	INLINE friend fcolor operator * (float f, const fcolor& col1) { return fcolor{ col1.r* f, col1.g* f, col1.b* f }; }
};

INLINE fcolor tofcolor(color &col)
{
	return fcolor{ (float)col.r / 255.0f, (float)col.g / 255.0f, (float)col.b / 255.0f };
}

// TODO: typedef Vector?
struct position
{
	double x, y, z;
	position() {x=0.0f;y=0.0f;z=0.0f;}
	position(double pX, double pY, double pZ) {x=pX;y=pY;z=pZ;}
};

const color blanc = {255,255,255};
const color noir = {0,0,0};
const fcolor fblanc = { 1.0f, 1.0f, 1.0f };
const fcolor fnoir = { 0.0f, 0.0f, 0.0f };
static char errBuff[128];
const double PI = 3.14159265f;
const double EPSILON = 0.000001f; // voir aussi FLT_EPSILON dans float.h

class Vector
{
public:
	Vector () {x=0.0f;y=0.0f;z=0.0f;}
	Vector (double pX, double pY, double pZ) {x=pX;y=pY;z=pZ;}
	Vector (const position &pPos) {x=pPos.x;y=pPos.y;z=pPos.z;}

	INLINE void Normalize()
	{
		CS_FSTART(Vector::Normalize);
		length = sqrt(x*x + y*y + z*z); //Length();
		if (length == 0.0f)
			x=y=z=0.0f;
		else
		{
			x/=length;
			y/=length;
			z/=length;
		}
		CS_FEND();
	}
	INLINE Vector getNormalized()
	{
		length = sqrt(x*x + y*y + z*z); //Length();
		if (length == 0.0f)
			return invalid;
		return Vector(x/length, y/length, z/length);
	}
	INLINE Vector Normal(const Vector &vec)
	{
		return Vector(y*vec.z - z*vec.y, z*vec.x - x*vec.z, x*vec.y - y*vec.x);
	}
	INLINE double Length()
	{
		return sqrt(x*x + y*y + z*z);
	}
	INLINE double dotProduct(const Vector& v1) const // produit scalaire
	{
		// cos(A,B)= Ax*Bx + Ay*By + Az*Bz
		return /*acos*/(x*v1.x + y*v1.y + z*v1.z);
	}
	INLINE Vector crossProduct(const Vector& v1) const // produit vectoriel
	{
		return Vector(y*v1.z - z*v1.y, z*v1.x - x*v1.z, x*v1.y - y*v1.x);
	}

	INLINE static Vector reflect(const Vector& normal, const Vector& incident)
	{
		const double cosI = normal.dotProduct(incident);
		return incident - 2 * cosI * normal;
	}
	INLINE static Vector refract(const Vector& normal, const Vector& incident,
		double n1, double n2)
	{
		const double n = n1 / n2;
		const double cosI = normal.dotProduct(incident);
		const double sinT2 = n * n * (1.0 - cosI * cosI);
		if (sinT2 > 1.0)
			return invalid;
		return n * incident - (n + sqrt(1.0 - sinT2)) * normal;
	}
	INLINE Vector reflect(const Vector& normal) {return reflect(normal, *this);}
	INLINE Vector refract(const Vector& normal,double n1, double n2) {return refract(normal,*this,n1,n2);}

	INLINE operator position() {return position(x,y,z);}

	/*INLINE Vector operator*(const double &f){return Vector(x*f,y*f,z*f);}
	//INLINE Vector operator*(const Vector &vec){return Vector(x*vec.x,y*vec.y,z*vec.z);}
	INLINE Vector operator+(const Vector &vec){return Vector(x+vec.x,y+vec.y,z+vec.z);}
	//INLINE void operator+=(const Vector &vec){x+=vec.x;y+=vec.y;z+=vec.z;}
	INLINE Vector operator-(const Vector &vec){return Vector(x-vec.x,y-vec.y,z-vec.z);}
	//INLINE void operator-=(const Vector &vec){x-=vec.x;y-=vec.y;z-=vec.z;}
	INLINE void operator=(const Vector &vec){x=vec.x;y=vec.y;z=vec.z;}*/

	INLINE friend Vector operator + (const Vector& v1, const Vector& v2) { return Vector(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }
	INLINE friend Vector operator + (const Vector& v1, const double &f) { return Vector(v1.x + f, v1.y + f, v1.z + f); }
	INLINE friend Vector operator + (const double &f, const Vector& v1) { return Vector(v1.x + f, v1.y + f, v1.z + f); }
	INLINE friend Vector operator + (const position &p, const Vector& v1){ return Vector(p.x + v1.x, p.y + v1.y, p.z + v1.z);}
	INLINE friend Vector operator + (const Vector& v1, const position &p){ return Vector(v1.x + p.x, v1.y + p.y, v1.z + p.z);}

	INLINE friend Vector operator - (const Vector& v1) { return Vector(-v1.x, -v1.y, -v1.z); }

	INLINE friend Vector operator - (const Vector& v1, const Vector& v2) { return Vector(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }
	INLINE friend Vector operator - (const Vector& v1, const double &f){ return Vector(v1.x - f, v1.y - f, v1.z - f);}
	INLINE friend Vector operator - (const double &f, const Vector& v1){ return Vector(f - v1.x, f - v1.y, f - v1.z);}
	INLINE friend Vector operator - (const position &p, const Vector& v1){ return Vector(p.x - v1.x, p.y - v1.y, p.z - v1.z);}
	INLINE friend Vector operator - (const Vector& v1, const position &p){ return Vector(v1.x - p.x, v1.y - p.y, v1.z - p.z);}

	// opérateur * représente le produit scalaire
	//INLINE friend Vector operator * (const Vector& v1, Vector& v2) { return Vector(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z); }
	INLINE friend double operator * (const Vector& v1, Vector& v2) { return (v2.x*v1.x + v2.y*v1.y + v2.z*v1.z); }
	INLINE friend Vector operator * (const Vector& v1, double f) { return Vector(v1.x * f, v1.y * f, v1.z * f); }
	INLINE friend Vector operator * (double f, const Vector& v1) { return Vector(v1.x * f, v1.y * f, v1.z * f); }

	INLINE friend Vector operator / (const Vector& v1, Vector& v2) { return Vector(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z); }
	INLINE friend Vector operator / (const Vector& v1, double f) { return Vector(v1.x / f, v1.y / f, v1.z / f); }
	INLINE friend Vector operator / (double f, const Vector& v1) { return Vector(f / v1.x, f / v1.y, f / v1.z); }

	INLINE void operator += (const double &f) { x += f; y += f; z += f; }
	INLINE void operator += (const Vector& a_V) { x += a_V.x; y += a_V.y; z += a_V.z; }
	//INLINE void operator += ( const Vector* a_V) { x += a_V->x; y += a_V->y; z += a_V->z; }

	INLINE void operator -= (const double &f) { x -= f; y -= f; z -= f; }
	INLINE void operator -= (const Vector& a_V) {x -= a_V.x; y -= a_V.y; z -= a_V.z; }
	//INLINE void operator -= ( const Vector* a_V) { x -= a_V->x; y -= a_V->y; z -= a_V->z; }

	INLINE void operator *= (const double f) { x *= f; y *= f; z *= f; }
	//INLINE void operator *= (const double *f) { x *= *f; y *= *f; z *= *f; }
	INLINE void operator *= (const Vector& a_V) { x *= a_V.x; y *= a_V.y; z *= a_V.z; }
	//INLINE void operator *= (const Vector* a_V) { x *= a_V->x; y *= a_V->y; z *= a_V->z; }

	INLINE void operator /= (const double f) { x /= f; y /= f; z /= f; }
	INLINE void operator /= (const Vector& a_V) { x /= a_V.x; y /= a_V.y; z /= a_V.z; }
	//INLINE void operator /= (const Vector* a_V) { x /= a_V->x; y /= a_V->y; z /= a_V->z; }

	INLINE void operator = (const position &pos){ x=pos.x; y=pos.y; z=pos.z; }
	//INLINE void operator = (const Vector &v1){ x=v1.x; y=v1.y; z=v1.z; }
	
	//INLINE bool operator == (const Vector& a_V){ return (EPSILON > x - a_V.x) && (EPSILON > y - a_V.y) && (EPSILON > z - a_V.z); }
	INLINE bool operator == (const Vector& a_V){ return x==a_V.x && y==a_V.y && z==a_V.z; }
	INLINE bool operator != (const Vector& a_V){ return !(*this == a_V); }

	//INLINE operator double() const { return x; }
//protected:
	double x, y, z;
	static const Vector invalid;
private:
	double length;
};

INLINE double fast_round( double x )
{
	const double sd = 1000; //for accuracy to 3 decimal places
	return int(x*sd + (x < 0 ? -0.5 : 0.5)) / sd;
}

INLINE uint max(uint a, uint b)
{
	UITMP1 = a; UITMP2 = b;
	return MAX(UITMP1, UITMP2);
}

INLINE uint min(uint a, uint b)
{
	UITMP1 = a; UITMP2 = b;
	return MIN(UITMP1, UITMP2);
}

template<class T>
class ExtendedVector : public std::vector<T>
{
public:
	bool Contains(T& obj)
	{
		for (vector<T>::iterator i = this->begin(); i != this->end(); ++i)
		if ((*i) == obj)
			return true;
		return false;
	}
	void push_back_new(value_type&& _Val)
	{
		if (!Contains(_Val))
			push_back(_Val);
	}
};

template<class T = Object>
class ObjVector : public ExtendedVector<T>
{
public:
	T operator ()(const char* name)
	{
		for (vector<T>::iterator i = this->begin(); i != this->end(); ++i)
		if (strcmp((*i)->m_name, name) == 0)
			return *i;
		return NULL;
	}
};

}
