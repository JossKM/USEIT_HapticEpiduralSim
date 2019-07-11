//------------------------------------------------------------------------------
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
//------------------------------------------------------------------------------
#include "chai3d.h"
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------
#include "UnityCHAI3DPlugin.h"
//------------------------------------------------------------------------------

// quality-of-life debugging preprocessor macros, because i'm lazy - joss
 #define HAPTIC_DEBUG

#ifdef HAPTIC_DEBUG
#define PRINTLN(x) std::cout << x << std::endl;
#define PRINTWAIT(x, time) std::cout << x << std::endl; Sleep(time);
#else
#define PRINTLN(x) ; 
#define PRINTWAIT(x, time) ;
#endif

namespace NeedleSimPlugin
{
	extern "C" {

		// a world that contains all objects of the virtual environment
		cWorld* world;

		// clock
		cPrecisionClock toolClock;

		double deltaTime;

		// a haptic device handler
		cHapticDeviceHandler* handler;

		// a pointer to the current haptic device
		cGenericHapticDevicePtr* hapticDevice;

		// a virtual tool representing the haptic device in the scene
		Needle* tool;

		// define the radius of the tool (sphere)
		double toolRadius;

		// indicates if the haptic simulation currently running
		bool simulationRunning = true;

		// indicates if the haptic simulation has terminated
		bool simulationFinished = false;

		// frequency counter to measure the simulation haptic rate
		cFrequencyCounter frequencyCounter;

		// get spec of haptic device
		cHapticDeviceInfo hapticDeviceInfo;

		HapticLayerContainer patient;

#ifdef HAPTIC_DEBUG
		bool lastForceEngagedState(false);
#endif

		bool prepareHaptics(double hapticScale)
		{
#ifdef HAPTIC_DEBUG
			FILE* pConsole;
			AllocConsole();
			freopen_s(&pConsole, "CONOUT$", "wb", stdout);
#endif

			//--------------------------------------------------------------------------
			// WORLD
			//--------------------------------------------------------------------------

			world = new cWorld();

			//--------------------------------------------------------------------------
			// HAPTIC DEVICES / TOOLS
			//--------------------------------------------------------------------------

			// create a haptic device handler
			handler = new cHapticDeviceHandler();

			// instantiate the shared pointer to the device. The reason it is created and deleted is because it does not leave scope when you stop the play session in Unity.
			hapticDevice = new cGenericHapticDevicePtr();

			// get access to the first available haptic device
			if (!handler->getDevice(*hapticDevice, 0))
				return false;

			// retrieve information about the current haptic device
			hapticDeviceInfo = (*hapticDevice)->getSpecifications();

			// create a 3D tool and add it to the world
			tool = new Needle(world, 0.1, 20);
			world->addChild(tool);

			// connect the haptic device to the tool
			tool->setHapticDevice(*hapticDevice);

			// define the radius of the tool (sphere)
			toolRadius = 0.1;

			// define a radius for the tool. Also used for collision bounding box creation
			tool->setRadius(toolRadius);

			// enable if objects in the scene are going to rotate of translate
			// or possibly collide against the tool. If the environment
			// is entirely static, you can set this parameter to "false"
			tool->enableDynamicObjects(true);

			// map the physical workspace of the haptic device to a larger virtual workspace.
			tool->setWorkspaceRadius(hapticScale);

			// haptic forces are enabled only if small forces are first sent to the device;
			// this mode avoids the force spike that occurs when the application starts when 
			// the tool is located inside an object for instance. 
			tool->setWaitForSmallForce(true);

			// start the haptic tool
			tool->start();

#ifdef HAPTIC_DEBUG
			lastForceEngagedState = tool->isForceEngaged();

			std::cout << "===[ world initialized ]===" << std::endl;
			std::cout << "model id: " << hapticDeviceInfo.m_model << std::endl;
			std::cout << "model name: " << hapticDeviceInfo.m_modelName << std::endl;
			std::cout << "manufacturer: " << hapticDeviceInfo.m_manufacturerName << std::endl;
			std::cout << "===============================\n" << std::endl;
#endif
			return true;
		}

		void startHaptics(void)
		{
			//--------------------------------------------------------------------------
			// START SIMULATION
			//--------------------------------------------------------------------------

			// create a thread which starts the main haptics rendering loop
			cThread* hapticsThread = new cThread();
			hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);
		}

