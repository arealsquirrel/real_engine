
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
	float3 vert_norm;
	float3 vert_pos;
};

struct PSInput {
	float3 color  : COLOR0;
	float2 uv;
	float3 vert_norm;
	float3 vert_pos;
};

[[vk::push_constant]]
cbuffer PushConstants {
    float4x4 render_matrix;
	Vertex *_vertex_buffer; // SET INTERNALY BY THE ENGINE
};

[shader("vertex")]
VSOutput vertex_main(uint vertexIndex : SV_VertexID) {
    VSOutput output;
    Vertex v = _vertex_buffer[vertexIndex];

    output.position = mul(render_matrix, float4(v.position, 1.0f));
    output.color = v.color.xyz;
    output.uv = float2(v.uv_x, v.uv_y);
	output.vert_pos = v.position;
	output.vert_norm = v.normal;

    return output;
}

[shader("fragment")]
float4 fragment_main(PSInput input) : SV_Target {
	float3 objectColor = input.color;
	float3 lightColor = float3(1.0f, 1.0f, 1.0f);
	float3 lightPos = float3(0.0f, 0.0f, -4.0f);

	float ambientStrength = 0.1;
    float3 ambient = ambientStrength * lightColor;

	float3 norm = normalize(input.vert_norm);
	float3 lightDir = normalize(lightPos - input.vert_pos);
	float diff = max(dot(norm, lightDir), 0.0);
	float3 diffuse = diff * lightColor;

    float3 result = (ambient + diffuse) * objectColor;

	return float4(result, 1.0f);
}

