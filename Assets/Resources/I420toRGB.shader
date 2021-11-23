Shader "Custom/I420RGB"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _UTex ("U", 2D) = "white" {}
        _VTex ("V", 2D) = "white" {}
    }
    SubShader
    {
        Tags
        {
            "RenderType"="Opaque"
        }
        LOD 100

        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag

            #include "UnityCG.cginc"

            struct appdata
            {
                float4 vertex : POSITION;
                float2 uv : TEXCOORD0;
            };

            struct v2f
            {
                float2 uv : TEXCOORD0;
                float4 vertex : SV_POSITION;
            };

            sampler2D _MainTex;
            sampler2D _UTex;
            sampler2D _VTex;
            float4 _MainTex_ST;

            float4x4 _YUVMat;

            v2f vert(appdata v)
            {
                v2f o;
                o.vertex = UnityObjectToClipPos(v.vertex);
                o.uv = TRANSFORM_TEX(v.uv, _MainTex);
                o.uv.y = 1.0 - o.uv.y;
                return o;
            }

            fixed4 frag(v2f i) : SV_Target
            {
                fixed3 col = tex2D(_MainTex, i.uv);
                // YUV data saved in Texture r channel
                // return fixed4(col.rrr, 1.0);
                fixed y = tex2D(_MainTex, i.uv).r;
                fixed u = tex2D(_UTex, i.uv).r;
                fixed v = tex2D(_VTex, i.uv).r;

                fixed4 yuvc = fixed4(y, u, v, 1.0);
                yuvc = mul(_YUVMat, yuvc);
                yuvc = yuvc + fixed4(-0.7011, 0.53005, -0.8855, 0);
                return yuvc;
            }
            ENDCG
        }
    }
}