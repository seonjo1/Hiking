cbuffer MatrixBuffer
{
    matrix world;
    matrix view;
    matrix projection;
};

struct VertexInputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

PixelInputType ModelVertexShader(VertexInputType input)
{
    PixelInputType output;

    float4 position = float4(input.position, 1.0f);

    output.position = mul(world, position);
    output.position = mul(view, output.position);
    output.position = mul(projection, output.position);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    return output;
}
