/*********************************************************************************
Name:			bloom.fx
Description:	bloom pixel shader implemented in Mecha-Ziu
*********************************************************************************/

//Constants
static const int cKernelSize = 4;
static const float fBrightThreshold = 0.00f; 
static const float fBloomScale = 1.5f;
static const float fBloomRadius = 1.0f;

//Extern parameters
uniform extern texture 		Tex;
uniform extern texture		OriginalTex; 
uniform extern unsigned int	unWidth;
uniform extern unsigned int	unHeight;

//Offsets
uniform extern float2 		BlurOffsetsH[cKernelSize];
uniform extern float2 		BlurOffsetsV[cKernelSize];
uniform extern float2 		ScaleOffsets[16];

//Samplers
sampler2D g_SampTex = sampler_state
{
	Texture = <Tex>;
	AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};

sampler2D g_SampOriginalTex = sampler_state
{
	Texture = <OriginalTex>;
	AddressU = Clamp;
    AddressV = Clamp;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = None;
};

//Blur Weights
static const float BlurWeights[cKernelSize] = 
{
	1.0f / 8.0f,
	3.0f / 8.0f,
	3.0f / 8.0f,
	1.0f / 8.0f,
}; 

//Brightpass
float4 DownSample4xBrightPass(float2 uv : TEXCOORD1 ) : COLOR0
{   
    float4 Color = 0;
	float4 temp = 0; 
   
    for (int i = 0; i < 16; i++)
    {
		temp = tex2D( g_SampTex, uv + ScaleOffsets[i].xy ); 
        Color += temp * temp.a;
    }
    
    return Color / 16;
}

//Horizontal Blur
float4 HorizontalBlurPS(float2 uv : TEXCOORD1 ) : COLOR0
{
    float4 Color = 0;
    
    for (int i = 0; i < cKernelSize; i++)
    {    
        Color += tex2D( g_SampTex, uv + BlurOffsetsH[i].xy * fBloomRadius) * BlurWeights[i];
    }

    return Color * fBloomScale;
}

//Vertical Blur
float4 VerticalBlurPS(float2 uv : TEXCOORD1 ) : COLOR0
{
    float4 Color = 0;

    for (int i = 0; i < cKernelSize; i++)
    {
        Color += tex2D( g_SampTex, uv + BlurOffsetsV[i].xy * fBloomRadius) * BlurWeights[i];
    }

    return Color * fBloomScale;
}

//Upscale and blend
float4 UpSample4xBlend(float2 uv : TEXCOORD0 ) : COLOR0
{
	float4 bloomColor = tex2D( g_SampTex, uv);
	float4 origColor = tex2D( g_SampOriginalTex, uv);
	
	//return origColor;
	//return bloomColor;
	//return float4(bloomColor.xyz * bloomColor.a, 1.0f) + float4(origColor.xyz * (1 - bloomColor.a), 1.0f);	
	return origColor + bloomColor;
}

technique TBloom
{
    pass p0
    {
        VertexShader = null;
        PixelShader = compile ps_2_0 DownSample4xBrightPass();
    }

    pass p1
    {
		VertexShader = null;
        PixelShader = compile ps_2_0 HorizontalBlurPS();
    }

    pass p2
    {
        VertexShader = null;
        PixelShader = compile ps_2_0 VerticalBlurPS();
    }

    pass p3
    {
        VertexShader = null;
        PixelShader = compile ps_2_0 UpSample4xBlend();
    }
}