		void stopHaptics(void)
		{
			// stop the simulation
			simulationRunning = false;

			// wait for graphics and haptics loops to terminate
			while (!simulationFinished) { cSleepMs(100); }

			// close haptic device
			(*hapticDevice)->close();
			tool->stop();


			//--------------------------------------------------------------------------
			// Memory cleanup
			//--------------------------------------------------------------------------

			delete handler;
			handler = nullptr;

			// clean up world. This will also delete the tool, because the tool is a child of the world
			world->deleteAllChildren();
			delete world;
			world = nullptr;
			tool = nullptr;

			// clean up device shared pointer
			delete hapticDevice;

#ifdef HAPTIC_DEBUG
			FreeConsole();
#endif
		}

		void updateHaptics(void)
		{
			// reset clock
			toolClock.reset();

			// simulation in now running
			simulationRunning = true;
			simulationFinished = false;

			// main haptic simulation loop
			while (simulationRunning)
			{
				/////////////////////////////////////////////////////////////////////
				// SIMULATION TIME
				/////////////////////////////////////////////////////////////////////

				// stop the simulation clock
				toolClock.stop();

				// read the time increment in seconds
				deltaTime = toolClock.getCurrentTimeSeconds();

				// restart the simulation clock
				toolClock.reset();
				toolClock.start();

				// update frequency counter
				frequencyCounter.signal(1);

				/////////////////////////////////////////////////////////////////////
				// HAPTIC FORCE COMPUTATION
				/////////////////////////////////////////////////////////////////////

				// compute global reference frames for each object
				world->computeGlobalPositions(true);

				// update position and orientation of tool
				tool->updateFromDevice();

				// compute interaction forces
				tool->computeInteractionForces();

				// send forces to haptic device
				tool->applyToDevice();

#ifdef HAPTIC_DEBUG

				// report on when the haptic force feedback is enabled/disabled
				{
					bool isEngaged = tool->isForceEngaged();
					if (isEngaged != lastForceEngagedState)
					{
						std::cout << "Force feedback engaged?: " << to_string(isEngaged) << std::endl;
					}
					lastForceEngagedState = isEngaged;
				}

				// report on framerate
				{
					static double time(0.0);
					static int num_samples(0);
					time += deltaTime;
					num_samples++;

					if (num_samples == 10000) // reports approx. every 10sec
					{
						std::cout << "avg framerate: " << 1.0 / (time / num_samples) << std::endl;
						num_samples = 0;
						time = 0.0;
					}
				}

#endif HAPTIC_DEBUG
			}

			// exit haptics thread
			simulationFinished = true;
		}

		void getProxyPosition(double outPosArray[])
		{
			if (simulationRunning)
			{
				outPosArray[0] = tool->m_hapticTip->getGlobalPosProxy().x();
				outPosArray[1] = tool->m_hapticTip->getGlobalPosProxy().y();
				outPosArray[2] = tool->m_hapticTip->getGlobalPosProxy().z();
				convertXYZFromCHAI3D(outPosArray);
			}
			else
			{
				outPosArray[0] = 0;
				outPosArray[1] = 0;
				outPosArray[2] = 0;
			}
		}

		void getDevicePosition(double outPosArray[])
		{
			if (simulationRunning)
			{
				outPosArray[0] = tool->m_hapticTip->getGlobalPosGoal().x();
				outPosArray[1] = tool->m_hapticTip->getGlobalPosGoal().y();
				outPosArray[2] = tool->m_hapticTip->getGlobalPosGoal().z();
				convertXYZFromCHAI3D(outPosArray);
			}
			else
			{
				outPosArray[0] = 0;
				outPosArray[1] = 0;
				outPosArray[2] = 0;
			}
		}

		void getAllProxyPositions(double outPosArray[][3])
		{
			unsigned int sz = tool->getNumHapticPoints();
			if (simulationRunning)
			{
				for (unsigned int i = 0; i < sz; i++)
				{
					try {
					outPosArray[i][0] = tool->getHapticPoint(i)->getGlobalPosGoal().y();
					outPosArray[i][1] = tool->getHapticPoint(i)->getGlobalPosGoal().z();
					outPosArray[i][2] = -1.0 * tool->getHapticPoint(i)->getGlobalPosGoal().x();


					}
					catch (exception e)
					{
						PRINTWAIT("Something went wrong in GetProxyPositions?", 10000)
					}
				}
			}
			else
			{
				for (unsigned int i = 0; i < sz; i++)
				{
					outPosArray[i][0] = 0.0;
					outPosArray[i][1] = 0.0;
					outPosArray[i][2] = 0.0;
				}
			}
		}

