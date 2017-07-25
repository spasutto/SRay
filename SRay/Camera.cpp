#include "Camera.h"

namespace SRay
{

Camera::Camera(ushort pWidth, ushort pHeight, double pFov)
:m_viewplane(pWidth, pHeight), m_distvp(0.0f)
{
	vRight = Vector(1.0f, 0, 0);
	vUp = Vector(0, 1.0f, 0);
	vFront = Vector(0, 0, 1.0f);
	m_pos = position(0,0,-200.0f);
	setFov(pFov);
	//m_viewplane.m_widthr = tan(0.5f*pFov*PI/180)*m_distvp.x*2;
	//m_viewplane.m_heightr = ((double)pHeight/(double)pWidth)*m_viewplane.m_widthr;
}

Camera::~Camera(void)
{
}

void Camera::setPos(position &pos)
{
	m_pos = pos;
	// si on a définit la visée par une position plutôt qu'une direction on recalcule la direction
	if (bDirPos)
		setDirection(m_dir);
}

void Camera::setDirection(Vector dir)
{
	bDirPos = false;
	setDirectionInternal(dir);
}

void Camera::setDirection(double x, double y, double z)
{
	// on ne peut faire cela car sinon le compilateur choisit toujours la signature setDirection(Vector dir)
	//return setDirection(position(x, y, z));
	position pos_tmp = position(x,y,z);
	setDirection(pos_tmp);
}

void Camera::setDirection(position &dir)
{
	bDirPos = true;
	m_dir = dir;
	Vector vTmp = Vector(dir) - Vector(m_pos);
	setDirectionInternal(vTmp);
}

void Camera::init()
{
	// si l'utilisateur a à la fin défini le fov on recalcule le viewplane
	if (m_hfov != DEFAULT_FOV && getWidthVP() != 0)
	{
		double tmp_dist = getWidthVP() / (2 * tan(RADIAN(0.5f*m_hfov)));
		uint heightVP = uint(round(tmp_dist*(2 * tan(RADIAN(0.5f*m_vfov)))));
		m_viewplane.setHeight(heightVP);
	}
	else if (m_vfov != DEFAULT_FOV && getHeightVP() != 0)
	{
		double tmp_dist = getHeightVP() / (2 * tan(RADIAN(0.5f*m_vfov)));
		uint widthVP = uint(round(tmp_dist*(2 * tan(RADIAN(0.5f*m_hfov)))));
		m_viewplane.setWidth(widthVP);
	}
	m_viewplane.init();
	m_distvp = m_viewplane.m_widthr / (2 * tan(RADIAN(0.5f*m_hfov)));
	m_init = m_distvp*vFront+vUp*(0.5f*m_viewplane.m_heightr)-vRight*(0.5f*m_viewplane.m_widthr);
	//debug_log("	m_init : %2.3f %2.3f %2.3f\n", m_init.x, m_init.y, m_init.z);
}

// TODO: cette fonction bug à cause du roulis éventuel (pour l'instant on utilise le simple
// produit vectoriel pour positionner la caméra voir setDirection)
void Camera::setDirectionNormal(Vector &dir)
{
	double phi1 = acos(vFront.dotProduct(dir));
	double cosphi1 = cos(phi1);
	double sinphi1 = sin(phi1);
	Vector N = vFront.crossProduct(dir).getNormalized(); //dans certains cas vFront^newDir ne produit pas un vecteur unitaire...
	vFront = dir;
	// formule de rotation de Rodrigues http://fr.wikipedia.org/wiki/Rotation_vectorielle#Cas_g.C3.A9n.C3.A9ral
	vUp = (cosphi1*vUp) + (1-cosphi1)*(vUp.dotProduct(N))*N+sinphi1*(N.crossProduct(vUp));
	//vRight = (cosphi1*vRight) + (1-cosphi1)*(vRight.dotProduct(N))*N+sinphi1*(N.crossProduct(vRight));
	vRight = vFront.crossProduct(-vUp);
}

void Camera::setDirectionInternal(Vector &dir)
{
	// on décompose la rotation en deux mouvements pour éviter tout phénomène de roulis (vRight doit avoir sa composante y nulle): 
	//	- un en fixant l'axe des ordonnées; on se concentre sur un mouvement de lacet (tangage nul),
	//	- l'autre en fixant les abscisses; on se concentre sur un mouvement de tanguage (lacet nul)
	Vector newDir = dir;
	newDir.Normalize();
//	if (!newDir.Normalize())
//		return;
	if (newDir != vFront)
	{
		newDir.y=vFront.y; // on fixe d'abord l'axe y
		// cas particulier: demi tour de 180° ==> produit vectoriel nul, problématique
		if (newDir.dotProduct(vFront) == -1.0f)
		{
			vRight = -vRight;
			vFront = newDir;
		}
		else
		{
			vUp = newDir.crossProduct(vFront).getNormalized();
			vFront = newDir;
			vRight = vUp.crossProduct(vFront);

			newDir = dir.getNormalized();
			if (newDir != vFront)
			{
				vRight = newDir.crossProduct(-vFront).getNormalized();
				vFront = newDir;
				vUp = vFront.crossProduct(vRight).getNormalized();
			}
		}
	}
}

}
