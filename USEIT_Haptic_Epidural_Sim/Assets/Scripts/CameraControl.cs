using UnityEngine;
using System.Collections;

public class CameraControl : MonoBehaviour
{
    public bool moveWorkspace = true;

    public float moveSpeed = 1.0f;
    public float rotationSpeed = 1.0f;
    public Transform workspace;

	private float rotationX = 0f;
	private float rotationY = 0f;
    
    private Transform deviceOrigin;

    public Vector3 workspaceOffset; // offset of the workspace from the camera

    private void Start()
    {
        deviceOrigin = GameObject.Find("Haptic Origin").transform;
        workspaceOffset = workspace.position - transform.position;
    }

	// Update is called once per frame
	void Update () {

        if(Input.GetKeyUp(KeyCode.M))
        {
            moveWorkspace = !moveWorkspace;
        }

		float scroll = Input.GetAxis("Mouse ScrollWheel");
		transform.Translate(0, 0f, scroll * 0.1f * moveSpeed, Space.Self);

		if (Input.GetMouseButton (1)) {
			rotationX += Input.GetAxis ("Mouse X") * rotationSpeed * 500f * Time.deltaTime;
			rotationY += Input.GetAxis ("Mouse Y") * rotationSpeed * 500f * Time.deltaTime;
			transform.localEulerAngles = new Vector3 (-rotationY, rotationX, 0);
		}
		else if (Input.GetMouseButton (2)) {
			var xMove = Input.GetAxis ("Mouse X") * -rotationSpeed * Time.deltaTime;
			var yMove = Input.GetAxis ("Mouse Y") * -rotationSpeed * Time.deltaTime;
			transform.Translate(xMove, yMove, 0f, Space.Self);
		}

		if (Input.GetKey(KeyCode.UpArrow))
			transform.Translate(0f, 0f, moveSpeed * Time.deltaTime, Space.World);
		if (Input.GetKey(KeyCode.DownArrow))
			transform.Translate(0f, 0f, -moveSpeed * Time.deltaTime, Space.World);
		if (Input.GetKey(KeyCode.LeftArrow))
			transform.Translate(-moveSpeed * Time.deltaTime, 0f, 0f, Space.World);
		if (Input.GetKey(KeyCode.RightArrow))
			transform.Translate(moveSpeed * Time.deltaTime, 0f, 0f, Space.World);
	    if (Input.GetKey(KeyCode.Space))
	        transform.Translate(0f, moveSpeed * Time.deltaTime, 0f, Space.World);
	    if (Input.GetKey(KeyCode.LeftControl))
	        transform.Translate(0f, -moveSpeed * Time.deltaTime, 0f, Space.World);

	    if (moveWorkspace)
	    {
	        workspace.position = transform.position + workspaceOffset;
            UpdateHapticRotation();
        }
        
        UpdateHapticPosition();
    }

    private void UpdateHapticPosition()
    {
        HapticNativePlugin.SetHapticPosition((workspace.position - deviceOrigin.position));
    }

    private void UpdateHapticRotation()
    {
        HapticNativePlugin.SetHapticRotation(this.transform.rotation.eulerAngles);
    }

}
