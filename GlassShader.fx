uniform extern float4x4  gWVP;
uniform extern float4x4  gWorldInv;
uniform extern float3    gEyePosW;
uniform extern texture   gEnvMap;
uniform extern float	 gRIndex;

//uniform extern float4x4  gWorld;
//uniform extern float4x4  gViewProj;
//uniform extern float3	 gLightPos;

//uniform extern Mtrl      gMtrl;
//uniform extern SpotLight gLight;

uniform extern texture   gTex;
//uniform extern texture   gShadowMap;
//uniform extern float4x4 gLightWVP;
//uniform extern bool gImposter;

sampler TexS = sampler_state
{
	Texture = <gTex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP; 
    AddressV  = WRAP;
};

samplerCUBE EnvMapS = sampler_state
{
    Texture   = <gEnvMap>;
	//MinFilter = LINEAR; 
    //MagFilter = LINEAR;
    //MipFilter = LINEAR;
    //AddressU  = WRAP;
    //AddressV  = WRAP;
};


struct VertOut
{
	float4 posH		: POSITION0;
	float2 tex0		: TEXCOORD0;
	float3 lPos		: TEXCOORD1;		//Light
	float3 nPos		: TEXCOORD2;		//Normal
	float3 vPos		: TEXCOORD3;		//View

};

 
VertOut RefractVS(float4 posL: POSITION, float2 tex0: TEXCOORD, float3 normalL: NORMAL)
{
	VertOut Out = (VertOut)0;
	Out.posH = mul(posL, gWVP);

	Out.nPos = normalize(mul(gWorldInv, normalL));
	Out.vPos = gEyePosW - posL;

	return Out;
}

float4 RefractPS(float2 tex0: TEXCOORD0, float3 lPos: TEXCOORD1, float3 nPos: TEXCOORD2, float3 vPos: TEXCOORD3) : COLOR
{
	float3 ViewDir = normalize(vPos); 
 
    float3 Refract = refract(ViewDir, nPos, gRIndex);
	//Refract = mul(Refract, 0.25);
	//Refract = float3(1.0,1.0,1.0) - Refract;
    float3 RefractColor = texCUBE(EnvMapS, Refract);
 
    // return the color
    return float4(RefractColor,1);
}


technique RefractShader
{
	pass P0
	{
		VertexShader = compile vs_2_0 RefractVS();
		PixelShader = compile ps_2_0 RefractPS();
	}
}