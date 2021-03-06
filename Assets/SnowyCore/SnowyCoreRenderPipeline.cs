using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering;
using UnityEditor;

public class SnowyCoreRenderPipeline : RenderPipeline
{
    RenderTexture m_GDepth;                                                 //depth attachment
    RenderTexture[] m_GBuffers = new RenderTexture[4];                      //color attachment
    RenderTargetIdentifier[] m_GBufferID = new RenderTargetIdentifier[4];   //tex id

    // IBL Map
    public Cubemap DiffuseIBL { get; set; }
    public Cubemap SpecularIBL { get; set; }
    public Texture BRDFLut { get; set; }
    
    public SnowyCoreRenderPipeline()
    {
        // create texture
        m_GDepth = new RenderTexture(Screen.width, Screen.height, 24, RenderTextureFormat.Depth, RenderTextureReadWrite.Linear);
        m_GBuffers[0] = new RenderTexture(Screen.width, Screen.height, 0, RenderTextureFormat.ARGB32, RenderTextureReadWrite.Linear);
        m_GBuffers[1] = new RenderTexture(Screen.width, Screen.height, 0, RenderTextureFormat.ARGB2101010, RenderTextureReadWrite.Linear);
        m_GBuffers[2] = new RenderTexture(Screen.width, Screen.height, 0, RenderTextureFormat.ARGB64, RenderTextureReadWrite.Linear);
        m_GBuffers[3] = new RenderTexture(Screen.width, Screen.height, 0, RenderTextureFormat.ARGBFloat, RenderTextureReadWrite.Linear);
        // bind tex id
        for(int i = 0; i < 4; i++)
        {
            m_GBufferID[i] = m_GBuffers[i];
        }
    }
    protected override void Render(ScriptableRenderContext context, Camera[] cameras)
    {
        // main camera
        var camera = cameras[0];
        context.SetupCameraProperties(camera);
        
        // GBuffer
        Shader.SetGlobalTexture("_gdepth", m_GDepth);
        for(var i = 0; i < 4; i++)
        {
            Shader.SetGlobalTexture("_GT" + i, m_GBuffers[i]);
        }

        // IBL贴图
        Shader.SetGlobalTexture("_diffuseIBL", DiffuseIBL);
        Shader.SetGlobalTexture("_specularIBL", SpecularIBL);
        Shader.SetGlobalTexture("_brdfLut", BRDFLut);

        // set matrix
        Matrix4x4 viewMat = camera.worldToCameraMatrix;
        Matrix4x4 projectionMat = GL.GetGPUProjectionMatrix(camera.projectionMatrix, false);
        Matrix4x4 vpMat = viewMat * projectionMat;
        Matrix4x4 vpMatInv = vpMat.inverse;
        Shader.SetGlobalMatrix("_vpMat", vpMat);
        Shader.SetGlobalMatrix("_vpMatInv", vpMatInv);

        // ------------------------ Pass Start ------------------------ //
        GBufferPass(context, camera);
        LightPass(context, camera);
        // ------------------------ Pass End   ------------------------ //

        // skybox and Gizmos
        context.DrawSkybox(camera);
        if (Handles.ShouldRenderGizmos())
        {
            context.DrawGizmos(camera, GizmoSubset.PreImageEffects);
            context.DrawGizmos(camera, GizmoSubset.PostImageEffects);
        }

        context.Submit();
    }
    void GBufferPass(ScriptableRenderContext context, Camera camera)
    {
        var cmd = new CommandBuffer
        {
            name = "gbuffer"
        };

        cmd.SetRenderTarget(m_GBufferID, m_GDepth);
        // clear
        cmd.ClearRenderTarget(true, true, Color.clear);
        context.ExecuteCommandBuffer(cmd);
        cmd.Clear();

        // culling
        camera.TryGetCullingParameters(out var cullingParameters);
        var cullingResults = context.Cull(ref cullingParameters);

        // config setting
        var shaderTagId = new ShaderTagId("gbuffer");   //使用LightMode为gbuffer的Shader
        var sortingSettings = new SortingSettings(camera);
        var drawingSettings = new DrawingSettings(shaderTagId, sortingSettings);
        var filteringSettings = FilteringSettings.defaultValue;

        // render
        context.DrawRenderers(cullingResults, ref drawingSettings, ref filteringSettings);
        context.Submit();
    }
    void LightPass(ScriptableRenderContext context, Camera camera)
    {
        var cmd = new CommandBuffer
        {
            name = "lightPass"
        };
        var mat = new Material(Shader.Find("SnowyCore/lightpass"));
        cmd.Blit(m_GBufferID[0], BuiltinRenderTextureType.CameraTarget, mat);
        context.ExecuteCommandBuffer(cmd);
        context.Submit();
    }
}
