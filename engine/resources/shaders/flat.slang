
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

[[vk::push_constant]]
cbuffer PushConstants {
    float4x4 render_matrix;
	Vertex *_vertex_buffer; // SET INTERNALY BY THE ENGINE
};

// Sampler2D sampler;

[shader("vertex")]
VSOutput vertex_main(uint vertexIndex : SV_VertexID) {
    VSOutput output;
    Vertex v = _vertex_buffer[vertexIndex];

    output.position = mul(render_matrix, float4(v.position, 1.0f));
    output.color = v.color.xyz;
    output.uv = float2(v.uv_x, v.uv_y);

    return output;
}

[shader("fragment")]
float4 fragment_main(PSInput input) : SV_Target {
	// float4 color = in_texture.Sample(input.uv);
	return float4(input.color, 1.0f); // * sampler.Sample(input.uv);
}

