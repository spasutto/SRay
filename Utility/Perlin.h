#pragma once
#ifndef __PERLIN_H
#define __PERLIN_H

namespace SRay
{
	namespace Utility
	{

		double perlin_noise(double x, double y, double z);

		inline float perlin_noisef(float x, float y, float z)
		{
			return (float)perlin_noise((double)x, (double)y, (double)z);
		}
	}
}

#endif // __PERLIN_H
