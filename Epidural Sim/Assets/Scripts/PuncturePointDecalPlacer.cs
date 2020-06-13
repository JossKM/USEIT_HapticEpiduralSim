using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PuncturePointDecalPlacer : MonoBehaviour
{
    [SerializeField]
    NeedlePunctureBehaviour needle;

    [SerializeField]
    UnityEngine.GameObject needlePenetrationPointPrefab;

    private void Start()
    {
        DecalSystem.instance.RegisterDecalPrefab(needlePenetrationPointPrefab);
    }

    //must be registered as a callback to something
    public void OnNewLayerPenetrated()
    {
        RaycastHit hit = needle.GetLatestPuncturePoint();
        // generate penetration point visualizer
        DecalSystem.instance.PlaceDecal(needlePenetrationPointPrefab, hit.point, hit.normal);
    }
}
