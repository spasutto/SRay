#pragma once

#include "Common.h"
#include "Light.h"
#include "../Utility/Random.h"

using namespace SRay::Utility;

namespace SRay
{

	class AreaLight : public Light
	{
	public:
		AreaLight(char *name = NULL, position pOrigin = position(0, 0, -50.0f), float psize = 1.0f, Vector pnorm = Vector(0.0f, 0.0f, -1.0f), fcolor pColor = fblanc, _float pCoef = 1.0f);
		~AreaLight(void);
		virtual void setNormal(Vector pNormal) { normal = pNormal; Init(); }
		void setPos(double x, double y, double z) { origin = position(x, y, z); Init(); }
		void setPos(position &pos) { origin = pos; Init(); }
		void setSize(float size) { size = size; Init(); }

		virtual float CalcVisibility(position *intersect, Ray *lightray, double *objDist, ObjVector<Shape*> *shapes, Shape *shape)
		{
			uint i;
			uchar x, y;
			Shape *pShape;
			double objDisttemp;
			float curVisib = 0.0f;
			bool bIntersect = false;

			lightray->m_origin = origin;
			lightray->m_origin.x -= size2;
			lightray->m_origin.z -= size2;
			float xinit = lightray->m_origin.x;
			for (y = 0; y < NBR_SAMPLES_SHADOWS; y++)
			{
				lightray->m_origin.z += (Random::Rand() * cellsize);
				lightray->m_origin.x = xinit;
				for (x = 0; x < NBR_SAMPLES_SHADOWS; x++)
				{
					bIntersect = false;

					//lightray->m_origin = origin + x*(vx + (Random::Rand() * cellsize) - size2) + y*(vy + (Random::Rand() * cellsize) - size2);
					lightray->m_origin.x += (Random::Rand() * cellsize);

					lightray->m_direction = *intersect;
					lightray->m_direction -= lightray->m_origin;
					*objDist = lightray->m_direction.Length();
					lightray->m_direction.Normalize();

					for (i = 0; i < shapes->size(); i++)
					{
						pShape = (*shapes)[i];
						if (pShape == shape) continue;
						if (pShape->Intersect(*lightray, &objDisttemp))
						{
							if ((objDisttemp - *objDist) < 0.0f) // WARNING : <=
							{
								bIntersect = true;
								break;
							}
						}
					}
					if (!bIntersect)
						curVisib += SHADOW_INCREMENT;
				}
			}
			return curVisib;
		}

	protected:
		void Init()
		{
			float xn, yn, zn;
			size2 = size / 2.0f;
			cellsize = size / NBR_SAMPLES_SHADOWS;
			vx = Vector(0, 0, 0);
			vy = Vector(0, 0, 0);
			xn = float(abs(normal.x)); yn = float(abs(normal.y)); zn = float(abs(normal.z));
			if (xn > yn && xn > zn)
			{
				vx.z = cellsize;
				vy.y = -cellsize;
			}
			else if (yn > xn && yn > zn)
			{
				vx.z = cellsize;
				vy.x = cellsize;
			}
			else if (zn > xn && zn > yn)
			{
				vx.x = cellsize;
				vy.y = -cellsize;
			}
		}

	public:
		Vector normal;
		position	origin;
		float size, size2;
	private:
		const float SHADOW_INCREMENT = 1.0f / (NBR_SAMPLES_SHADOWS*NBR_SAMPLES_SHADOWS);
		float cellsize;
		Vector vx, vy;
	};

}
