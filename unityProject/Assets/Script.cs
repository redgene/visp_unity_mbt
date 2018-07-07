using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.UI;

public class Script : MonoBehaviour
{
    // FUNCTIONS IMPORTED FROM DLL:

    [DllImport("ViSPUnity", CallingConvention = CallingConvention.Cdecl, EntryPoint = "createCaoFile")]
    public static extern void createCaoFile(double cubeEdgeSize);

    [DllImport("ViSPUnity", CallingConvention = CallingConvention.Cdecl, EntryPoint = "InitMBT")]
    public static extern void InitMBT(double cam_px, double cam_py, double cam_u0, double cam_v0, int t);

    [DllImport("ViSPUnity", CallingConvention = CallingConvention.Cdecl, EntryPoint = "AprilTagMBT")]
    public static extern void AprilTagMBT(byte[] bitmap, int height, int width, double[] pointx, double[] pointy, double[] kltX, double[] kltY, int[] kltNumber, int t, int e);
    
    WebCamTexture wct;
    Renderer rend;
    Texture2D tex;

    [Header("Enter your Camera Resolution here")]
    public int X = 640;   // for example 640
    public int Y = 480;   // for example 480

    [Header("Enter your Camera Parameters here")]
    public double cam_px = 1131.561907;   
    public double cam_py = 1085.157822;
    public double cam_u0 = 588.2376812;
    public double cam_v0 = 191.1328903;

    double[] pointx = new double[24];
    double[] pointy = new double[24];
    double[] kltX = new double[300];
    double[] kltY = new double[300];

    int[] kltNumber = new int[1];

    public enum tracking{
        Edge_Tracking,
        Edge_Tracking_with_KLT
    };
    public enum edges
    {
        Visible_Edge_tracking_only,
        All_Edges_tracking
    };

    [Header("Tracking Method Selection")]
    public Script.tracking trackingMethod = tracking.Edge_Tracking;          // selected by default
    public Script.edges edgesVisibility = edges.Visible_Edge_tracking_only; // selected by default

    int tr = 0;
    int e = 0;

    void Start()
    {
        wct = new WebCamTexture();
        //wct = new WebCamTexture(WebCamTexture.devices[0].name, 640, 480, 30);
        rend = GetComponent<Renderer>();
        rend.material.mainTexture = wct;

        wct.Play(); //Start capturing image using webcam  
        createCaoFile(0.125);

        if (trackingMethod == tracking.Edge_Tracking_with_KLT)
            tr = 1;
        else
            tr = 0;

        if (edgesVisibility == edges.All_Edges_tracking)
            e = 1;
        else
            e = 0;

        //Change camera parameters in the inspector window
        InitMBT(1131.561907, 1085.157822, 588.2376812, 191.1328903, tr);
        //Debug.Log(Application.persistentDataPath);
    }

    double x1,x2;
    double y1,y2;

    void Update()
    {
        AprilTagMBT(Color32ArrayToByteArray(wct.GetPixels32()), wct.height, wct.width, pointx, pointy, kltX, kltY, kltNumber,tr, e);
        GameObject[] line = GameObject.FindGameObjectsWithTag("Line");

        //Loop for all 8 points (each repeated 3 times) and draw lines:
        for (int i = 0; i < 24; i += 2)
        {
            //Scaling according to plane
            x1 =-( (float)13.33 / X * pointx[i]*2 - 13.33/2);
            x2 =-( (float)13.33 / X * pointx[i+1]*2 - 13.33 / 2);
            y1 =-((float)10 / Y * pointy[i]*2 - 10 / 2);
            y2 =-( (float)10 / Y * pointy[i+1]*2 - 10 / 2);

            //Draw lines
            line[i/2].GetComponent<LineRenderer>().SetPosition(0, new Vector3((float)x1, (float)y1, -9f));
            line[i/2].GetComponent<LineRenderer>().SetPosition(1, new Vector3((float)x2, (float)y2, -9f));
            
            //Following line is for debugging purpose only: shows lines only in the 'Scene' view, not in 'Game' view:    
            Debug.DrawLine(new Vector3((float)x1, (float)y1, -19f), new Vector3((float)x2, (float)y2, -19f));
        }

        // Testing KLT in Unity:
        if(tr==1)
            Debug.Log("Num of KLT Feature Points:" + kltNumber[0]);
        
        if(tr==0)
            Debug.Log("Edge Tracking");

        //for (int i = 0; i < kltNumber[0]; i++) {
        //    GameObject s = GameObject.CreatePrimitive(PrimitiveType.Sphere);
        //    s.tag = "kltPoint";
        //    s.transform.localScale = new Vector3(1, 1, 1);
        //    x1 = -((float)13.33 / X * kltX[i] * 2 - 13.33 / 2);
        //    y1 = -((float)10 / Y * kltY[i] * 2 - 10 / 2);
        //    s.transform.localPosition = new Vector3((float)x1, (float) x2, -20f);
        //}
        //GameObject[] dest = GameObject.FindGameObjectsWithTag("kltPoint");
    }

    //Function for converting into Byte Array to be sent to functions in DLL
    private static byte[] Color32ArrayToByteArray(Color32[] colors)
    {
        if (colors == null || colors.Length == 0)
            return null;

        int length = colors.Length;
        byte[] bytes = new byte[length];
        int value = 0;

        for (int i = 0; i < colors.Length; i++)
        {
            value = (colors[i].r + colors[i].g + colors[i].b) / 3;
            bytes[colors.Length - i - 1] = (byte)value;
        }

        return bytes;
    }
}
