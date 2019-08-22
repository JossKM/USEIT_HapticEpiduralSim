#include "PIDController.h"

#ifdef FUNCDLL_EXPORT
#define FUNCDLL_API __declspec(dllexport) 
#else
#define FUNCDLL_API __declspec(dllimport) 
#endif

namespace NeedleSimPlugin
{
	class AxialConstraint;
	class HapticLayerContainer;

	// custom AlgorithmFingerProxy meant to reduce oscillations
	class AlgorithmFingerProxyPID : public cAlgorithmFingerProxy
	{
	public:
		//! Constructor
		AlgorithmFingerProxyPID();

		//! Destructor
		virtual ~AlgorithmFingerProxyPID() {}

		virtual cVector3d computeForces(const cVector3d& a_toolPos,
			const cVector3d& a_toolVel) override;

		PIDController<cVector3d> m_PID;
		double m_dt;
	};

	class AlgorithmFingerProxyNeedle : public AlgorithmFingerProxyPID
	{
	public:
		//! Constructor
		AlgorithmFingerProxyNeedle(HapticLayerContainer* patient, AxialConstraint* axialConstraint);

		//! Destructor
		virtual ~AlgorithmFingerProxyNeedle() {}

		cVector3d computeForces(const cVector3d& a_toolPos,
			const cVector3d& a_toolVel) override;

		HapticLayerContainer* mp_patient;

		AxialConstraint* mp_axialConstraint;
	};

