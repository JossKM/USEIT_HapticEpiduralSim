using UnityEngine;
using System.Collections;

public class TouchableBoxObject : HapticObject
{

    // Use this for initialization
    void Start()
    {
        if (!HapticManager.isHapticAvail)
        {
            gameObject.SetActive(false);
            return;
        }

        Vector3 totalScale = this.transform.localScale;

        // get global scale by looping through parenting system
        Transform nextParent = transform.parent;
        while (nextParent != null)
        {
            totalScale.Scale(nextParent.localScale);
            nextParent = nextParent.parent;
        }

        objectId = HapticNativePlugin.AddBoxObject(
            this.transform.position - HapticManager.hapticOrigin.position,
            totalScale,
            this.transform.rotation.eulerAngles
            );
    }

    void Update()
    {
        HapticNativePlugin.setObjectProperties(objectId, stiffness, friction_static, friction_dynamic, viscosity, penetrationForce);
    }
}
