/*********************************************************************************
Name:			bloom.fx
Description:	bloom pixel shader implemented in Mecha-Ziu
*********************************************************************************/

//Constants
static const int cKernelSize = 13;
static const float fBrightThreshold = 0.6f; 
static const float fBloomScale = 1.5f;

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
    MinFilter = Point;
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
	1.0f / 4096.0f,
	12.0f / 4096.0f,
	66.0f / 4096.0f,
	220.0f / 4096.0f,
	495.0f / 4096.0f,
	792.0f / 4096.0f,
	924.0f / 4096.0f,
	792.0f / 4096.0f,
	495.0f / 4096.0f,
	220.0f / 4096.0f,
	66.0f / 4096.0f,
	12.0f / 4096.0f,
	1.0f / 4096.0f,
}; 

//Brightpass
float4 DownSample4xBrightPass(float2 uv : TEXCOORD1 ) : COLOR0
{   
    float4 Color = 0;
   
    for (int i = 0; i < 16; i++)
    {
        Color += tex2D( g_SampTex, uv + ScaleOffsets[i].xy ) - fBrightThreshold;
    }

    return Color / 16;
}

//Horizontal Blur
float4 HorizontalBlurPS(float2 uv : TEXCOORD1 ) : COLOR0
{
    float4 Color = 0;
    
    for (int i = 0; i < cKernelSize; i++)
    {    
        Color += tex2D( g_SampTex, uv + BlurOffsetsH[i].xy ) * BlurWeights[i];
    }

    return Color * fBloomScale;
}

//Vertical Blur
float4 VerticalBlurPS(float2 uv : TEXCOORD1 ) : COLOR0
{
    float4 Color = 0;

    for (int i = 0; i < cKernelSize; i++)
    {
        Color += tex2D( g_SampTex, uv + BlurOffsetsV[i].xy ) * BlurWeights[i];
    }

    return Color * fBloomScale;
}

//Upscale and blend
float4 UpSample4xBlend(float2 uv : TEXCOORD0 ) : COLOR0
{
	return tex2D( g_SampTex, uv) * 0.8 + tex2D( g_SampOriginalTex, uv);
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