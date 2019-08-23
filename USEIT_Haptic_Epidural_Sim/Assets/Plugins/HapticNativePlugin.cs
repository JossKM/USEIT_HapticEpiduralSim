// imports the C++ DLL code, includes wrapper functions for easy use in Unity.

using System;
using System.Runtime.InteropServices;
using System.Text;
using UnityEngine;

public class HapticNativePlugin
{

    [DllImport("UnityPlugin")]
    protected static extern void getProxyPosition(double[] array);

    public static Vector3 GetProxyPosition()
    {
        double[] arrayToUse = new double[3];
        getProxyPosition(arrayToUse);
        return new Vector3((float) arrayToUse[0], (float) arrayToUse[1], (float) arrayToUse[2]);
    }

    [DllImport("UnityPlugin")]
    protected static extern void getAllProxyPositions(double[,] array, int size);

    public static Vector3[] GetAllProxyPositions(uint size)
    {
        double[,] arrayToUse = new double[size,3];
        getAllProxyPositions(arrayToUse, (int)size);

        Vector3[] ret = new Vector3[size];
        for(uint i = 0; i < size; i++)
        {
            ret[i] = new Vector3((float)arrayToUse[i,0], (float)arrayToUse[i,1], (float)arrayToUse[i,2]);
        }

        return ret;
    }

    [DllImport("UnityPlugin")]
    protected static extern void getDevicePosition(double[] array);

    public static Vector3 GetDevicePosition()
    {
        double[] arrayToUse = new double[3];
        getDevicePosition(arrayToUse);
        return new Vector3((float) arrayToUse[0], (float) arrayToUse[1], (float) arrayToUse[2]);
    }

    public static bool IsInContact()
    {
        var pos1 = GetProxyPosition();
        var pos2 = GetDevicePosition();
        return !(pos1.x == pos2.x && pos1.y == pos2.y && pos1.y == pos2.y);
    }

    [DllImport("UnityPlugin")]
    protected static extern int addObject(double[] objectPos, double[] objectScale, double[] objectRotation,
        double[,] vertPos, double[,] normals, int vertNum, int[,] tris, int triNum);

    public static int AddObject(Vector3 position, Vector3 scale, Vector3 rotation, Vector3[] vertPos, Vector3[] normals,
        int vertNum, int[,] tris, int triNum)
    {
        double[] objectPosition = new double[3];
        objectPosition[0] = (double) position.x;
        objectPosition[1] = (double) position.y;
        objectPosition[2] = (double) position.z;

        double[] objectScale = new double[3];
        objectScale[0] = (double) scale.x;
        objectScale[1] = (double) scale.y;
        objectScale[2] = (double) scale.z;

        double[] objectRotation = new double[3];
        objectRotation[0] = (double) rotation.x;
        objectRotation[1] = (double) rotation.y;
        objectRotation[2] = (double) rotation.z;

        double[,] objectVertPos = new double[vertNum, 3];
        for (int i = 0; i < vertNum; i++)
        {
            objectVertPos[i, 0] = (double) vertPos[i].x;
            objectVertPos[i, 1] = (double) vertPos[i].y;
            objectVertPos[i, 2] = (double) vertPos[i].z;
        }

        double[,] objectNormals = new double[vertNum, 3];
        for (int i = 0; i < vertNum; i++)
        {
            objectNormals[i, 0] = (double) normals[i].x;
            objectNormals[i, 1] = (double) normals[i].y;
            objectNormals[i, 2] = (double) normals[i].z;
        }
        

        return addObject(objectPosition, objectScale, objectRotation, objectVertPos, objectNormals, vertNum, tris, triNum); ;
    }

    [DllImport("UnityPlugin")]
    protected static extern int addBoxObject(double[] objectPos, double[] objectScale, double[] objectRotation);
    public static int AddBoxObject(Vector3 position, Vector3 scale, Vector3 rotation)
    {
        double[] objectPosition = new double[3];
        objectPosition[0] = (double)position.x;
        objectPosition[1] = (double)position.y;
        objectPosition[2] = (double)position.z;

        double[] objectScale = new double[3];
        objectScale[0] = (double)scale.x;
        objectScale[1] = (double)scale.y;
        objectScale[2] = (double)scale.z;

        double[] objectRotation = new double[3];
        objectRotation[0] = (double)rotation.x;
        objectRotation[1] = (double)rotation.y;
        objectRotation[2] = (double)rotation.z;

        return addBoxObject(objectPosition, objectScale, objectRotation);
    }

