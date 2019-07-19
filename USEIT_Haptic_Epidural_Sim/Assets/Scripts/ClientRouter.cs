using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.Networking;
using UnityEngine.Networking.NetworkSystem;

public class ClientRouter : NetworkLobbyManager
{
    //public override void OnClientConnect(NetworkConnection conn)
    //{
    //    SceneManager.LoadScene("HostScene");
    //    ClientScene.Ready(conn);
    //    ClientScene.AddPlayer(conn, 0);
    //}
    //
    //public override void OnClientSceneChanged(NetworkConnection conn)
    //{
    //    SceneManager.LoadScene("Matthew'sScene");
    //    ClientScene.Ready(conn);
    //    ClientScene.AddPlayer(conn, 0);
    //}
    //
    //public override void OnServerSceneChanged(string sceneName)
    //{
    //    ServerChangeScene("HostScene");
    //    //SceneManager.LoadScene("HostScene");
    //}

    public override void OnLobbyClientConnect(NetworkConnection conn)
    {
        SceneManager.LoadScene("Matthew'sScene");
        ClientScene.Ready(conn);
        ClientScene.AddPlayer(conn, 0);
    }

    public override void OnLobbyServerConnect(NetworkConnection conn)
    {
        SceneManager.LoadScene("HostScene");
        ClientScene.Ready(conn);
        ClientScene.AddPlayer(conn, 0);
    }

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
