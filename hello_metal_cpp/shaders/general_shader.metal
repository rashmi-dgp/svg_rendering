#include <metal_stdlib>
using namespace metal;

struct VertexInput {
    float2 position [[attribute(0)]];
    float3 color [[attribute(1)]];
};

struct VertexOutput {
    float4 position [[position]];
    half3 color;
};

struct transform {
    float zoomFactor;
    float panX;
    float panY;
    float padding; // Padding to align to 16 bytes
};

VertexOutput vertex vertexMainGeneral(VertexInput input [[stage_in]]) {
    
    VertexOutput payload;
    float zoomFactor = transform[0].x;
    float panX = transform[0].y;
    float panY = transform[0].z;
//    payload.position = float4(input.position, 0.0, 1.0);
//    payload.color = half3(input.color);
    
    float2 zoomedPosition = input.position * zoomFactor;
       zoomedPosition.x += panX;
       zoomedPosition.y += panY;
    
    payload.position = float4(zoomedPosition, 0.0,1.0);
    payload.color    = half3(input.color);
    
    return payload;
}

half4 fragment fragmentMainGeneral(VertexOutput frag [[stage_in]]) {
    return half4(frag.color, 1.0);
}
