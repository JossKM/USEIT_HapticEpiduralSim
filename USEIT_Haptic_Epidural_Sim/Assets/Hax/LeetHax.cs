//
//
// Handles debug Keyboard commands etc.

// comment/uncomment this define to disable/enable 1337 h@x0rz
#define LEETHAX

using UnityEngine;
using UnityEngine.Events;
using System.Collections;

public class LeetHax : MonoBehaviour
{
    private const int NUM_HAX = 6;

    [SerializeField]
    string[] hackNames = new string[NUM_HAX];

    public UnityEvent OnHack1;
    public UnityEvent OnHack2;
    public UnityEvent OnHack3;
    public UnityEvent OnHack4;
    public UnityEvent OnHack5;
    public UnityEvent OnHack6;
    
    public bool haxEnabled = true;

    [Tooltip("To show when hax are enabled")]
    [SerializeField]
    Animator haxCanvas;

    //Color originalColor = Color.black;

#if LEETHAX
    // Start is called before the first frame update
    void Start()
    {
        haxCanvas.gameObject.SetActive(haxEnabled);
        //Color oldCol = haxCanvas.GetColor();
    }

    void DoAnim()
    {
        haxCanvas.SetTrigger("Hax");
        //StartCoroutine(FlashAnim());
    }

    //IEnumerator FlashAnim()
    //{
    //    for(int i = 0; i < ; i++)
    //    {
    //        haxCanvas.SetColor(new Color(Random.value, Random.value, Random.value));
    //        yield return new WaitForEndOfFrame(0.02f);
    //    }
    //
    //    haxCanvas.SetColor(originalColor);
    //}

    // Update is called once per frame
    void Update()
    {
        if (Input.GetButtonDown("EnableHacks"))
        {
            haxEnabled = !haxEnabled;
            haxCanvas.gameObject.SetActive(haxEnabled);
        }

        if(haxEnabled)
        {
            if (Input.GetButtonDown("Hack1"))
            {
                OnHack1.Invoke();
                DoAnim();
            }
            if (Input.GetButtonDown("Hack2"))
            {
                OnHack2.Invoke();
                DoAnim();
            }
            if (Input.GetButtonDown("Hack3"))
            {
                OnHack3.Invoke();
                DoAnim();
            }
            if (Input.GetButtonDown("Hack4"))
            {
                OnHack4.Invoke();
                DoAnim();
            }
            if (Input.GetButtonDown("Hack5"))
            {
                OnHack5.Invoke();
                DoAnim();
            }
            if (Input.GetButtonDown("Hack6"))
            {
                OnHack6.Invoke();
                DoAnim();
            }
        }
    }

#endif
}
