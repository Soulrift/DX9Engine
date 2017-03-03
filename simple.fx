uniform extern float4x4 gWVP;
uniform extern float4x4 gWorld;
uniform extern float4 gDiffuse;


struct VertexOut
{
    float4 posH		: POSITION0;
    float4 colH	    : COLOR0;
};

VertexOut VShader(float4 Pos : POSITION0, float3 Normal : NORMAL)
{
    VertexOut Vert = (VertexOut)0;
	float3 LightPos = float3(-1, 5, 20);
	float3 AmbientColor = float3(0.3, 0.3, 0.3);
	float3 DiffuseColor = gDiffuse.xyz;
	float4 Position = Pos;
	Position = mul(Position, gWorld);
	//Pos = mul(Pos, gWorld);
	Normal = mul(Normal, gWorld);
	float3 LightVec = normalize(LightPos - Position);
	DiffuseColor = dot(Normal, LightVec) * DiffuseColor;
	DiffuseColor = max(DiffuseColor, 0);

	Vert.colH.rgb = DiffuseColor + AmbientColor;

    Vert.posH = mul(Pos, gWVP);
    //Vert.colH = gDiffuse;
    return Vert;
}


technique FirstTechnique
{
    pass FirstPass
    {
        Lighting = TRUE;
        ZEnable = TRUE;

        VertexShader = compile vs_2_0 VShader();
		
    }
}