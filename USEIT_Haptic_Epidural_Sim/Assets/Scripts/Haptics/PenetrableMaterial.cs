using UnityEngine;
using System.Collections;

// holds properties of layers

public class PenetrableMaterial : MonoBehaviour {

    //should rotation be locked upon penetrating this layer?
    [SerializeField]
    public bool m_locksRotation = true;

    // before penetration
    [SerializeField]
    public double m_penetrationThreshold = 8.0;
    [SerializeField]
    public double   m_stiffness = 6000.0;
    [SerializeField]
    public double   m_stiffnessExponent = 1.5;

    // once penetrated
    [SerializeField]
    public double 	m_maxFrictionForce = 0.1;
    [SerializeField]
    public double 	m_malleability = 1.0;
    [SerializeField]
    public double   m_fluidResistance = 1.0;

    // face name
    [SerializeField]
    public string m_name = "penetrablematerialname";

    // unique identifier for each layer
    private int m_uniqueID = -1;

    // goes up every time one is created. never goes down, so no two materials can ever have the same ID
    private static int s_IDCounter = 0;
	private static ArrayList s_layers = new ArrayList();

    void Awake()
    {
        m_uniqueID = s_IDCounter;
		s_layers.Add(this);
        s_IDCounter++;
    }
    
	public int GetID()
	{
		return m_uniqueID;
	}

	public static string GetNameOfLayer(int id)
	{
		if (id < 0) {
			return "none";
		} else 
			return ((PenetrableMaterial)(s_layers [id])).m_name;
	}

	public static PenetrableMaterial GetLayer(int id)
	{
		if (id < 0) {
			return null;
		} else 
			return (PenetrableMaterial)(s_layers [id]);
	}
}
