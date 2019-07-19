using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class VRPNDebug : MonoBehaviour
{
    public Player p;

    public void MouseClicked()
    {
        Debug.Log("You have clicked the left mouse button!");
    }

    public void KeyboardDown()
    {
        Debug.Log("You have clicked the ctrl key!");
        p.JumpKeyDown();
    }

    public void KeyboardUp()
    {
        Debug.Log("You have clicked the ctrl key!");
        p.JumpKeyUp();
    }
}