    [DllImport("UnityPlugin")]
    protected static extern void translateObjects(double[] translation);

    public static void TranslateObjects(Vector3 translation)
    {
        double[] objectPos = new double[3];
        objectPos[0] = (double) translation.x;
        objectPos[1] = (double) translation.y;
        objectPos[2] = (double) translation.z;
        translateObjects(objectPos);
    }

    [DllImport("UnityPlugin")]
    protected static extern void setHapticPosition(double[] position);

    public static void SetHapticPosition(Vector3 position)
    {
        double[] objectPos = new double[3];
        objectPos[0] = (double) position.x;
        objectPos[1] = (double) position.y;
        objectPos[2] = (double) position.z;
        setHapticPosition(objectPos);
    }

    [DllImport("UnityPlugin")]
    protected static extern void setHapticRotation(double[] rotation);

    public static void SetHapticRotation(Vector3 rotation)
    {
        double[] objectPos = new double[3];
        objectPos[0] = (double) (Mathf.Deg2Rad * rotation.x);
        objectPos[1] = (double) (Mathf.Deg2Rad * rotation.y);
        objectPos[2] = (double) (Mathf.Deg2Rad * rotation.z);
        setHapticRotation(objectPos);
    }

    [DllImport("UnityPlugin")]
    protected static extern void setSpringProperties(bool enabled, double[] position, double minDist, double maxDist,
        double maxForce);

    public static void SetSpringProperties(bool enabled, Vector3 anchorPosition, double minDist, double maxDist,
        double maxForce)
    {
        double[] anchorPos = new double[3];
        anchorPos[0] = (double) (anchorPosition.x);
        anchorPos[1] = (double) (anchorPosition.y);
        anchorPos[2] = (double) (anchorPosition.z);
        setSpringProperties(enabled, anchorPos, minDist, maxDist, maxForce);
    }
    
    [DllImport("UnityPlugin")]
    protected static extern void setNeedleAxialConstraint(bool enabled, double[] position, double[] direction, double minDist,
        double maxDist, double maxForce, double damping);

    public static void SetAxialConstraint(bool enabled, Vector3 anchorPosition, Vector3 anchorDirection, double minDist,
        double maxDist, double maxForce, double damping)
    {
        double[] anchorPos = new double[3];
        double[] anchorDir = new double[3];
        anchorPos[0] = (double) (anchorPosition.x);
        anchorPos[1] = (double) (anchorPosition.y);
        anchorPos[2] = (double) (anchorPosition.z);

        anchorDir[0] = (double) (anchorDirection.x);
        anchorDir[1] = (double) (anchorDirection.y);
        anchorDir[2] = (double) (anchorDirection.z);
        setNeedleAxialConstraint(enabled, anchorPos, anchorDir, minDist, maxDist, maxForce, damping);
    }

    [DllImport("UnityPlugin")]
    protected static extern void setHapticEntryPoint(double[] position, double[] direction);

    public static void SetHapticEntryPoint(Vector3 anchorPosition, Vector3 anchorDirection)
    {
        
        double[] anchorPos = new double[3];
        double[] anchorDir = new double[3];
        anchorPos[0] = (double)(anchorPosition.x);
        anchorPos[1] = (double)(anchorPosition.y);
        anchorPos[2] = (double)(anchorPosition.z);

        anchorDir[0] = (double)(anchorDirection.x);
        anchorDir[1] = (double)(anchorDirection.y);
        anchorDir[2] = (double)(anchorDirection.z);
        setHapticEntryPoint(anchorPos, anchorDir);
    }

    [DllImport("UnityPlugin")]
    protected static extern void setNeedleAxialSpring(bool enabled, double[] position, double[] direction, double minDist,
       double maxDist, double maxForce, double damping);

