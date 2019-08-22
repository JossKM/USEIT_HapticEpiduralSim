#pragma once

//------------------------------------------------------------------------------
#ifndef AlgorithmFingerProxyLineSegmentH
#define AlgorithmFingerProxyLineSegmentH
//------------------------------------------------------------------------------
#include "forces/CAlgorithmFingerProxy.h"

namespace chai3d
{
	// custom AlgorithmFingerProxy meant to reduce oscillations
	class AlgorithmFingerProxyLineSegment : public cAlgorithmFingerProxy
	{
	public:
		//! Constructor
		AlgorithmFingerProxyLineSegment();

		//! Destructor
		virtual ~AlgorithmFingerProxyLineSegment();

		void computeNextBestProxyPosition(const cVector3d & a_goal) override;

		cVector3d m_globalNeedleRootPosition;

		//virtual cVector3d computeForces(const cVector3d& a_toolPos, const cVector3d& a_toolVel) override;
	};

}

#endif