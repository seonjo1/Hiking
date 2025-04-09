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

cbuffer Vectors : register(b2) {
    float4 cameraFrontVector;
    float4 toChildVector;
};

PixelInputType BoneVertexShader(VertexInput input) {
    PixelInputType output;
    
    int idx = input.idx;

    float4 worldPos   = mul(worldTransforms[idx], float4(input.position, 1.0f));
    int sign = ((idx & 1) << 1) - 1;
    float3 crossRes = normalize(cross(cameraFrontVector.xyz, toChildVector.xyz)) * sign * 0.05f; 
    worldPos = worldPos + float4(crossRes, 0.0f);
    float4 viewPos    = mul(view, worldPos); 
    float4 projPos    = mul(projection, viewPos);

    output.position = projPos;
    output.color = input.color;
    
    return output;
}
