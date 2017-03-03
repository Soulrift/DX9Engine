uniform extern float4x4  gWorld;
uniform extern float4x4  gViewProj;
uniform extern float3	 gLightPos;
uniform extern float4x4  gWVP;
//uniform extern Mtrl      gMtrl;
//uniform extern SpotLight gLight;
uniform extern float3    gEyePosW;
uniform extern texture   gTex;
uniform extern texture   gShadowMap;
uniform extern float4x4 gLightWVP;
uniform extern bool gImposter;

static const float SHADOW_EPSILON = 0.00005f;
static const float SMAP_SIZE = 512.0f;

struct VertexOut
{
    float4 posH		: POSITION0;
    float4 colH	    : COLOR0;
};

VertexOut DynamicShadowVS(float4 Pos : POSITION0)
{
	VertexOut Vert = (VertexOut)0;

	//Place Vertex Position in World Coordinates
    float3 Pi = float4(mul(Pos, gWorld)).xyz;// * float4(aVertexPosition, 1.0) ).xyz;
    
    //Get Ray cast from light to vert
    float3 V  = normalize( Pi - gLightPos);
    
    //Terrain Normal
    float3 N  = float3(0.0, 1.0, 0.0);
    
    //Point on terrain
    float3 Po = float3(0.0, 0.0, 0.0);
    
    //Calculate Scale of Light ray    
    float t = dot(N,(Po - Pi)) / dot(N,V);
    
    //Calculate new point for vert on terrain
    float3 P = Pi + t*V;

    //Calculates Final Position of Vert
	Vert.posH = mul(float4(P, 1.0), gViewProj);
    //oPosH =  gProj * tempPos;
	return Vert;

	//oProjTex = mul(float4(posL, 1.0f), gLightWVP);
}

float4 DynamicShadowPS() : COLOR
{
	return float4(0.0, 0.0, 0.0, 1.0);
}

technique BuildShadows
{
	pass P0
	{
		vertexShader = compile vs_2_0 DynamicShadowVS();
        pixelShader  = compile ps_2_0 DynamicShadowPS();
	}
}



 
sampler TexS = sampler_state
{
	Texture = <gTex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP; 
    AddressV  = WRAP;
};

void LightShadowVS(float3 posL         : POSITION0,
                   float3 normalL      : NORMAL0,
                   float2 tex0         : TEXCOORD0,
                   out float4 oPosH    : POSITION0,
                   out float3 oPosW    : TEXCOORD0,
                   out float3 oNormalW : TEXCOORD1,
                   out float3 oToEyeW  : TEXCOORD2,
                   out float2 oTex0    : TEXCOORD3,
                   out float4 oProjTex : TEXCOORD4)
{
	//Transform to homogeneous screen space
	oPosH = mul(float4(posL, 1.0f), gWVP);
	
	//Transform to world space
	oPosW = mul(float4(posL, 1.0f), gWorld).xyz;
	
	//Transform the normal to world space
	oNormalW = mul(float4(normalL, 0.0f), gWorld).xyz;
	
	//Calculate the eye vector
	oToEyeW = gEyePosW - oPosW;
	
	//Pass along the texture
	oTex0 = tex0;
	

	//Project the position into homogeneous light space
	oProjTex = mul(float4(posL, 1.0f), gLightWVP);


	

}

float4 LightShadowPS(float3 posW    : TEXCOORD0,
                     float3 normalW : TEXCOORD1,
                     float3 toEyeW  : TEXCOORD2,
                     float2 tex0    : TEXCOORD3,
                     float4 projTex : TEXCOORD4) : COLOR
{
	//Do fog stuff:
	float4 texColor = tex2D(TexS, tex0);

	float distance = length(posW - gEyePosW);

	float fogEnd = 200.0;
	float fogBegin = 80.0;

	float f = (fogEnd - distance) / (fogEnd - fogBegin);

	float4 fogColor = float4(0.0, 0.0, 0.0, 1.0);

	float4 endTexColor = mul(texColor, f) + float4(mul(1.0 - f, fogColor));
	//endTexColor.a = 1.0;
	return endTexColor;
}

technique LightShadowTech
{
    pass P0
    {
	AlphaBlendEnable = true;
			SrcBlend = SrcAlpha;
			DestBlend = InvSrcAlpha;
		vertexShader = compile vs_2_0 LightShadowVS();
        pixelShader  = compile ps_2_0 LightShadowPS();


    }
}

technique LightShadowTechImp
{
    pass P0
    {

			AlphaBlendEnable = true;
			SrcBlend = SrcAlpha;
			DestBlend = InvSrcAlpha;
        
		vertexShader = compile vs_2_0 LightShadowVS();
        pixelShader  = compile ps_2_0 LightShadowPS();


    }
}