		bool isTouching(int objectId)
		{
			return tool->m_hapticTip->isInContact(world->getChild(objectId));
		}

		bool isButtonPressed(int buttonId)
		{
			return tool->getUserSwitch(buttonId);
		}

		void setToolRadius(double a_toolRadius)
		{
			toolRadius = a_toolRadius;
			tool->setRadius(a_toolRadius);
		}

		int addObject(double objectPos[], double objectScale[], double objectRotation[], double vertPos[][3], double normals[][3], int vertNum, int triPos[][3], int triNum)
		{
			// read the scale factor between the physical workspace of the haptic
			// device and the virtual workspace defined for the tool
			double workspaceScaleFactor = tool->getWorkspaceScaleFactor();

			cMesh* object = new cMesh();

			// set vertices
			for (int i = 0; i < vertNum; i++)
			{
				int vertex = object->newVertex();

				convertXYZToCHAI3D(vertPos[i]);
				cVector3d vertPosVecotor3 = cVector3d(vertPos[i][0], vertPos[i][1], vertPos[i][2]);
				convertXYZToCHAI3D(normals[i]);
				cVector3d vertNormalVecotor3 = cVector3d(normals[i][0], normals[i][1], normals[i][2]);

				object->m_vertices->setLocalPos(vertex, vertPosVecotor3);
				object->m_vertices->setNormal(vertex, vertNormalVecotor3);
			}

			// set triangles
			for (int i = 0; i < triNum; i++)
			{
				object->newTriangle(triPos[i][2], triPos[i][1], triPos[i][0]);
			}

			// set the position of the object at the center of the world
			convertXYZToCHAI3D(objectPos);
			object->setLocalPos(objectPos[0], objectPos[1], objectPos[2]);

			// scale object
			object->scaleXYZ(objectScale[2], objectScale[0], objectScale[1]);

			// rotate object
			object->rotateExtrinsicEulerAnglesDeg(objectRotation[2], -1 * objectRotation[0], -1 * objectRotation[1], C_EULER_ORDER_XYZ);

			// stiffness property
			double maxStiffness = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor;

			// define a default stiffness for the object
			object->m_material->setStiffness(0.3 * maxStiffness);

			// define some static friction
			object->m_material->setStaticFriction(0.5);

			// define some dynamic friction
			object->m_material->setDynamicFriction(0.5);

			// render triangles haptically
			object->m_material->setHapticTriangleSides(true, false);

			// disable culling
			object->setUseCulling(false, true);

			// compute a boundary box
			object->computeBoundaryBox(true);

			// compute collision detection algorithm
			object->createAABBCollisionDetector(toolRadius);

			// add object to world
			world->addChild(object);

			int objectID = (world->getNumChildren() - 1);

			return objectID;
		}

		void setObjectProperties(int objectID, double stiffness, double friction_static, double friction_dynamic, double viscosity, double penetrationForce)
		{
			cGenericObject* object = world->getChild(objectID);

			// read the scale factor between the physical workspace of the haptic
			// device and the virtual workspace defined for the tool
			double workspaceScaleFactor = tool->getWorkspaceScaleFactor();

			// stiffness properties
			double maxStiffness = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor;

			// stiffness for the object in N/m
			object->m_material->setStiffness(stiffness * maxStiffness);

			// coefficient of static friction
			object->m_material->setStaticFriction(friction_static);

			// coefficient of dynamic friction
			object->m_material->setDynamicFriction(friction_dynamic);

			// viscosity (only active if a viscosity effect is also on it)
			object->m_material->setViscosity(viscosity);

			// force to penetrate
			object->m_material->setPenetrationForceThreshold(penetrationForce);
		}

		void addViscosityEffect(int objectID, double viscosity)
		{
			cGenericObject* object = world->getChild(objectID);

			//create and link effect
			object->addEffect(new cEffectViscosity(object));
			object->m_material->setViscosity(viscosity); // set property
		}

		void translateObjects(double translation[])
		{
			convertXYZToCHAI3D(translation);
			int num = world->getNumChildren();
			for (int i = 0; i < num; i++)
			{
				if (tool != world->getChild(i))
				{
					world->getChild(i)->translate(translation[0], translation[1], translation[2]);
				}
			}
		}

