
// Slang vertex shader equivalent
struct Vertex {
    float3 position;
    float  uv_x;
    float3 normal;
    float  uv_y;
    float4 color;
};

struct VSOutput {
    float4 position     : SV_Position;
    float4 color        : COLOR0;
    float2 uv           : UV0;
	float4 vert_norm    : NORM0;
	float4 vert_pos     :  POS0;
};

struct PSInput {
    float4 color : COLOR0;
    float2 uv : UV0;
    float4 vert_norm : NORM0;
    float4 vert_pos : POS0;
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
    float3x3 model_inv;
    Vertex *_vertex_buffer; // SET INTERNALY BY THE ENGINE
};

Sampler2D sampler;
ConstantBuffer<SceneData> scene_data;

[shader("vertex")]
VSOutput vertex_main(uint vertexIndex : SV_VertexID) {
    VSOutput output;
    Vertex v = _vertex_buffer[vertexIndex];
    float4 mpos = mul(model, float4(v.position, 1.0f));

    output.position = mul(scene_data.projection, mul(scene_data.view, mpos));
    output.color = v.color;
    output.uv = float2(v.uv_x, v.uv_y);
    output.vert_pos = mpos;
    output.vert_norm = float4(mul(model_inv, v.normal), 1.0f);

    return output;
}

[shader("fragment")]
float4 fragment_main(PSInput input) : SV_Target {
    float3 norm = normalize(input.vert_norm.xyz);
    float3 lightDir = normalize(scene_data.light_position - input.vert_pos.xyz);
    float diff = max(dot(norm, lightDir), 0.0);
    float4 diffuse = float4(diff * scene_data.light_color, 1.0f);

    float4 ambient = float4(scene_data.ambient * scene_data.light_color, 1.0f);
    float4 flat_color = input.color * sampler.Sample(input.uv);

    return (ambient + diffuse) * flat_color;
}

