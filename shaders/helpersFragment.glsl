#version 410
uniform highp sampler2D points;

uniform highp int pointsCount;

struct Material {
    vec4 emissive;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform highp Material material;

struct LightSource {
    vec4 position;
    vec4 color;
    float ambientIntensity;
};

uniform highp LightSource lightSource;

struct Ranges {
   vec2 xRange;
   vec2 yRange;
   vec2 zRange;
};

uniform highp Ranges ranges;


bool needToRender(const vec3 position,
                  const vec2 xab, const vec2 yab, const vec2 zab);

vec4 calcFragColor(const vec4 position, const vec4 normal, const vec4 color);

vec4 highlightColor(const vec3 position);


bool needToRender(const vec3 position,
                  const vec2 xab, const vec2 yab, const vec2 zab) {
    return position.x >= ranges.xRange.s * xab.s + xab.t
           && position.x <= ranges.xRange.t * xab.s + xab.t
           && position.y >= ranges.yRange.s * yab.s + yab.t
           && position.y <= ranges.yRange.t * yab.s + yab.t
           && position.z >= ranges.zRange.s * zab.s + zab.t
           && position.z <= ranges.zRange.t * zab.s + zab.t;
}


vec4 calcFragColor(const vec4 position, const vec4 normal, const vec4 color) {
    vec4 L = normalize(lightSource.position - position);

    float NdotL = max(dot(normal, L), 0);
    vec4 diffuse =  NdotL * lightSource.color * material.diffuse;

    vec4 V = normalize(normal - position);
    vec4 H = normalize(L + V);
    vec4 R = reflect(-L, normal);

    float RdotV = max(dot(R, V), 0);
    float NdotH = max(dot(normal, H), 0);

    vec4 specular = pow(RdotV, material.shininess) * lightSource.color * material.specular;

    vec4 hColor = highlightColor(vec3(position));

    return (material.emissive + lightSource.ambientIntensity + diffuse + specular) *
            ((hColor != vec4(0.0)) ? hColor : color);
}


vec4 highlightColor(const vec3 position) {
    vec4 color = vec4(0.0f);

    vec4 pointPos;
    for (int i = 0; i != pointsCount; ++ i) {
        // x, y, z - point coords, w - color radius
        pointPos = texture(points, vec2(0, i));

        if ((position.x - pointPos.x) * (position.x - pointPos.x) +
            (position.y - pointPos.y) * (position.y - pointPos.y) +
            (position.z - pointPos.z) * (position.z - pointPos.z) < pointPos.w * pointPos.w) {
            color += texture(points, vec2(1, i));
        }
    }

    return color;
}
