#include "mathUtils.h"
#include "math/CVector3d.h"

inline double lmapd(const double& from, const double& fromMin, const double& fromMax, const double& toMin, const double& toMax)
{
	double fromAbs = from - fromMin;
	double fromMaxAbs = fromMax - fromMin;
	double normal = fromAbs / fromMaxAbs;
	double toMaxAbs = toMax - toMin;
	double toAbs = toMaxAbs * normal;
	double to = toAbs + toMin;
	return to;
}
