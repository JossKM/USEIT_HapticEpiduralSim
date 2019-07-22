#pragma once

inline double lmapd(const double& from, const double& fromMin, const double& fromMax, const double& toMin, const double& toMax);

template<typename T, typename R>
inline R lmap(const T& from, const T& fromMin, const T& fromMax, const R& toMin, const R& toMax)
{
	T fromAbs = from - fromMin;
	T fromMaxAbs = fromMax - fromMin;
	double normal = fromAbs / fromMaxAbs;
	R toMaxAbs = toMax - toMin;
	R toAbs = toMaxAbs * normal;
	R to = toAbs + toMin;
	return to;
}


template<typename T, typename R>
inline R lerp(T from, T to, double tValue)
{
	return (1.0 - tValue) * from + (tValue * to);
}

template<typename T>
inline T lerp(T from, T to, double tValue)
{
	return (1.0 - tValue) * from + (tValue * to);
}