
struct VSOutput {
    float4 position : SV_Position;
    float3 color    : COLOR0;
};

struct PSInput {
    float3 inColor : COLOR0;
};

[shader("vertex")]
VSOutput vertex_main(uint vertexIndex : SV_VertexID) {
    VSOutput output;

    // Const array of positions for the triangle
    const float3 positions[3] = {
        float3( 1.0f,  1.0f, 0.0f),
        float3(-1.0f,  1.0f, 0.0f),
        float3( 0.0f, -1.0f, 0.0f)
    };

    // Const array of colors for the triangle
    const float3 colors[3] = {
        float3(1.0f, 0.0f, 0.0f), // red
        float3(0.0f, 1.0f, 0.0f), // green
        float3(0.0f, 0.0f, 1.0f)  // blue
    };

    // Output the position and color of each vertex
    output.position = float4(positions[vertexIndex], 1.0f);
    output.color = colors[vertexIndex];

    return output;
}

[shader("fragment")]
float4 fragment_main(PSInput input) : SV_Target {
    return float4(input.inColor, 1.0f);
}

