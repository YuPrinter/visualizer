#version 410
layout(location = 0) in highp vec3 vertex;
layout(location = 1) in highp vec3 color;
layout(location = 2) in highp float polygon;

out vData {
    highp vec3 color;
    highp float polygon;
} oVertex;

void main(void) {
    gl_Position = vec4(vertex, 1.0f);

    oVertex.color = color;
    oVertex.polygon = polygon;
}
