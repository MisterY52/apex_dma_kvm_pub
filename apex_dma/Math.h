#include <math.h>
#include "vector.h"

struct SVector
{
	float x;
	float y;
	float z;
	SVector(float x1, float y1, float z1)
    {
		x = x1;
		y = y1;
		z = z1;
	}

	SVector(QAngle q)
    {
		x = q.x;
		y = q.y;
		z = q.z;
	}
};

namespace Math
{
	void NormalizeAngles(QAngle& angle);
	double GetFov(const QAngle& viewAngle, const QAngle& aimAngle);
	double DotProduct(const Vector& v1, const float* v2);
	QAngle CalcAngle(const Vector& src, const Vector& dst);
}