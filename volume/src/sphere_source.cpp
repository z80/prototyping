
#include "sphere_source.h"

SphereSource::SphereSource(const Real r, const Vector3 &center)
    : mR(r), mCenter(center)
{
}

Vector4 SphereSource::getValueAndGradient(const Vector3 &position) const
{
    Vector3 pMinCenter = position- mCenter;
    Vector3 gradient = pMinCenter;
    return Vector4(
        gradient.x,
        gradient.y,
        gradient.z,
        mR - gradient.normalise()
        );
}

Real SphereSource::getValue(const Vector3 &position) const
{
    Vector3 pMinCenter = position - mCenter;
    return mR - pMinCenter.length();
}




