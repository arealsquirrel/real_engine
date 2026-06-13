
[vk::binding(0,0)]
RWTexture2D<float4> image;

[vk::push_constant]
cbuffer PushConstants {
	float4 topColor;
	float4 bottomColor;
};

[shader("compute")]
[numthreads(16, 16, 1)]
void compute_main(
	uint3 globalID : SV_DispatchThreadID,
    uint3 localID  : SV_GroupThreadID) {

    uint width, height;
    image.GetDimensions(width, height);

    int2 texelCoord = int2(globalID.xy);

    if(texelCoord.x < width && texelCoord.y < height) {
		if(image[texelCoord].w == 1) {
			return;
		}

        float blend = float(texelCoord.y) / (height);
		float4 gradient_color = lerp(topColor, bottomColor, blend);
    	image[texelCoord] = lerp(gradient_color, image[texelCoord], image[texelCoord].w);
    }
}
