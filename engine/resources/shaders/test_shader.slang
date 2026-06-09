
struct LightData {
	float3 position;
	float3 color;
};

struct SceneData {
	float4 view;
	float4 proj;
	float4 ambient_light_direction;
	float3 ambient_light_color;
	float ambient_light;
};

[[vk::push_constant]]
cbuffer PushConstants {
    float4x4 model;
};

uniform Sampler2D in_texture;
uniform LightData light_data[10];

[shader("compute")]
[numthreads(16, 16, 1)]
void compute_main(
	uint3 globalID : SV_DispatchThreadID,
	uint3 localID  : SV_GroupThreadID) {
}
