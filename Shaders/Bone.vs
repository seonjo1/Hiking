struct VertexInput {
    float3 position : POSITION;
    float4 color : COLOR;
    int idx : IDX;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer MatrixBuffer : register(b0) {
    matrix view;
    matrix projection;
};

cbuffer WorldTransforms : register(b1) {
    matrix worldTransforms[4];
};

cbuffer MoveVector : register(b2) {
    float3 moveVector[4];
};

PixelInputType BoneVertexShader(VertexInput input) {
    PixelInputType output;
    
    float4 worldPos   = mul(worldTransforms[idx], float4(input.position, 1.0f));
    worldPos = worldPos + moveVector[idx];
    float4 viewPos    = mul(view, worldPos); 
    float4 projPos    = mul(projection, viewPos);

    output.position = projPos;
    output.color = input.color;
    
    return output;
}
