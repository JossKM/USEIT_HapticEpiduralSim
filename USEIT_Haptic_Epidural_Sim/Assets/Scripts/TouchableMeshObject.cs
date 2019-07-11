using UnityEngine;
using System.Collections;


public abstract class HapticObject : MonoBehaviour
{
    public int objectId = -1;

    public double friction_static = 0.8;
    public double friction_dynamic = 0.6;
    public double stiffness = 0.7;
    public double viscosity = 0.0;
    public double penetrationForce = 10000.0;
}

public class TouchableMeshObject : HapticObject
{
    void Start()
    {

        var devicePosition = GameObject.Find("Haptic Origin");

        Mesh mesh = this.GetComponent<MeshFilter>().mesh;
        Vector3[] vertices = mesh.vertices;
        Vector3[] normals = mesh.normals;
        int[,] triangles = new int[(mesh.triangles.Length / 3), 3];

        for (int i = 0; i < mesh.triangles.Length / 3; i++)
        {
            triangles[i, 0] = mesh.triangles[3 * i];
            triangles[i, 1] = mesh.triangles[3 * i + 1];
            triangles[i, 2] = mesh.triangles[3 * i + 2];
        }

        Vector3 totalScale = this.transform.localScale;

        // get global scale by looping through parenting system
        Transform nextParent = transform.parent;
        while (nextParent != null)
        {
            totalScale.Scale(nextParent.localScale);
            nextParent = nextParent.parent;
        }

        objectId = HapticNativePlugin.AddObject(this.transform.position - devicePosition.transform.position,
            totalScale,
            this.transform.rotation.eulerAngles,
            vertices,
            normals,
            mesh.vertices.Length,
            triangles,
            mesh.triangles.Length / 3);
    }

    void Update()
    {
        HapticNativePlugin.setObjectProperties(objectId, stiffness, friction_static, friction_dynamic, viscosity, penetrationForce);
    }
}