	extern "C"
	{

		// Axial Constraint resists movement on the 2 axes perpendicular to its own direction. Thereby forcing movement along its axis. 
		class AxialConstraint
		{
		public:
			AxialConstraint() : m_maxForce(0.0), m_direction(0.0, 0.0, 1.0) {}

			double m_maxForce;
			cVector3d m_direction;

			PIDController<cVector3d> m_controller;

			cVector3d computeForce(const cVector3d& currentPosition, const double& deltaTime);
		};


		// 1-dimensional layer. Has a state
		class HapticLayer
		{
		public:
			HapticLayer(const double& a_stiffness, const double& a_stiffnessExponent, const double& a_maxFrictionForce, const double& a_penetrationThreshold, const double& a_malleability, const double& a_depth);
			HapticLayer();

			~HapticLayer() {}

			// force applied before penetration
			// uses formula: outputForce = stiffness * displacement^stiffnessExponent
			// caps at penetration threshold; will not output any higher than that
			double computeTensionForce(const double& displacement);

			// force applied after penetration. implements a moving spring model where the contact point can move
			double computeFrictionForce(const double& displacement);

			// the resting depth of the layer, relative to the entry point of the needle
			double m_restingDepth;

			//inline void setProperties(const double& a_stiffness, const double& a_stiffnessExponent, const double& a_penetrationThreshold);

			// sets the scalar m_stiffness such that at a displacement of {distance} units, the maximum force will be achieved.
			void setStiffnessByExponentAndDistance(double a_stiffnessExponent, double a_distance, double a_penetrationThreshold);

			// called when the layer is penetrated
			void onEnterLayer();

			// called when leaving the layer
			void onExitLayer();

			// Identification number, set from Unity
			int m_id = -1;

			//// pre-penetration parameters ///////////

			// scalar from 0 to inf
			double m_stiffness;

			// behaviour of the force function. e.g. at 1 it is linear. at 2 it is quadratic. at (0, 1) it is logarithmic. at 0 it is constant
			double m_stiffnessExponent;

			// maximum force before penetration
			double m_penetrationThreshold;

			//// post-penetration parameters ///////////
			double m_maxFrictionForce; // max friction force once penetrated
			double m_malleability;

			// where is the layer is in contact with a penetrator such as a needle, relative to its rest position
			double m_displacementPoint;
		};


		// This class represents, haptically, an object with 1 or more penetrable layers
		// holds a list of resistive layers in sequence and computes output forces
		class HapticLayerContainer
		{
		public:
			HapticLayerContainer();
			~HapticLayerContainer();

			// create numLayers HapticLayers
			void initialize(unsigned int numLayers);

		public:
			inline cVector3d computeForces(cVector3d& devicePosition, double forceScalar = 1.0);

			// holds the material properties and states
			std::vector<HapticLayer> m_layerMaterials;

			// set from Unity
			cVector3d m_entryDirection;
			cVector3d m_entryPoint;

			// index of the last layer material that was penetrated. During needle insertion this keeps track of which layers are penetrated and which are not.
			int m_lastLayerPenetrated; // an index of -1 means no layers are being penetrated

			// number of layers to iterate through
			int m_numLayersInUse;

			bool m_enabled;

			bool isPenetrated();
		};

		//Custom simulation tool
		class Needle : public cGenericTool
		{
		private:
			double needleLength;

			//std::vector<cVector3d>hapticPointPositions;

			// recalculate position of haptic points based on offset along axis from tip
			//inline void calculateHapticPointPositions();

		public:
			Needle(cWorld* a_parentWorld);
			Needle(cWorld* a_parentWorld, double shaftLength, unsigned int numHapticPoints);

			~Needle();

			cHapticPoint* m_hapticTip; // tip of the needle

			// to constrain movement to a specific axis at a specific location in worldspace
			// This Axial constraint represents the penetration/needle shaft axis. The needle resists movement perpendicular to its pointy bit
			AxialConstraint axialConstraint;

			bool isForceEngaged();

			void computeInteractionForces() override;

			inline cVector3d getHapticPointPosition(size_t index);
		private:
			cVector3d m_lastForceApplied;
		};


		/////////////////////////////////////////////////////////////////////////////

		// DLL Export functions
		// Position Vectors being passed to and from the DLL are represented by arrays of double-precision floats (each is assumed to be of size 3)

		////// Setup ///

		FUNCDLL_API bool prepareHaptics(double hapticScale);

		FUNCDLL_API void startHaptics(void);
		FUNCDLL_API void stopHaptics(void);
		void updateHaptics(void);
		FUNCDLL_API void setPaused(bool pause);


		FUNCDLL_API void setToolRadius(double a_toolRadius);

		// No removing objects is allowed, since these IDs work by assuming everything is added sequentially as a child of the world, and never removed.
		// return a world ID to the added object (literally just its index in the cWorld children vector).
		// will also break if you try to parent objects using Chai3d's parenting system, since it is assumed all objects are part children of the world.
		FUNCDLL_API int addObject(double objectPos[], double objectScale[], double objectRotation[], double vertPos[][3], double normals[][3], int vertNum, int tris[][3], int triNum);

		FUNCDLL_API void setObjectProperties(int objectID, double stiffness, double friction_static, double friction_dynamic, double viscosity, double penetrationForce);
		FUNCDLL_API void addViscosityEffect(int objectID, double viscosity);

		// move all objects in the world
		FUNCDLL_API void translateObjects(double translation[]);

		// set position of workspace
		FUNCDLL_API void setHapticPosition(double position[]);

		// set orientation of workspace
		FUNCDLL_API void setHapticRotation(double rotation[]);

		FUNCDLL_API void setGlobalForce(double force[]);



		////// Patient layer system ///

		FUNCDLL_API void setHapticEntryPoint(double position[], double direction[]);

		FUNCDLL_API void clearHapticLayersFromPatient();
		//FUNCDLL_API void addHapticLayerToPatient(double a_stiffness, double a_stiffnessExponent, double a_maxFrictionForce, double a_penetrationThreshold, double a_resistanceToMovement, double a_depth);
		FUNCDLL_API void addHapticLayerToPatient();

		FUNCDLL_API void setPatientLayersEnabled(bool a_enabled);

		// is the needle inside the skin of the patient?
		FUNCDLL_API bool isPatientPenetrated();

		// where in the patient is the needle tip? Returns an ID number
		FUNCDLL_API int getLastLayerPenetratedID();

		// get the sequence index of the last layer penetrated -- i.e. if it is the third layer from the entry point its index will be 2
		FUNCDLL_API int getCurrentLayerDepth();

		//set the number of layers that are present in the needle's path into the patient
		FUNCDLL_API void setPatientNumLayersToUse(int a_numLayers);

		FUNCDLL_API void setHapticLayerProperties(int depthIndex, int layerID, double a_stiffness, double a_stiffnessExponent, double a_maxFrictionForce, double a_penetrationThreshold, double a_resistanceToMovement, double a_depth);

		// set constraint to only allow movement along a specific axis given by a direction vector. passing 0,0,0 will disable the constraint
		FUNCDLL_API void setNeedleAxialConstraintPID(double position[], double direction[], double maxForce, double kp, double ki, double kd, double gain);
		FUNCDLL_API void updateAxialConstraint(double position[], double direction[]);

		FUNCDLL_API void setNeedlePID(double kp, double ki, double kd, double gain);

		FUNCDLL_API void resetAxialConstraintPID();

		///////



		/////// utilities ///
		FUNCDLL_API bool isSimulationRunning();
		FUNCDLL_API int getNumHapticPoints();
		FUNCDLL_API void getProxyPosition(double outPosArray[3]);
		FUNCDLL_API void getDevicePosition(double outPosArray[3]);
		// returns an array of positions, each position being a fixed array
		FUNCDLL_API void getAllProxyPositions(double outPosArray[][3]);

		// check if the tool is touching a particular object by ID.	
		// object IDs start at 1 because the haptic tool is the first child of the world (index 0)
		FUNCDLL_API bool isTouching(int objectId);
		FUNCDLL_API bool isButtonPressed(int buttonId);

		// Like Unity, Chai3D uses a right handed coordinate system, but -z x y
		FUNCDLL_API void convertXYZFromCHAI3D(double inputXYZ[3]);
		FUNCDLL_API void convertXYZToCHAI3D(double inputXYZ[3]);
	}

	// simulates a spring
	inline cVector3d computeSpringForce(const cVector3d& position, cVector3d& targetPos, double& minDist, double& maxDist, double& maxForce);

	// applies perpendicular to direction
	inline cVector3d computeAxialConstraintForce(cVector3d position, cVector3d& targetPos, cVector3d& targetDir, double& minDist, double& maxDist, double& maxForce, double& kDamping);

	// applies forces parallel to direction
	inline cVector3d computeAxialSpringForce(cVector3d position, cVector3d& targetPos, cVector3d& targetDir, double& minDist, double& maxDist, double& maxForce, double& kDamping);

	// applies speed damping effect
	inline cVector3d computeDampingEffect(const double& kDamping);

	inline chai3d::cVector3d lerpd(const chai3d::cVector3d& from, const chai3d::cVector3d& to, double tValue);
}