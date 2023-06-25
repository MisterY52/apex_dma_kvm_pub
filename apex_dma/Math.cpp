#include "Math.h"

void Math::NormalizeAngles(QAngle& angle)
{
	while (angle.x > 89.0f)
		angle.x -= 180.f;

	while (angle.x < -89.0f)
		angle.x += 180.f;

	while (angle.y > 180.f)
		angle.y -= 360.f;

	while (angle.y < -180.f)
		angle.y += 360.f;
}

Vector Math::Bezier(const Vector& start, const Vector& mid1, const Vector& mid2, const Vector& end, float t)
{
    Vector q1 = start + (mid1 - start) * t;
    Vector q2 = mid1 + (mid2 - mid1) * t;
    Vector q3 = mid2 + (end - mid2) * t;

    Vector r1 = q1 + (q2 - q1) * t;
    Vector r2 = q2 + (q3 - q2) * t;

    return r1 + (r2 - r1) * t;
    return Vector();
}

QAngle Math::CalcAngle(const Vector& src, const Vector& dst)
{
	QAngle angle = QAngle();
	SVector delta = SVector((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));

	double hyp = sqrt(delta.x*delta.x + delta.y * delta.y);

	angle.x = atan(delta.z / hyp) * (180.0f / M_PI);
	angle.y = atan(delta.y / delta.x) * (180.0f / M_PI);
	angle.z = 0;
	if (delta.x >= 0.0) angle.y += 180.0f;

	return angle;
}

double Math::GetFov(const QAngle& viewAngle, const QAngle& aimAngle)
{
	QAngle delta = aimAngle - viewAngle;
	NormalizeAngles(delta);

	return sqrt(pow(delta.x, 2.0f) + pow(delta.y, 2.0f));
}

double Math::DotProduct(const Vector& v1, const float* v2)
{
	return v1.x * v2[0] + v1.y * v2[1] + v1.z * v2[2];
}
