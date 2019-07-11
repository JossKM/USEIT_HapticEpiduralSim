using UnityEngine;
using System.Collections;

public class PenetrableMaterial : MonoBehaviour {

    // before penetration
	public double 	m_penetrationThreshold = 8.0;
    public double   m_stiffness = 6000.0;
    public double   m_stiffnessExponent = 1.5;

    // once penetrated
	public double 	m_maxFrictionForce = 0.1;
	public double 	m_malleability = 1.0;
	public double   m_fluidResistance = 1.0;

	// face name
	public string m_name;

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

    void Start ()
    {
    }
	
	void Update () {
	
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