		void setHapticPosition(double position[])
		{
			convertXYZToCHAI3D(position);
			tool->setLocalPos(position[0], position[1], position[2]);
		}

		void setHapticRotation(double rotation[])
		{
			tool->setLocalRot(cMatrix3d(rotation[2], -1 * rotation[0], -1 * rotation[1], C_EULER_ORDER_XYZ));
		}

		void setGlobalForce(double force[])
		{
			convertXYZToCHAI3D(force);
			tool->setDeviceGlobalForce(cVector3d(force));
		}

		void setNeedleAxialConstraintPID(double position[], double direction[], double maxForce, double kp, double ki, double kd, double gain)
		{
			convertXYZToCHAI3D(position);
			convertXYZToCHAI3D(direction);

			cVector3d goalDir = cVector3d(direction);
			tool->axialConstraint.m_direction = goalDir;

			cVector3d goalPosition = cVector3d(position);
			goalPosition.projectToPlane(goalDir);
			tool->axialConstraint.m_controller.setGoal(goalPosition);

			tool->axialConstraint.m_controller.configure(kp, ki, kd, gain);

			tool->axialConstraint.m_maxForce = maxForce;
		}

		void setNeedlePID(double kp, double ki, double kd, double gain)
		{
			for (int i = 0; i < tool->getNumHapticPoints(); i++)
			{
				((AlgorithmFingerProxyPID*)(tool->getHapticPoint(i)->m_algorithmFingerProxy))->m_PID.configure(kp, ki, kd, gain);
			}
		}

		void resetAxialConstraintPID()
		{
			tool->axialConstraint.m_controller.reset<cVector3d>();
		}

		void setHapticEntryPoint(double position[], double direction[])
		{
			convertXYZToCHAI3D(position);
			convertXYZToCHAI3D(direction);

			patient.m_entryPoint = cVector3d(position);
			patient.m_entryDirection = cVector3d(direction);
			//PRINTLN("entry point set!")
		}

		void clearHapticLayersFromPatient()
		{
			patient.m_layerMaterials.clear();
		}

		void addHapticLayerToPatient(double a_stiffness, double a_stiffnessExponent, double a_maxFrictionForce, double a_penetrationThreshold, double a_resistanceToMovement, double a_depth)
		{
			HapticLayer layer = HapticLayer(a_stiffness, a_stiffnessExponent, a_maxFrictionForce, a_penetrationThreshold, a_resistanceToMovement, a_depth);
			patient.m_layerMaterials.push_back(layer);
		}

		void setPatientLayersEnabled(bool a_enabled)
		{
			patient.m_enabled = a_enabled;
		}

		bool isPatientPenetrated()
		{
			return patient.m_lastLayerPenetrated > -1;
		}

		int getLastLayerPenetrated()
		{
			return patient.m_layerMaterials[patient.m_lastLayerPenetrated].m_id;
		}

		void setPatientNumLayersToUse(int a_numLayers)
		{
			patient.m_numLayersInUse = a_numLayers;
		}

		void setHapticLayerProperties(int depthIndex, int layerID, double a_stiffness, double a_stiffnessExponent, double a_maxFrictionForce, double a_penetrationThreshold, double a_resistanceToMovement, double a_depth)
		{
			patient.m_layerMaterials[depthIndex].m_id = layerID;
			patient.m_layerMaterials[depthIndex].m_stiffness = a_stiffness;
			patient.m_layerMaterials[depthIndex].m_stiffnessExponent = a_stiffnessExponent;
			patient.m_layerMaterials[depthIndex].m_maxFrictionForce = a_maxFrictionForce;
			patient.m_layerMaterials[depthIndex].m_penetrationThreshold = a_penetrationThreshold;
			patient.m_layerMaterials[depthIndex].m_malleability = a_resistanceToMovement;
			patient.m_layerMaterials[depthIndex].m_restingDepth = a_depth;
		}

		//--------------------------------------------------------------------------
		// Utils
		//--------------------------------------------------------------------------

		void convertXYZFromCHAI3D(double inputXYZ[])
		{
			double val0 = inputXYZ[0];
			double val1 = inputXYZ[1];
			double val2 = inputXYZ[2];

			inputXYZ[0] = val1;
			inputXYZ[1] = val2;
			inputXYZ[2] = -1.0 * val0;
		}

