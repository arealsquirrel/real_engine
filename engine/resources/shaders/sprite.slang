
struct Vertex {
	float4x4 model;
	float2 uv0;
	float2 uv1;
	float4 color;
}

struct VSOutput {
    float4 position : SV_Position;

    float4 color;
    float2 uv;
};

struct PSInput {
    float4 color;
    float2 uv;
};

struct SceneData {
    float4x4 projection;
	float4x4 view;
};

[[vk::push_constant]]
cbuffer PushConstants {
	Vertex *_vertex_buffer; // SET INTERNALY BY THE ENGINE
};

Sampler2D sampler;
ConstantBuffer<SceneData> scene_data;

static const float2 vertexes[6] = {
	float2(0, 0),
	float2(0, 1),
	float2(1, 1),
	float2(1, 1),
	float2(1, 0),
	float2(0, 0),
};

[shader("vertex")]
VSOutput vertex_main(
	uint vertex_index : SV_VertexID,
	uint instance_index : SV_InstanceID) {

	VSOutput output;
    Vertex v = _vertex_buffer[instance_index];
	float2 base_coord = vertexes[vertex_index % 6];
    output.position = mul(scene_data.projection,
				mul(scene_data.view,
				mul(v.model, float4(base_coord, 0.0f, 1.0f)))); 
	
	output.uv = (1.f - base_coord) * v.uv0 + base_coord * v.uv1;
	output.color = v.color;

	return output;
}

[shader("fragment")]
float4 fragment_main(PSInput input) : SV_Target {
	float4 color = input.color * sampler.Sample(input.uv);

	if(color.w < 0.1)
		discard;

	return color;
}

