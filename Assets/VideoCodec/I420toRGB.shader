// Upgrade NOTE: replaced 'mul(UNITY_MATRIX_MVP,*)' with 'UnityObjectToClipPos(*)'

Shader "CustomVideoShader/I420toRGB" {
	Properties
	{
		_MainTex("_MainTexture", 2D) = "white" {}
		_UTex("U_Texture", 2D) = "white" {}
		_VTex("V_Texture", 2D) = "white" {}
	}

	SubShader
	{	
	    Tags {"RenderType" = "Opaque"}
		Pass
	    {
			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag

			#include "UnityCG.cginc"

			struct appdata
			{
				float4 vertex : POSITION;
				float2 y :   TEXCOORD0;
				float2 u : TEXCOORD1;
				float2 v : TEXCOORD2;
			};

			struct v2f
			{
				float2 y : TEXCOORD0;
				float2 u : TEXCOORD1;
				float2 v : TEXCOORD2;
				float4 vertex : SV_POSITION;
			};

			sampler2D _MainTex;
			float4 _MainTex_ST;

			sampler2D _UTex;
			float4 _UTex_ST;

			sampler2D _VTex;
			float4 _VTex_ST;

			// matrix
			float4x4 _YUVMat;

			uniform float available_scale_y;
			uniform float available_scale_uv;

			v2f vert(appdata v)
			{
				v2f o;
				o.vertex = UnityObjectToClipPos(v.vertex);
				o.y = TRANSFORM_TEX(v.y, _MainTex);
				o.u = TRANSFORM_TEX(v.u, _UTex);
				o.v = TRANSFORM_TEX(v.v, _VTex);
				return o;
			}

			fixed4 frag(v2f i) : SV_Target
			{
				float y,u,v;
				y = tex2D(_MainTex, float2(i.y.x * available_scale_y, 1 - i.y.y)).r;
				u = tex2D(_UTex, float2(i.u.x * available_scale_uv, 1 - i.u.y)).r;
				v = tex2D(_VTex, float2(i.v.x * available_scale_uv, 1 - i.v.y)).r;
				fixed4 yuv = fixed4(y, u, v, 1.0) - fixed4(0.0627, 0.5, 0.5, 0);
				fixed4 rgba = mul(yuv, _YUVMat);
				return fixed4(rgba.rgb, 1.0);
			}
		    ENDCG
		}
	}
}
