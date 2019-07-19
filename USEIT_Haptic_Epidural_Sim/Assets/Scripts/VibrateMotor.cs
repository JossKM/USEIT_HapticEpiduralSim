using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System.IO;

public class VibrateMotor : MonoBehaviour
{
    Vector3 deltaConstraintPosition = new Vector3(0, 0, 0);

    //access script
    public ArduinoConnection arduinoConnection;
    public HapticAxialConstraint constraint;

    public GameObject axialConstraintPos;

    [Tooltip("maps distance between constraint and needle to haptic output")]
    [SerializeField]
    Vector2 minMaxHapticDistance = new Vector2(0.005f, 0.01f);

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        //Update positions
        Vector3 needlePosition = gameObject.transform.position;
        Vector3 constraintPosition = axialConstraintPos.transform.position;

        deltaConstraintPosition = needlePosition - constraintPosition;

        var distance = deltaConstraintPosition.magnitude;
        var direction = deltaConstraintPosition / distance; //normalized direction


        //Check if needle is too far from the axial constraint and send bytes based on which direction
        CheckDeltaConstraint(deltaConstraintPosition, direction);
    }

    void OnCollisionEnter(Collision col)
    {
        if (col.gameObject.tag == "Bone")
        {
            //5 for top(DNE), 4 for left, 3 for bottom, 2 for right, 1 for all
            arduinoConnection.WriteByte(1);
            Debug.Log("Touched bone!");
        }
    }

    void CheckDeltaConstraint(Vector3 deltaPos, Vector3 direction)
    {
        bool constraintEnabled = constraint.GetEnabled();
        if (constraintEnabled)
        {
            if(deltaPos.sqrMagnitude > minMaxHapticDistance.x * minMaxHapticDistance.x)
            {
               // x axis
                if(deltaPos.x > 0)
                {
                    //too far right
                    arduinoConnection.WriteByte(2);
                    Debug.Log("Too right!");
                } else
                {
                    //too far left
                    arduinoConnection.WriteByte(4);
                    Debug.Log("Too left!");
                }

                // y axis
                if (deltaPos.y > 0)
                {
                    //too far up
                    arduinoConnection.WriteByte(3);
                    Debug.Log("Too up!");
                }
                else
                {
                    //too far down
                    arduinoConnection.WriteByte(3);
                    Debug.Log("Too down!");
                }
            }

        }


        //if (needlePos.x + 1 > deltaPos.x)
        //{
        //    //too far right
        //    arduinoConnection.WriteByte(2);
        //    Debug.Log("Too right!");
        //}
        //else if (needlePos.x - 1 < deltaPos.x)
        //{
        //    //too far left
        //    arduinoConnection.WriteByte(4);
        //    Debug.Log("Too left!");
        //}
        //if (needlePos.y + 1 > deltaPos.y)
        //{
        //    //too far up
        //    arduinoConnection.WriteByte(3);
        //    Debug.Log("Too up!");
        //}
        //else if (needlePos.y - 1 < deltaPos.y)
        //{
        //    //too far down
        //    arduinoConnection.WriteByte(3);
        //    Debug.Log("Too down!");
        //}
    }
}