		 void convertXYZToCHAI3D(double inputXYZ[])
		{
			double val0 = inputXYZ[0];
			double val1 = inputXYZ[1];
			double val2 = inputXYZ[2];

			inputXYZ[0] = -1.0 * val2;
			inputXYZ[1] = val0;
			inputXYZ[2] = val1;
		}



		///////////////////////////////////////////
		// Needle
		///////////////////////////////////////////

		void Needle::computeInteractionForces()
		{
			// for each haptic point compute the interaction force
			// and combine their overall contribution to compute the output force
			// and torque to be sent to the haptic device

			cVector3d interactionForce(0.0, 0.0, 0.0);
			cVector3d globalTorque(0.0, 0.0, 0.0);

			// determine pivot point for torque calculation 
			
			cVector3d pivotPoint;
			if (isPatientPenetrated())
			{
				//(where the needle enters an object)
				pivotPoint = axialConstraint.m_controller.getGoal<cVector3d>();
			}
			else
			{
				// if the needle is not inside something, then pivot about its centre of mass, approximated to the base of the needle)
				pivotPoint = hapticPointPositions[m_hapticPoints.size() - 1];
			}

			// update haptic point positions
			calculateHapticPointPositions();

			// compute haptic mesh forces (solid object collision) based on multiple haptic points representing the whole needle shaft
			cVector3d forceFromHapticMeshes(0.0, 0.0, 0.0);
			unsigned int sz = m_hapticPoints.size();
			for (unsigned i = 0; i < sz; i++)
			{
				// get next haptic point
				cHapticPoint* nextContactPoint = m_hapticPoints[i];

				// compute force at haptic point as well as new proxy position
				forceFromHapticMeshes += nextContactPoint->computeInteractionForces(
					hapticPointPositions[i],
					m_deviceGlobalRot,
					m_deviceGlobalLinVel,
					m_deviceGlobalAngVel);

				// calculate torque
				cVector3d momentArm = pivotPoint - hapticPointPositions[i];
				globalTorque.add(cCross(momentArm, forceFromHapticMeshes));
			}

			// combine force contributions together
			interactionForce.add(forceFromHapticMeshes);


			///

			/// single haptic point force model
			// compute interaction forces at haptic point in global coordinates
			//cVector3d forceFromHapticMeshes = m_hapticTip->computeInteractionForces(m_deviceGlobalPos,
			//	m_deviceGlobalRot,
			//	m_deviceGlobalLinVel,
			//	m_deviceGlobalAngVel);
			//interactionForce += forceFromHapticMeshes;

			cVector3d devicePos = m_hapticTip->m_algorithmFingerProxy->getDeviceGlobalPosition();

			// forces keeping needle constrained to entry point
			interactionForce += axialConstraint.computeForce(devicePos, deltaTime);

			// forces of skin layers on needle tip
			interactionForce += patient.computeForces(devicePos);

			//apply an slight overall damping effect to (hopefully) reduce unwanted oscillations
			interactionForce += computeDampingEffect(1.0);

			// oscillation failsafe: stop applying forces if velocity is too high
			// > 1m/s could be considered unsafe
			if (tool->getDeviceGlobalLinVel().length() > 1.0)
			{
				interactionForce *= 0.0;
				PRINTLN("oscillation failsafe activated!")
			}

			// generic damping
			interactionForce -= tool->getDeviceGlobalLinVel() * 1.0;

			// avoid trying to apply more force than possible
			interactionForce.clamp(hapticDeviceInfo.m_maxLinearForce);

			// smooth forces by blending between new output and last output
			if (interactionForce.lengthsq() > 0.0001)
			{
				interactionForce = lerpd(m_lastForceApplied, interactionForce, 0.9);
			}

			setDeviceGlobalForce(interactionForce);
			setDeviceGlobalTorque(globalTorque);
			setGripperForce(0.0);
			//if(globalTorque.lengthsq() > 0.0) PRINTLN("torque: " << globalTorque)

			m_lastForceApplied = interactionForce;
		}

		bool Needle::isForceEngaged()
		{
			return m_forceEngaged;
		}

		void Needle::calculateHapticPointPositions()
		{
			cVector3d needleShaftAxis;

			// If device supports rotation, get data directly from device, otherwise must come from somewhere else
			if (m_hapticDevice->m_specifications.m_sensedRotation)
			{
				// direction of rotation axis toward screen (x-axis)
				needleShaftAxis = getGlobalRot().getCol0();
			}
			else
			{
				needleShaftAxis = axialConstraint.m_direction;
			}

			unsigned int sz = m_hapticPoints.size();
			for (unsigned i = 0; i < sz; i++)
			{
				double offset = needleLength / sz * (double)i;
				//PRINTLN(".....offset: " << offset)
				//PRINTLN("needleShaftAxis" << needleShaftAxis)

				// position of haptic point is based on offset along axis from tip
				hapticPointPositions[i] = m_deviceGlobalPos - (needleShaftAxis * offset);
			}
		}

