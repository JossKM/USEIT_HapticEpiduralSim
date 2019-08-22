#include "AlgorithmFingerProxyLineSegment.h"

namespace chai3d
{
	chai3d::AlgorithmFingerProxyLineSegment::AlgorithmFingerProxyLineSegment()
	{
	}

	AlgorithmFingerProxyLineSegment::~AlgorithmFingerProxyLineSegment()
	{
	}

	// Todo: update this description
	//==============================================================================
	/*!
		Given the new position of the device and considering the current
		position of the proxy, this method attempts to move the proxy towards
		the device position (the goal).  If its path is blocked by an obstacle
		(e.g., a triangle in a mesh), the proxy is moved to this intersection
		point and a new goal is calculated as the closest point to the original
		goal in the half-plane above the intersection triangle.
		The process is repeated if necessary, bringing the proxy to its
		final location.

		\param  a_goal  The goal position of the __proxy__ subject to constraints.
	*/
	//==============================================================================
	void AlgorithmFingerProxyLineSegment::computeNextBestProxyPosition(const cVector3d& a_goal)
	{
		bool hit0, hit1, hit2;

		if (m_useDynamicProxy)
		{
			// adjust the proxy according moving objects that may have collided with the proxy
			adjustDynamicProxy(a_goal);

			// search for a first contact
			hit0 = computeNextProxyPositionWithContraints0(a_goal);
			m_proxyGlobalPos = m_nextBestProxyGlobalPos;
			if (!hit0)
			{
				return;
			}

			// search for a second contact
			hit1 = computeNextProxyPositionWithContraints1(a_goal);
			m_proxyGlobalPos = m_nextBestProxyGlobalPos;
			if (!hit1)
			{
				m_contactPointLocalPos0 = cTranspose(m_collisionRecorderConstraint0.m_nearestCollision.m_object->getGlobalRot()) * (m_proxyGlobalPos - m_collisionRecorderConstraint0.m_nearestCollision.m_object->getGlobalPos());
				return;
			}

			// search for a third contact
			hit2 = computeNextProxyPositionWithContraints2(a_goal);
			m_proxyGlobalPos = m_nextBestProxyGlobalPos;
			if (!hit2)
			{
				m_contactPointLocalPos0 = cTranspose(m_collisionRecorderConstraint0.m_nearestCollision.m_object->getGlobalRot()) * (m_proxyGlobalPos - m_collisionRecorderConstraint0.m_nearestCollision.m_object->getGlobalPos());
				m_contactPointLocalPos1 = cTranspose(m_collisionRecorderConstraint1.m_nearestCollision.m_object->getGlobalRot()) * (m_proxyGlobalPos - m_collisionRecorderConstraint1.m_nearestCollision.m_object->getGlobalPos());
				return;
			}
			m_contactPointLocalPos0 = cTranspose(m_collisionRecorderConstraint0.m_nearestCollision.m_object->getGlobalRot()) * (m_proxyGlobalPos - m_collisionRecorderConstraint0.m_nearestCollision.m_object->getGlobalPos());
			m_contactPointLocalPos1 = cTranspose(m_collisionRecorderConstraint1.m_nearestCollision.m_object->getGlobalRot()) * (m_proxyGlobalPos - m_collisionRecorderConstraint1.m_nearestCollision.m_object->getGlobalPos());
			m_contactPointLocalPos2 = cTranspose(m_collisionRecorderConstraint2.m_nearestCollision.m_object->getGlobalRot()) * (m_proxyGlobalPos - m_collisionRecorderConstraint2.m_nearestCollision.m_object->getGlobalPos());
		}
		else
		{
			// In order to keep the finger-proxy algorithm running fast in static mode, we only
			// compute collision with one constraint at the time. The next time
			// the algorithm is called, it searches for the second or
			// third obstacle.

			switch (m_algoCounter)
			{
			case 0:
				hit0 = computeNextProxyPositionWithContraints0(a_goal);
				if (hit0)
				{
					m_contactPointLocalPos0 = cTranspose(m_collisionRecorderConstraint0.m_nearestCollision.m_object->getGlobalRot()) * (m_nextBestProxyGlobalPos - m_collisionRecorderConstraint0.m_nearestCollision.m_object->getGlobalPos());
				}
				break;

			case 1:
				hit1 = computeNextProxyPositionWithContraints1(a_goal);
				if (hit1)
				{
					m_contactPointLocalPos0 = cTranspose(m_collisionRecorderConstraint0.m_nearestCollision.m_object->getGlobalRot()) * (m_nextBestProxyGlobalPos - m_collisionRecorderConstraint0.m_nearestCollision.m_object->getGlobalPos());
					m_contactPointLocalPos1 = cTranspose(m_collisionRecorderConstraint1.m_nearestCollision.m_object->getGlobalRot()) * (m_nextBestProxyGlobalPos - m_collisionRecorderConstraint1.m_nearestCollision.m_object->getGlobalPos());
				}
				break;

			case 2:
				hit2 = computeNextProxyPositionWithContraints2(a_goal);
				if (hit2)
				{
					m_contactPointLocalPos0 = cTranspose(m_collisionRecorderConstraint0.m_nearestCollision.m_object->getGlobalRot()) * (m_nextBestProxyGlobalPos - m_collisionRecorderConstraint0.m_nearestCollision.m_object->getGlobalPos());
					m_contactPointLocalPos1 = cTranspose(m_collisionRecorderConstraint1.m_nearestCollision.m_object->getGlobalRot()) * (m_nextBestProxyGlobalPos - m_collisionRecorderConstraint1.m_nearestCollision.m_object->getGlobalPos());
					m_contactPointLocalPos2 = cTranspose(m_collisionRecorderConstraint2.m_nearestCollision.m_object->getGlobalRot()) * (m_nextBestProxyGlobalPos - m_collisionRecorderConstraint2.m_nearestCollision.m_object->getGlobalPos());
				}
				break;
			}
		}
	}


}