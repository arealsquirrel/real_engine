
[vk::binding(0,0)]
RWTexture2D<float4> image : register(u0);

[shader("compute")][numthreads(16, 16, 1)]
void main(
    uint3 dispatchThreadID : SV_DispatchThreadID,
    uint3 groupThreadID    : SV_GroupThreadID) {

    uint2 texelCoord = dispatchThreadID.xy;

    uint width, height;
    image.GetDimensions(width, height);

    if (texelCoord.x < width && texelCoord.y < height) {
        float4 color = float4(0, 0, 0, 1);
		
		color.x = float(texelCoord.x) / float(width);
		color.y = float(texelCoord.y) / float(height);

        image[texelCoord] = color;
    }
}

