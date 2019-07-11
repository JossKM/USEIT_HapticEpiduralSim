using UnityEngine;
using System.Collections;
using UnityEngine.Assertions;

[RequireComponent(typeof(HapticObject))]
public class HapticMembraneEffect : MonoBehaviour
{
    public double resistance = 9.0;
    public double friction_static = 0.2;
    public double friction_dynamic = 0.5;
    public double maxForce = 9.0;
    public double springMass = 1.0;
    public double distanceToMaxForce = 0.1;
    public double penetrationThreshold = 4.0;


    // Use this for initialization
    void Start ()
    {
        //Assert.AreNotEqual(springMass, 0.0);
        //

        var a = GetComponent<TouchableMeshObject>();

        int objectId = GetComponent<TouchableMeshObject>().objectId;

        HapticNativePlugin.addViscosityEffect(objectId, GetComponent<HapticObject>().viscosity);
        HapticNativePlugin.addMembraneEffect(objectId, resistance, friction_static, friction_dynamic, maxForce, distanceToMaxForce, springMass, penetrationThreshold);
    }
	
	// Update is called once per frame
	void Update ()
	{
    }
}
