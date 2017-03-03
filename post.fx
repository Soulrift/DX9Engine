//Basic Post-Process Shader - Display Quad
//v1.0 - Just draw quad atm

uniform extern float4x4 gWVP;
uniform extern texture gTex;

sampler TexS = sampler_state
{
	Texture = <gTex>;
};
 
struct OutputVS
{
	float2 tex0    : TEXCOORD0;
    float4 posH	   : POSITION0;
};

OutputVS PostVS(float3 posL : POSITION0, float2 tex0: TEXCOORD0)
{
     // Zero out our output.
	OutputVS outVS = (OutputVS)0;
    outVS.posH = mul(float4(posL,1.0), gWVP);
	outVS.tex0 = tex0;
    return outVS;
}

float4 PostPS(float2 tex0 : TEXCOORD0) : COLOR
{
	float4 texColor = tex2D(TexS, tex0);
    return texColor;
}

technique PostProcess
{
    pass P0
    {
        vertexShader = compile vs_2_0 PostVS();
        pixelShader  = compile ps_2_0 PostPS();
    }
}
