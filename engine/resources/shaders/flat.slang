
// Slang vertex shader equivalent
struct Vertex
{
    float3 position;
    float  uv_x;
    float3 normal;
    float  uv_y;
    float4 color;
};

struct VSOutput
{
    float4 position : SV_Position;
    float3 color    : COLOR0;
    float2 uv       : TEXCOORD0;
};

// [[vk::binding(0,0)]]
StructuredBuffer<Vertex> vertexBuffer;

[[vk::push_constant]]
cbuffer PushConstants {
    float4x4 render_matrix;
};

[shader("vertex")]
VSOutput vertex_main(uint vertexIndex : SV_VertexID)
{
    VSOutput output;

    // Load vertex data
    Vertex v = vertexBuffer[vertexIndex];

    // Output transformed position
    output.position = mul(render_matrix, float4(v.position, 1.0f));

    // Pass through attributes
    output.color = v.color.xyz;
    output.uv = float2(v.uv_x, v.uv_y);

    return output;
}