		inline cVector3d Needle::getHapticPointPosition(size_t index)
		{
			return hapticPointPositions.at(index);
		}

		Needle::Needle(cWorld * a_parentWorld) : cGenericTool(a_parentWorld)
		{
			hapticPointPositions.resize(1);

			needleLength = 0.0;

			m_lastForceApplied = 0.0;

			// initialize haptic interaction point
			cHapticPoint* newPoint = new cHapticPoint(this);

			// replace fingerproxy algorithm with custom version
			delete newPoint->m_algorithmFingerProxy;
			newPoint->m_algorithmFingerProxy = new AlgorithmFingerProxyPID();

			m_hapticPoints.push_back(newPoint);

			m_hapticTip = m_hapticPoints[0];
		}

		Needle::Needle(cWorld * a_parentWorld, double a_shaftLength, unsigned int a_numHapticPoints) : cGenericTool(a_parentWorld)
		{
			m_lastForceApplied = 0.0;
			needleLength = a_shaftLength;


#ifdef _DEBUG
			PRINTLN("creating" << a_numHapticPoints << " haptic points...")
#endif

			// initialize haptic interaction points
			for (unsigned int i = 0; i < a_numHapticPoints; i++)
			{
				cHapticPoint* newPoint = new cHapticPoint(this);

				//if (i == 0)
				//{
					// replace fingerproxy algorithm with custom version
				//delete newPoint->m_algorithmFingerProxy;
				//newPoint->m_algorithmFingerProxy = new AlgorithmFingerProxyPID();
				//}
			
				hapticPointPositions.push_back(cVector3d(0.0, 0.0, 0.0));

				m_hapticPoints.push_back(newPoint);
				//PRINTLN("number of hapticPoints: " << m_hapticPoints.size())
				//PRINTLN("number of hapticPointPositions: " << hapticPointPositions.size())
			}

			m_hapticTip = m_hapticPoints[0];
		}

