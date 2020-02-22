/*********************************************************************************
Name:			noise.fx
Description:	pixel shader to make noise
*********************************************************************************/

//noise texture

uniform extern texture 		tex;
uniform extern texture		blendTex;
uniform extern float		time;
uniform extern float		alpha;

sampler3D noiseVolume = sampler_state 
{
	texture = <tex>;
	AddressU = wrap; 
	AddressV = wrap;
	AddressW = wrap;
	minfilter = linear;
	magfilter = linear;
};

sampler2D blendSampler = sampler_state
{
	texture = <blendTex>;
	AddressU = wrap; 
	AddressV = wrap;
	minfilter = linear;
	magfilter = linear;
};

float4 NoisePixelShader(float2 uv : TEXCOORD0 ) : COLOR0
{
	float4 color = 0;
	float3 position = float3(uv.x , uv.y, time * 0.5);

	color += tex3D(noiseVolume, position);
	color += tex3D(noiseVolume, 2 * position);
	color += tex3D(noiseVolume, 4 * position);
	//color += tex3D(noiseVolume, 8 * position);
	//color += tex3D(noiseVolume, 16 * position);

	color *= 0.33;
	color *= time * 3; 

	//CRT Fade Out
	if(time > 2.5)
	{
		color = (3.5 - time * 1.2) - length(uv - 0.5);
		color *= time * 2;
	}
	
	return (color * float4(0.5, 0.5, 1.0, 1.0)) * 0.5 + tex2D(blendSampler, uv) * 0.7;
}

technique TNoise
{
    pass p0
    {
        VertexShader = null;
        PixelShader = compile ps_2_0 NoisePixelShader();
    }
}