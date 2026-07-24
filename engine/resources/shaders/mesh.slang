
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
    float4 frag_pos;
	float3 color;
	float3 normal;
    float2 uv;
	uint texture_index;
	uint shader_id;
};

struct PSInput {
	float4 frag_pos;
	float3 color;
	float3 normal;
	float2 uv;
	uint texture_index;
	uint shader_id;
};

struct SceneData {
    float4x4 projection;
    float4x4 view;
	float3 view_position;
	float ambient_power;
	float4 light_color;
	float4 light_position;
};

[[vk::push_constant]]
cbuffer PushConstants {
    float4x4 model;
	float3x3 inv_model;
	Vertex *_vertex_buffer; // SET INTERNALY BY THE ENGINE
	uint texture_index;
	uint shader_id;
};

Sampler2D sampler[32];
ConstantBuffer<SceneData> scene_data;

[shader("vertex")]
VSOutput vertex_main(uint vertexIndex : SV_VertexID) {
    VSOutput output;
    Vertex v = _vertex_buffer[vertexIndex];
	
	output.frag_pos = mul(model, float4(v.position, 1.0f));
    output.position = mul(scene_data.projection, mul(scene_data.view, output.frag_pos));
    output.color = v.color.xyz;
    output.uv = float2(v.uv_x, v.uv_y);
	output.texture_index = texture_index;
	output.shader_id = shader_id;
	output.normal = mul(transpose(inv_model), v.normal);

    return output;
}

float4 diffuse_light(float3 vertPos, float3 normal) {
	float ambientStrength = scene_data.ambient_power;
	float specularStrength = 0.5;
	float3 lightColor = scene_data.light_color.xyz;
	float3 lightPos = scene_data.light_position.xyz;

	float3 norm = normalize(normal);
	float3 lightDir = normalize(lightPos - vertPos);
	float diff = max(dot(norm, lightDir), 0.0);
	float3 diffuse = diff * lightColor;

	float3 viewDir = normalize(scene_data.view_position - vertPos);
	float3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	float3 specular = specularStrength * spec * lightColor;  


    float3 ambient = ambientStrength * lightColor;
	return float4(ambient + diffuse + specular, 1.0f);
}

[shader("fragment")]
float4 fragment_main(PSInput input) : SV_Target {
	float4 color = float4(1.0f, 0.0f, 0.0f, 1.0f);

	if(input.shader_id == 0) { // flat
		color = float4(input.color, 1.0f) * sampler[input.texture_index].Sample(input.uv);
	} else if(input.shader_id == 1) { // lit (we dont have a lit)
		color = float4(input.color, 1.0f) * sampler[input.texture_index].Sample(input.uv) * diffuse_light(input.frag_pos.xyz, input.normal);
	} else if(input.shader_id == 2) { // normals
		color = float4(input.normal, 1.0f);
	} else if(input.shader_id == 3) { // UV
		color = float4(input.uv, 0.0f, 1.0f);
	}

	return color;
}