		Needle::~Needle()
		{
			for (auto point : m_hapticPoints)
			{
				delete point;
				point = nullptr;
			}

			m_hapticPoints.clear();
		}

	} // extern 

	inline cVector3d computeSpringForce(const cVector3d& position, cVector3d& springRestPosition, double& minDist, double& maxDist, double& maxForce)
	{
		cVector3d displacementToTarget = springRestPosition - position;
		double dist = displacementToTarget.length();
		displacementToTarget.normalize();

		double forceMagnitude = lmapd(dist, minDist, maxDist, 0.0, maxForce);
		forceMagnitude = cClamp(forceMagnitude, 0.0, maxForce);

		cVector3d springForce = displacementToTarget * forceMagnitude;

		return springForce;
	}

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


	//cPenetrablePoint::cPenetrablePoint(cGenericTool * a_parentTool) : cHapticPoint(a_parentTool)
	//{
	//	// replace finger-proxy algorithm used for modelling contacts with 
	//	// cMesh objects.
	//	delete m_algorithmFingerProxy;
	//	m_algorithmFingerProxy = new cAlgorithmFingerProxyPuncture();
	//}
	//
	//cPenetrablePoint::~cPenetrablePoint()
	//{
	//}

	inline cVector3d computeAxialConstraintForce(cVector3d position, cVector3d & targetPos, cVector3d & targetDir, double & minDist, double & maxDist, double & maxForce, double& kDamping)
	{
		cVector3d displacementToTarget = targetPos - position;
		displacementToTarget = displacementToTarget.projectToPlane(targetDir);
		double dist = displacementToTarget.length();
		displacementToTarget.normalize();

		// spring
		double forceMagnitude = lmapd(dist, minDist, maxDist, 0.0, maxForce);
		cVector3d springForce = displacementToTarget * forceMagnitude;

		// speed damping
		cVector3d vel = tool->getDeviceGlobalLinVel().projectToPlane(targetDir);;
		springForce -= kDamping * vel * maxForce;

		springForce.clamp(maxForce);
		return springForce;
	}

	inline cVector3d computeAxialSpringForce(cVector3d position, cVector3d & targetPos, cVector3d & targetDir, double & minDist, double & maxDist, double & maxForce, double & kDamping)
	{
		cVector3d displacementToTarget = targetPos - position;
		displacementToTarget = displacementToTarget.projectToVector(targetDir);
		double dist = displacementToTarget.length();
		displacementToTarget.normalize();

		// spring
		double forceMagnitude = lmapd(dist, minDist, maxDist, 0.0, maxForce);
		cVector3d springForce = displacementToTarget * forceMagnitude;

		// speed damping
		cVector3d vel = tool->getDeviceGlobalLinVel().projectToVector(targetDir);;
		springForce -= kDamping * vel * maxForce;

		springForce.clamp(maxForce);
		return springForce;
	}

	cVector3d computeDampingEffect(const double & kDamping)
	{
		return -kDamping * tool->getDeviceGlobalLinVel();
	}


	HapticLayerContainer::HapticLayerContainer()
	{
		m_lastLayerPenetrated = -1;
		initialize(10);
	}

	HapticLayerContainer::~HapticLayerContainer()
	{
	}

	void HapticLayerContainer::initialize(unsigned int numLayers)
	{
		m_layerMaterials.clear();
		m_enabled = true;

		m_lastLayerPenetrated = -1;
		m_numLayersInUse = 0;

		//create a default dataset of dummy layers
		for (int i = 0; i < numLayers; i++)
		{
			// add material
			HapticLayer layer = HapticLayer(0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
			m_layerMaterials.push_back(layer);
		}
	}

	inline cVector3d HapticLayerContainer::computeForces(cVector3d & devicePosition, double forceScalar)
	{
		if (m_enabled)
		{
			// what is the displacement parallel to the layers?
			cVector3d displacement = (devicePosition - m_entryPoint);

			double penetrationDepth = displacement.dot(m_entryDirection);

			double netForceMagnitude = 0.0;

			// for each layer, determine if the layer is already being penetrated.
			// If it has, then apply the friction force. If not, then apply the tension force.
			int numLayers = (int)m_layerMaterials.size();
			for (int layerIdx = 0; layerIdx < numLayers; layerIdx++)
			{
				auto layer = &m_layerMaterials[layerIdx];

				if (m_lastLayerPenetrated >= layerIdx)
				{ // penetrated behaviour

				  // if behind the material's depth
					if (penetrationDepth < layer->m_restingDepth)
					{
						//it is no longer penetrated.
						m_lastLayerPenetrated = max(m_lastLayerPenetrated - 1, -1);
						layer->onExitLayer();

						//PRINTLN("exited layer" << layerIdx)
							//any materials after it are also no longer penetrated.
					}

					//PRINTLN("last layer: " << m_lastLayerPenetrated << "| pen depth: " << penetrationDepth)

					// if this layer is among the active set, apply its forces
					if (layerIdx < m_numLayersInUse)
					{
						double force = layer->computeFrictionForce(penetrationDepth);
						netForceMagnitude += force;
					}

					//PRINTLN("inside layer" + layerIdx)
				}
				else
				{ // not-yet-penetrated behaviour


				  // Only allow surface touching of the next layer after the last layer penetrated--no skipping layers allowed
					if (layerIdx != m_lastLayerPenetrated + 1)
					{
						break;
					}


					// if this layer is among the active set, apply its forces. 
					if (layerIdx < m_numLayersInUse)
					{
						double displacement = penetrationDepth - layer->m_restingDepth;

						// determine where the haptic tip is in relation to the layer's rest position in 1D.
						double force = layer->computeTensionForce(displacement);


						// if the force is enough to penetrate it, go through.
						if (force >= layer->m_penetrationThreshold && displacement > 0.0)
						{
							m_lastLayerPenetrated = layerIdx;
							layer->onEnterLayer();

							//PRINTLN("penetrated layer" << layerIdx)
						}

						netForceMagnitude += force;
					}
				}
			}

			cVector3d outputForce = m_entryDirection;
			outputForce.normalize();
			outputForce.mul(-netForceMagnitude);

			return outputForce;
		}
		else
		{
			return cVector3d(0.0);
		}
	}

	HapticLayer::HapticLayer(const double & a_stiffness, const double & a_stiffnessExponent, const double & a_maxFrictionForce, const double & a_penetrationThreshold, const double& a_resistanceToMovement, const double & a_depth) :
		m_stiffness(a_stiffness),
		m_stiffnessExponent(a_stiffnessExponent),
		m_maxFrictionForce(a_maxFrictionForce),
		m_penetrationThreshold(a_penetrationThreshold),
		m_malleability(a_resistanceToMovement),
		m_restingDepth(a_depth),
		m_displacementPoint(0.0)
	{
	}

	HapticLayer::HapticLayer()
	{
		m_stiffnessExponent = 1.0;
		m_maxFrictionForce = 0.1;
		m_penetrationThreshold = 1.0;
		m_displacementPoint = 0.0;
		m_restingDepth = 0.0;
		m_malleability = 100.0;
	}
	double HapticLayer::computeTensionForce(const double& displacement)
	{
		double outputForce = 0.0;

		// the layer is receiving pressure
		if (displacement > 0.0)
		{
			outputForce = min(m_stiffness * std::pow(displacement, m_stiffnessExponent), m_penetrationThreshold);
		}

		return outputForce;
	}

	double HapticLayer::computeFrictionForce(const double & penetrationDepth)
	{
		double displacement = penetrationDepth - (m_restingDepth + m_displacementPoint);

		// compute the resistance force.
		double distance = abs(displacement); //pow function needs to take a positive number
		double absoluteForce = m_stiffness * std::pow(distance, m_stiffnessExponent);

		// positive direction is inward, negative direction is outward
		double direction = (displacement < 0.0) ? -1.0 : 1.0;

		// the force was calculated with an absolute value. Reverse if needed.
		double outputForce = min(absoluteForce, m_maxFrictionForce) * direction;

		// move the layer to meet the contact point
		m_displacementPoint = lerp(m_displacementPoint, penetrationDepth - m_restingDepth,
			min((deltaTime * m_malleability), 1.0));

		return outputForce;
	}

	void HapticLayer::setStiffnessByExponentAndDistance(double a_stiffnessExponent, double distance, double a_penetrationThreshold)
	{
		m_stiffnessExponent = a_stiffnessExponent;
		m_penetrationThreshold = a_penetrationThreshold;
		m_stiffness = m_penetrationThreshold / (std::pow(distance, m_stiffnessExponent));
	}

	void HapticLayer::onEnterLayer()
	{
		m_displacementPoint = 0.0;
	}

	void HapticLayer::onExitLayer()
	{
	}

	// uses projection of position onto the plane on which AxialConstraint acts
	cVector3d AxialConstraint::computeForce(const cVector3d& currentPosition, const double & deltaTime)
	{
		cVector3d positionProjected = currentPosition;
		positionProjected = positionProjected.projectToPlane(m_direction);

		m_controller.update(positionProjected, deltaTime);
		cVector3d outputForce = m_controller.getOutput<cVector3d>().projectToPlane(m_direction);

		outputForce.clamp(m_maxForce);

		return outputForce;
	}

	AlgorithmFingerProxyPID::AlgorithmFingerProxyPID()
	{

	}

	cVector3d AlgorithmFingerProxyPID::computeForces(const cVector3d & a_toolPos, const cVector3d & a_toolVel)
	{
		// update device position
		m_deviceGlobalPos = a_toolPos;

		// check if world has been defined; if so, compute forces
		if (m_world != NULL)
		{
			// compute next best position of proxy
			computeNextBestProxyPosition(m_deviceGlobalPos);

			// update proxy to next best position
			m_proxyGlobalPos = m_nextBestProxyGlobalPos;

			// compute force vector applied to device
			updateForce();

			// if no force, reset PID state
			if (m_lastGlobalForce.lengthsq() == 0.0)
			{
				m_PID.reset<cVector3d>();
			}

			// calculate PID output
			m_PID.setGoal(m_nextBestProxyGlobalPos);
			m_PID.update(m_deviceGlobalPos, 0.001);

			// augment force output with PID output
			m_lastGlobalForce *= m_PID.m_kp;
			m_lastGlobalForce += m_PID.getIntegralOutput<cVector3d>() + m_PID.getDerivativeOutput<cVector3d>();
			m_lastGlobalForce *= m_PID.m_gain;


			// return result
			return (m_lastGlobalForce);
		}

		// if no world has been defined in which algorithm operates, there is no force
		else
		{
			return (cVector3d(0.0, 0.0, 0.0));
		}
	}
//------------------------------------------------------------------------------
} // namespace NeedleSimPlugin
//------------------------------------------------------------------------------
