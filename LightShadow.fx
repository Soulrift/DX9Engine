uniform extern float4x4 gLightWVP;

static const float SHADOW_EPSILON = 0.00005f;
static const float SMAP_SIZE = 512.0f;

void BuildShadowMapVS(float3 posL : POSITION0,
                      out float4 posH : POSITION0,
                      out float2 depth : TEXCOORD0)
{
	//Pull Local Coordinates into Light's Homogeneous Space
	posH = mul(float4(posL, 1.0f), gLightWVP);
	
	//Push out the z & w coordinates as depth
	depth = posH.zw;
}

float4 BuildShadowMapPS(float2 depth : TEXCOORD0) : COLOR
{
	//Normalize the depth and store in the texture (following the output from VS, this is essentially  z / w, or normalized z)
	return (depth.x / depth.y); 
}

technique BuildShadowMapTech
{
	pass P0
	{
		vertexShader = compile vs_2_0 BuildShadowMapVS();
        pixelShader  = compile ps_2_0 BuildShadowMapPS();
	}
}

struct Mtrl
{
	float4 diffuse;
	float4 ambient;
	float4 spec;
	float4 Emissive;
	float  specPower;
};

struct SpotLight
{
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float3 posW;
	float3 dirW;  
	float  spotPower;
};

uniform extern float4x4  gWorld;
uniform extern float4x4  gWVP;
uniform extern Mtrl      gMtrl;
uniform extern SpotLight gLight;
uniform extern float3    gEyePosW;
uniform extern texture   gTex;
uniform extern texture   gShadowMap;
 
sampler TexS = sampler_state
{
	Texture = <gTex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU  = WRAP; 
    AddressV  = WRAP;
};

sampler ShadowMapS = sampler_state
{
	Texture = <gShadowMap>;
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = POINT;
	AddressU  = CLAMP; 
    AddressV  = CLAMP;
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
	//normalize again
	normalW = normalize(normalW);
	toEyeW  = normalize(toEyeW);
	
	// Light vector is from pixel to spotlight position.
	float3 lightVecW = normalize(gLight.posW - posW);
	
	// Compute the reflection vector.
	float3 r = reflect(-lightVecW, normalW);
	
	// Determine how much (if any) specular light makes it into the eye.
	float t  = pow(max(dot(r, toEyeW), 0.0f), gMtrl.specPower);
	
	// Determine the diffuse light intensity that strikes the vertex.
	float s = max(dot(lightVecW, normalW), 0.0f);
	
	// Compute the ambient, diffuse and specular terms separately. 
	float3 spec = t*(gMtrl.spec*gLight.spec).rgb;
	float3 diffuse = s*(gMtrl.diffuse*gLight.diffuse.rgb);
	float3 ambient = gMtrl.ambient*gLight.ambient;
	
	// Compute spotlight coefficient.
	float spot = pow(max( dot(-lightVecW, gLight.dirW), 0.0f), gLight.spotPower);
	
	// Sample decal map.
	float4 texColor = tex2D(TexS, tex0); 
	
	//Normalize texture coords and round.  Returns values 0-1
	projTex.xy /= projTex.w;            
	projTex.x =  0.5f*projTex.x + 0.5f; 
	projTex.y = -0.5f*projTex.y + 0.5f;
	
	//Pull out the depth value (re-normalize)
	float depth = projTex.z / projTex.w;
 
	//Transform into texel (texture) space
    float2 texelpos = SMAP_SIZE * projTex.xy;
        
    //Determine the amount we're going to lerp (pull out fractional position so we can weight the value in the lerp function)          
    float2 lerps = frac( texelpos );
    
    // 2x2 Filtering - Pull boolean value "if adjacent pixel is more shaded" 0.0f if false, 1.0f if true
    float dx = 1.0f / SMAP_SIZE;
	float s0 = (tex2D(ShadowMapS, projTex.xy).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
	float s1 = (tex2D(ShadowMapS, projTex.xy + float2(dx, 0.0f)).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
	float s2 = (tex2D(ShadowMapS, projTex.xy + float2(0.0f, dx)).r + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
	float s3 = (tex2D(ShadowMapS, projTex.xy + float2(dx, dx)).r   + SHADOW_EPSILON < depth) ? 0.0f : 1.0f;
	
	//Calculate weighted average using lerp values from earlier
	float shadowCoeff = lerp( lerp( s0, s1, lerps.x ),
                              lerp( s2, s3, lerps.x ),
                              lerps.y );
	
	//Apply lighting/shadows to the texture
	float3 litColor = spot*ambient*texColor.rgb + spot*shadowCoeff*(diffuse*texColor.rgb + spec);
	
	//Grab any alpha in the diffuse material
	return float4(litColor, gMtrl.diffuse.a*texColor.a);
}

technique LightShadowTech
{
    pass P0
    {
 
        vertexShader = compile vs_2_0 LightShadowVS();
        pixelShader  = compile ps_2_0 LightShadowPS();
    }
}
