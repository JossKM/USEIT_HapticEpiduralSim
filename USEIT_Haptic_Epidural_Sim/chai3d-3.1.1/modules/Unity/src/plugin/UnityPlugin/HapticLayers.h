//#pragma once
//
//namespace NeedleSimPlugin
//{
//	// 1-dimensional layer. Has a state
//	class HapticLayer
//	{
//	public:
//		HapticLayer(const double& a_stiffness, const double& a_stiffnessExponent, const double& a_maxFrictionForce, const double& a_penetrationThreshold, const double& a_malleability, const double& a_depth);
//		HapticLayer();
//
//		~HapticLayer() {}
//
//		// force applied before penetration
//		// uses formula: outputForce = stiffness * displacement^stiffnessExponent
//		// caps at penetration threshold; will not output any higher than that
//		double computeTensionForce(const double& displacement);
//
//		// force applied after penetration. implements a moving spring model where the contact point can move
//		double computeFrictionForce(const double& displacement);
//
//		// the resting depth of the layer, relative to the entry point of the needle
//		double m_restingDepth;
//
//		//inline void setProperties(const double& a_stiffness, const double& a_stiffnessExponent, const double& a_penetrationThreshold);
//
//		// sets the scalar m_stiffness such that at a displacement of {distance} units, the maximum force will be achieved.
//		void setStiffnessByExponentAndDistance(double a_stiffnessExponent, double a_distance, double a_penetrationThreshold);
//
//		// called when the layer is penetrated
//		void onEnterLayer();
//
//		// called when leaving the layer
//		void onExitLayer();
//
//		// Identification number, set from Unity
//		int m_id = -1;
//
//		//// pre-penetration parameters ///////////
//
//		// scalar from 0 to inf
//		double m_stiffness;
//
//		// behaviour of the force function. e.g. at 1 it is linear. at 2 it is quadratic. at (0, 1) it is logarithmic. at 0 it is constant
//		double m_stiffnessExponent;
//
//		// maximum force before penetration
//		double m_penetrationThreshold;
//
//		//// post-penetration parameters ///////////
//		double m_maxFrictionForce; // max friction force once penetrated
//		double m_malleability;
//
//		// where is the layer is in contact with a penetrator such as a needle, relative to its rest position
//		double m_displacementPoint;
//	};
//
//
//	// This class represents, haptically, an object with 1 or more penetrable layers
//	// holds a list of resistive layers in sequence and computes output forces
//	class HapticLayerContainer
//	{
//	public:
//		HapticLayerContainer();
//		~HapticLayerContainer();
//
//		// create numLayers HapticLayers
//		void initialize(unsigned int numLayers);
//
//	public:
//		inline chai3d::cVector3d computeForces(chai3d::cVector3d& devicePosition, double forceScalar = 1.0);
//
//		// holds the material properties and states
//		std::vector<HapticLayer> m_layerMaterials;
//
//		// set from Unity
//		chai3d::cVector3d m_entryDirection;
//		chai3d::cVector3d m_entryPoint;
//
//		// index of the last layer material that was penetrated. During needle insertion this keeps track of which layers are penetrated and which are not.
//		int m_lastLayerPenetrated; // an index of -1 means no layers are being penetrated
//
//		// number of layers to iterate through
//		int m_numLayersInUse;
//
//		bool m_enabled;
//	};
//}