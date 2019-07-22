//#include "chai3d.h"
//#include "mathUtils.h"
//
//#include "HapticLayers.h"
//
//using namespace chai3d;
//
//namespace NeedleSimPlugin
//{
//	HapticLayerContainer::HapticLayerContainer()
//	{
//		m_lastLayerPenetrated = -1;
//		initialize(10);
//	}
//
//	HapticLayerContainer::~HapticLayerContainer()
//	{
//	}
//
//	void HapticLayerContainer::initialize(unsigned int numLayers)
//	{
//		m_layerMaterials.clear();
//		m_enabled = true;
//
//		m_lastLayerPenetrated = -1;
//		m_numLayersInUse = 0;
//
//		//create a default dataset of dummy layers
//		for (int i = 0; i < numLayers; i++)
//		{
//			// add material
//			HapticLayer layer = HapticLayer(0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
//			m_layerMaterials.push_back(layer);
//		}
//	}
//
//	inline cVector3d HapticLayerContainer::computeForces(cVector3d & devicePosition, double forceScalar)
//	{
//		if (m_enabled)
//		{
//			// what is the displacement parallel to the layers?
//			cVector3d displacement = (devicePosition - m_entryPoint);
//
//			double penetrationDepth = displacement.dot(m_entryDirection);
//
//			double netForceMagnitude = 0.0;
//
//			// for each layer, determine if the layer is already being penetrated.
//			// If it has, then apply the friction force. If not, then apply the tension force.
//			int numLayers = (int)m_layerMaterials.size();
//			for (int layerIdx = 0; layerIdx < numLayers; layerIdx++)
//			{
//				auto layer = &m_layerMaterials[layerIdx];
//
//				if (m_lastLayerPenetrated >= layerIdx)
//				{ // penetrated behaviour
//
//				  // if behind the material's depth
//					if (penetrationDepth < layer->m_restingDepth)
//					{
//						//it is no longer penetrated.
//						m_lastLayerPenetrated = std::max(m_lastLayerPenetrated - 1, -1);
//						layer->onExitLayer();
//
//						//PRINTLN("exited layer" << layerIdx)
//							//any materials after it are also no longer penetrated.
//					}
//
//					//PRINTLN("last layer: " << m_lastLayerPenetrated << "| pen depth: " << penetrationDepth)
//
//					// if this layer is among the active set, apply its forces
//					if (layerIdx < m_numLayersInUse)
//					{
//						double force = layer->computeFrictionForce(penetrationDepth);
//						netForceMagnitude += force;
//					}
//
//					//PRINTLN("inside layer" + layerIdx)
//				}
//				else
//				{ // not-yet-penetrated behaviour
//
//
//				  // Only allow surface touching of the next layer after the last layer penetrated--no skipping layers allowed
//					if (layerIdx != m_lastLayerPenetrated + 1)
//					{
//						break;
//					}
//
//
//					// if this layer is among the active set, apply its forces. 
//					if (layerIdx < m_numLayersInUse)
//					{
//						double displacement = penetrationDepth - layer->m_restingDepth;
//
//						// determine where the haptic tip is in relation to the layer's rest position in 1D.
//						double force = layer->computeTensionForce(displacement);
//
//
//						// if the force is enough to penetrate it, go through.
//						if (force >= layer->m_penetrationThreshold && displacement > 0.0)
//						{
//							m_lastLayerPenetrated = layerIdx;
//							layer->onEnterLayer();
//
//							//PRINTLN("penetrated layer" << layerIdx)
//						}
//
//						netForceMagnitude += force;
//					}
//				}
//			}
//
//			cVector3d outputForce = m_entryDirection;
//			outputForce.normalize();
//			outputForce.mul(-netForceMagnitude);
//
//			return outputForce;
//		}
//		else
//		{
//			return cVector3d(0.0);
//		}
//	}
//
//	HapticLayer::HapticLayer(const double & a_stiffness, const double & a_stiffnessExponent, const double & a_maxFrictionForce, const double & a_penetrationThreshold, const double& a_resistanceToMovement, const double & a_depth) :
//		m_stiffness(a_stiffness),
//		m_stiffnessExponent(a_stiffnessExponent),
//		m_maxFrictionForce(a_maxFrictionForce),
//		m_penetrationThreshold(a_penetrationThreshold),
//		m_malleability(a_resistanceToMovement),
//		m_restingDepth(a_depth),
//		m_displacementPoint(0.0)
//	{
//	}
//
//	HapticLayer::HapticLayer()
//	{
//		m_stiffnessExponent = 1.0;
//		m_maxFrictionForce = 0.1;
//		m_penetrationThreshold = 1.0;
//		m_displacementPoint = 0.0;
//		m_restingDepth = 0.0;
//		m_malleability = 100.0;
//	}
//	double HapticLayer::computeTensionForce(const double& displacement)
//	{
//		double outputForce = 0.0;
//
//		// the layer is receiving pressure
//		if (displacement > 0.0)
//		{
//			outputForce = std::min(m_stiffness * std::pow(displacement, m_stiffnessExponent), m_penetrationThreshold);
//		}
//
//		return outputForce;
//	}
//
//	double HapticLayer::computeFrictionForce(const double & penetrationDepth)
//	{
//		double displacement = penetrationDepth - (m_restingDepth + m_displacementPoint);
//
//		// compute the resistance force.
//		double distance = abs(displacement); //pow function needs to take a positive number
//		double absoluteForce = m_stiffness * std::pow(distance, m_stiffnessExponent);
//
//		// positive direction is inward, negative direction is outward
//		double direction = (displacement < 0.0) ? -1.0 : 1.0;
//
//		// the force was calculated with an absolute value. Reverse if needed.
//		double outputForce = std::min(absoluteForce, m_maxFrictionForce) * direction;
//
//		// move the layer to meet the contact point
//		m_displacementPoint = lerp(m_displacementPoint, penetrationDepth - m_restingDepth,
//			std::min((0.001f * m_malleability), 1.0));
//
//		return outputForce;
//	}
//
//	void HapticLayer::setStiffnessByExponentAndDistance(double a_stiffnessExponent, double distance, double a_penetrationThreshold)
//	{
//		m_stiffnessExponent = a_stiffnessExponent;
//		m_penetrationThreshold = a_penetrationThreshold;
//		m_stiffness = m_penetrationThreshold / (std::pow(distance, m_stiffnessExponent));
//	}
//
//	void HapticLayer::onEnterLayer()
//	{
//		m_displacementPoint = 0.0;
//	}
//
//	void HapticLayer::onExitLayer()
//	{
//	}
//}