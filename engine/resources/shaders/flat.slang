
// Slang vertex shader equivalent
struct Vertex {
    float3 position;
    float  uv_x;
    float3 normal;
    float  uv_y;
    float4 color;
};

struct VSOutput {
    float4 position : SV_Position;
    float3 color;
    float2 uv;
};

struct PSInput {
	float3 color;
	float2 uv;
};

struct SceneData {
    float4x4 projection;
    float4x4 view;
    float ambient;
    float3 light_color;
    float3 light_position;
};

[[vk::push_constant]]
cbuffer PushConstants {
    float4x4 model;
	Vertex *_vertex_buffer; // SET INTERNALY BY THE ENGINE
};

Sampler2D sampler;
ConstantBuffer<SceneData> scene_data;

[shader("vertex")]
VSOutput vertex_main(uint vertexIndex : SV_VertexID) {
    VSOutput output;
    Vertex v = _vertex_buffer[vertexIndex];

    output.position = output.position = mul(scene_data.projection, mul(scene_data.view, mul(model, float4(v.position, 1.0f))));
    output.color = v.color.xyz;
    output.uv = float2(v.uv_x, v.uv_y);

    return output;
}

[shader("fragment")]
float4 fragment_main(PSInput input) : SV_Target {
	return float4(input.color, 1.0f) * sampler.Sample(input.uv);
}

