using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System.IO;

public class VibrateMotor : MonoBehaviour
{
    const byte COMMAND_NULL = 0;
    const byte COMMAND_LEFT = 1;
    const byte COMMAND_BOTTOM = 2;
    const byte COMMAND_RIGHT = 3;
    const byte COMMAND_TOP = 4;

    const byte MAX_VIBES = 255;
    const byte MED_VIBES = 100;

    Vector3 deltaConstraintPosition = new Vector3(0, 0, 0);

    //access script
    public ArduinoConnection arduinoConnection;
    public HapticAxialConstraint constraint;

    [SerializeField]
    string recvStr;

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

    void OnCollisionStay(Collision col)
    {
        //if (col.gameObject.tag == "Bone")
        {
            //5 for top(DNE), 4 for left, 3 for bottom, 2 for right, 1 for all
            //arduinoConnection.WriteByte(1);

            byte motorVal = 0;
            byte motorCMD = COMMAND_NULL;

            motorCMD = COMMAND_LEFT;
            motorVal = MAX_VIBES;
            arduinoConnection.GetSerialPort().Write(new byte[2] { motorCMD, motorVal }, 0, 2);

            motorCMD = COMMAND_RIGHT;
            motorVal = MAX_VIBES;
            arduinoConnection.GetSerialPort().Write(new byte[2] { motorCMD, motorVal }, 0, 2);

            motorCMD = COMMAND_TOP;
            motorVal = MAX_VIBES;
            arduinoConnection.GetSerialPort().Write(new byte[2] { motorCMD, motorVal }, 0, 2);

            motorCMD = COMMAND_BOTTOM;
            motorVal = MAX_VIBES;
            arduinoConnection.GetSerialPort().Write(new byte[2] { motorCMD, motorVal }, 0, 2);


            //Debug.Log("Touched bone!");
        }
    }

    void CheckDeltaConstraint(Vector3 deltaPos, Vector3 direction)
    {
        if(!arduinoConnection.isConnectedAndOpen())
        {
            Debug.Log("Port not open for Vibrotactile feedback!");
            return;
        }

        recvStr = arduinoConnection.ReadArduino();

        bool constraintEnabled = constraint.GetEnabled();
        if (constraintEnabled)
        {
            if(deltaPos.sqrMagnitude > minMaxHapticDistance.x * minMaxHapticDistance.x)
            {
                byte motorVal = 0;
                byte motorCMD = COMMAND_NULL;

               // x axis
                if(deltaPos.x > 0)
                {
                    //too far right
                    motorCMD = COMMAND_RIGHT;
                    motorVal = MED_VIBES;
                    arduinoConnection.GetSerialPort().Write(new byte[2] { motorCMD, motorVal }, 0, 2);

                    motorCMD = COMMAND_LEFT;
                    motorVal = 0;
                    arduinoConnection.GetSerialPort().Write(new byte[2] { motorCMD, motorVal }, 0, 2);

                    //Debug.Log("Too right!");
                } else
                {
                    //too far left
                    motorCMD = COMMAND_LEFT;
                    motorVal = MED_VIBES;
                    arduinoConnection.GetSerialPort().Write(new byte[2] { motorCMD, motorVal }, 0, 2);

                    motorCMD = COMMAND_RIGHT;
                    motorVal = 0;
                    arduinoConnection.GetSerialPort().Write(new byte[2] { motorCMD, motorVal }, 0, 2);

                    //Debug.Log("Too left!");
                }

                // y axis
                if (deltaPos.y > 0)
                {
                    //too far up
                    motorCMD = COMMAND_TOP;
                    motorVal = MED_VIBES;
                    arduinoConnection.GetSerialPort().Write(new byte[2] { motorCMD, motorVal }, 0, 2);

                    motorCMD = COMMAND_BOTTOM;
                    motorVal = 0;
                    arduinoConnection.GetSerialPort().Write(new byte[2] { motorCMD, motorVal }, 0, 2);

                    //Debug.Log("Too up!");
                }
                else
                {
                    //too far down
                    motorCMD = COMMAND_BOTTOM;
                    motorVal = MED_VIBES;
                    arduinoConnection.GetSerialPort().Write(new byte[2] { motorCMD, motorVal }, 0, 2);

                    motorCMD = COMMAND_TOP;
                    motorVal = 0;
                    arduinoConnection.GetSerialPort().Write(new byte[2] { motorCMD, motorVal }, 0, 2);

                    //Debug.Log("Too down!");
                }


            }

        }
    }
}
