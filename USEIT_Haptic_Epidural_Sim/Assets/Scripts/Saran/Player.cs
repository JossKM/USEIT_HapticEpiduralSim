using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : MonoBehaviour
{
    bool jump;

    Rigidbody rb;

    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody>();
    }

    // Update is called once per frame
    void Update()
    {
        if (jump)
        {
            rb.velocity = Vector3.up * 10f;
        }
        if (rb.velocity.y < 0)
        {
            rb.velocity += Vector3.up * Physics.gravity.y * 2.5f * Time.deltaTime;
        }
        else if (rb.velocity.y > 0 && !jump)
        {
            rb.velocity += Vector3.up * Physics.gravity.y * 2f * Time.deltaTime;
        }

    }

    public void JumpKeyDown()
    {
        jump = true;
    }

    public void JumpKeyUp()
    {
        jump = false;
    }
}
