struct VertexInput {
    float3 position : POSITION;
    float4 color    : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer MatrixBuffer : register(b0) {
    matrix world;
    matrix view;
    matrix projection;
};

PixelInputType JointVertexShader(VertexInput input) {
    PixelInputType output;
    
    float4 worldPos   = mul(world, float4(input.position, 1.0f));
    float4 viewPos    = mul(view, worldPos); 
    float4 projPos    = mul(projection, viewPos);

    output.position = projPos;
    output.color = input.color;
    
    return output;
}
