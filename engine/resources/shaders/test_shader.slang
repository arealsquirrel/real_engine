
struct LightData {
	float3 position;
	float3 color;
};

struct SceneData {
	float4 view;
	float4 proj;
	float4 ambient_light_direction;
	float4 ambient_light_color;
	float ambient_light;
};

struct AllTypesAhStruct {
	float f1;
	float2 f2;
	float3 f3;
	float4 f4;
}

[vk::push_constant]
cbuffer PushConstants {
	float4 topColor;
	float4 bottomColor;
	float arr[10];
	AllTypesAhStruct sarr[3];
};

RWTexture2D<float4> image;
ConstantBuffer<LightData> lights[32];
ConstantBuffer<SceneData> data;
ConstantBuffer<Sampler2D> imageSamplers[32];

[shader("compute")]
[numthreads(16, 32, 1)]
void compute_main(
	uint3 globalID : SV_DispatchThreadID,
    uint3 localID  : SV_GroupThreadID) {

    uint width, height;
    image.GetDimensions(width, height);

    int2 texelCoord = int2(globalID.xy);

    if(texelCoord.x < width && texelCoord.y < height) {
        float blend = float(texelCoord.y)/(height);

        image[texelCoord] = lerp(data.ambient_light_color * float4(lights[texelCoord.x].color, 1.0f), bottomColor, blend);
    }
}
