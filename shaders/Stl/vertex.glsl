#version 410
layout(location = 0) in highp vec4 vertex;
layout(location = 1) in highp vec3 normal;

uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 projection;

uniform highp mat3 normalMatrix;

layout(location = 0) out highp vec4 pos;
layout(location = 1) out highp vec3 vertexTest;
layout(location = 2) out highp vec4 N;

void main(void) {
    pos = model * vertex;

    vertexTest = vec3(vertex);
    N = vec4(normalize(normalMatrix * normal), 0.0);

    gl_Position = projection * view * pos;
}
