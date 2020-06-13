using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Sprites;

public class Decal : MonoBehaviour
{
    static float timeStep = 0.1f;

    [SerializeField]
    SpriteRenderer sprite;

    /// <summary>
    /// Represents which type of decal it is, not unique.
    /// </summary>
    public int prefabID { get; private set; }

    /// <summary>
    /// PLZ dont use this unless you know what you are doing
    /// </summary>
    public void SetID(int idNum)
    {
        prefabID = idNum;
    }

    [Tooltip("Time to live")]
    [SerializeField]
    float lifetime = float.PositiveInfinity;

    [Tooltip("Time to alpha fade out after lifetime")]
    [SerializeField]
    float deathTime = 1.0f;

    public void Start()
    {
        Respawn();
    }

    public void Respawn()
    {
        if (lifetime < float.PositiveInfinity)
        {
            Invoke("Fade", lifetime);
        }

        sprite.enabled = true;
        // restart animation??
    }

    private void Fade()
    {
        StartCoroutine(FadeOut());
    }

    private IEnumerator FadeOut()
    {
        Color color = sprite.color;
        float origAlpha = color.a;

        for (float timeLeft = deathTime; timeLeft > 0.0f; timeLeft -= timeStep)
        {
            float newAlpha = Mathf.Lerp(origAlpha, 0.0f, Mathf.InverseLerp(deathTime, 0.0f, timeLeft));
            color.a = newAlpha;
            sprite.color = color;

            yield return new WaitForSeconds(timeStep);
        }

        sprite.enabled = false;
    }
}