    public static void SetNeedleAxialSpring(bool enabled, Vector3 anchorPosition, Vector3 anchorDirection, double minDist,
        double maxDist, double maxForce, double damping)
    {
        double[] anchorPos = new double[3];
        double[] anchorDir = new double[3];
        anchorPos[0] = (double)(anchorPosition.x);
        anchorPos[1] = (double)(anchorPosition.y);
        anchorPos[2] = (double)(anchorPosition.z);

        anchorDir[0] = (double)(anchorDirection.x);
        anchorDir[1] = (double)(anchorDirection.y);
        anchorDir[2] = (double)(anchorDirection.z);
        setNeedleAxialConstraint(enabled, anchorPos, anchorDir, minDist, maxDist, maxForce, damping);
    }

	[DllImport("UnityPlugin")]
	protected static extern void setNeedleAxialConstraintPID(double[] position, double[] direction, double maxForce, double kp, double ki, double kd, double gain);
	
	public static void SetNeedleAxialPID(Vector3 anchorPosition, Vector3 anchorDirection, double maxForce, double kp, double ki, double kd, double gain)
	{
		double[] anchorPos = new double[3];
		double[] anchorDir = new double[3];
		anchorPos[0] = (double)(anchorPosition.x);
		anchorPos[1] = (double)(anchorPosition.y);
		anchorPos[2] = (double)(anchorPosition.z);
	
		anchorDir[0] = (double)(anchorDirection.x);
		anchorDir[1] = (double)(anchorDirection.y);
		anchorDir[2] = (double)(anchorDirection.z);
	
		setNeedleAxialConstraintPID(anchorPos, anchorDir, maxForce, kp, ki, kd, gain);
	}

    // wrapperless

    [DllImport("UnityPlugin")]
    public static extern bool prepareHaptics(double hapticScale);

    [DllImport("UnityPlugin")]
    public static extern bool isSimulationRunning();

    [DllImport("UnityPlugin")]
    public static extern void startHaptics();

    [DllImport("UnityPlugin")]
    public static extern void stopHaptics();

    [DllImport("UnityPlugin")]
    public static extern bool isButtonPressed(int buttonId);

    [DllImport("UnityPlugin")]
    public static extern bool isTouching(int objectId);

    [DllImport("UnityPlugin")]
    public static extern void setToolRadius(double a_toolRadius);

    [DllImport("UnityPlugin")]
    public static extern void setObjectProperties(int objectID, double stiffness, double friction_static,
        double friction_dynamic, double viscosity, double penetrationForce);

    [DllImport("UnityPlugin")]
    public static extern void addViscosityEffect(int objectID, double viscosity);

    [DllImport("UnityPlugin")]
    public static extern void addMembraneEffect(int objectID, double a_resistance, double a_friction_static, double a_friction_dynamic, double maxForce, double distanceToMaxForce, double a_springMass, double a_penetrationThreshold);

    [DllImport("UnityPlugin")]
    public static extern void clearHapticLayersFromPatient();

    //[DllImport("UnityPlugin")]
    //public static extern void addHapticLayerToPatient(double a_stiffness, double a_stiffnessExponent, double a_maxFrictionForce, double a_penetrationThreshold, double a_resistanceToMovement);


    //[DllImport("UnityPlugin")]
    //public static extern void addHapticLayerToPatient();
    
    //[DllImport("UnityPlugin")]
    //public static extern void setHapticLayerDepth(int a_layerDepth, int a_layerID, double a_depth);

    [DllImport("UnityPlugin")]
	public static extern void setHapticLayerProperties(int a_layerDepth, int a_layerID, double a_stiffness, double a_stiffnessExponent, double a_maxFrictionForce, double a_penetrationThreshold, double a_resistanceToMovement, double a_depth);

    [DllImport("UnityPlugin")]
    public static extern void setPatientLayersEnabled(bool a_enabled);

    [DllImport("UnityPlugin")]
    public static extern void setPatientNumLayersToUse(int a_numLayers);

	[DllImport("UnityPlugin")]
	public static extern void resetAxialConstraintPID();

	[DllImport("UnityPlugin")]
	public static extern bool isPatientPenetrated();

	[DllImport("UnityPlugin")]
	public static extern int getLastLayerPenetratedID();

    [DllImport("UnityPlugin")]
    public static extern int getCurrentLayerDepth();

    [DllImport("UnityPlugin")]
	public static extern void setNeedlePID(double kp, double ki, double kd, double gain);

    [DllImport("UnityPlugin")]
    public static extern int getNumHapticPoints();

}