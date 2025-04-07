struct VertexInput {
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    uint4 boneIndices : BONEINDICES;
    float4 boneWeights : BONEWEIGHTS;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

cbuffer MatrixBuffer : register(b0) {
    matrix world;
    matrix view;
    matrix projection;
};

cbuffer BoneTransforms : register(b1) {
    matrix boneTransforms[100];
};

PixelInputType TextureVertexShader(VertexInput input) {
    PixelInputType output;
    
    float4 skinnedPos = mul(boneTransforms[input.boneIndices[0]], (float4(input.position, 1.0f)) * input.boneWeights[0]) +        
                        mul(boneTransforms[input.boneIndices[1]], (float4(input.position, 1.0f)) * input.boneWeights[1]) +
                        mul(boneTransforms[input.boneIndices[2]], (float4(input.position, 1.0f)) * input.boneWeights[2]) +
                        mul(boneTransforms[input.boneIndices[3]], (float4(input.position, 1.0f)) * input.boneWeights[3]);
    float4 worldPos   = mul(world, skinnedPos);
    float4 viewPos    = mul(view, worldPos); 
    float4 projPos    = mul(projection, viewPos);

    output.position = projPos;
    output.tex = input.tex;
    
    return output;
}
