#version 330 core
out vec4 color;
in vec2 uv;
in vec3 reflect;
uniform float time;
uniform sampler2D tex;
uniform sampler2D tex_mirror;
uniform sampler2D tex_refract;
in vec4 clipSpace;
vec4 tangent = vec4(1.0, 0.0, 0.0, 0.0);
vec4 lightNormal = vec4(0.0, 1.0, 0.0, 0.0);
vec4 biTangent = vec4(0.0, 0.0, 1.0, 0.0);
float kDistortion = 0.015;
float kReflection = 0.020;


void main() {
    vec2 T = vec2(-2*cos(3.14/180 *(time*2)),2*sin(3.14/180*(time*2)));
    vec2 T_2 = vec2(-2*cos(3.14/180 *(time*4)),2*sin(3.14/180*(time*4)));
    vec3 L = vec3(0, 1, 1);
    vec4 water_color = texture(tex, uv*4+T);
    vec4 water_color_sm = texture(tex, uv*8+T_2);
    vec4 lightDirection = normalize(vec4(L.xyz, 1.0));
    vec4 lightTanSpace = normalize(vec4(dot(lightDirection, tangent), dot(lightDirection, biTangent), dot(lightDirection, lightNormal), 1.0));
    vec4 distOffset = water_color * kDistortion;
    vec4 normal = texture(tex, vec2(uv*4 + distOffset.xy));
    normal = normalize(normal * 2.0 - 1.0);
    normal.a = 0.81;

    vec4 lightReflection = normalize((-1 * lightTanSpace) - 2.0 * dot(normal, (-1 * lightTanSpace)) * normal);
    vec4 invertedFresnel = vec4(dot(normal, lightReflection));
    vec4 fresnelTerm = 1.0 - invertedFresnel;

    vec4 dudvColor = texture(tex, vec2(uv*4 + distOffset.xy));
    dudvColor = normalize(dudvColor * 2.0 - 1.0) * kReflection;

    vec4 tmp = vec4(1.0 / clipSpace.w);
    vec4 projCoord = clipSpace * tmp;
    projCoord += vec4(1.0);
    projCoord *= vec4(0.5);
    projCoord += dudvColor;
    projCoord = clamp(projCoord, 0.001, 0.999);

//    float fresnel = 1 - max(dot( water_color, normalize(L) ), 0.0);
    float water_normal = max(dot( water_color.rgb, normalize(L) ), 0.0);
    float water_normal_2 = max(dot( water_color_sm.rgb, normalize(L) ), 0.0);
    vec2 ndc = (clipSpace.xy/clipSpace.w) *.5 + .5;
    vec2 reflection = vec2(ndc.x, ndc.y);
//    color = texture(tex_mirror,reflection).rgb;

    vec4 color_water = vec4(0.16, 0.67, 1.0, 0.0);
    vec4 color_from_mirror = mix(texture(tex_mirror,projCoord.xy), color_water, 0.3);
    color_from_mirror*=water_normal;
    color_from_mirror*=water_normal_2;

    vec4 color_from_refract = texture(tex_refract,projCoord.xy);
    vec4 depthValue = vec4(0.1, 0.1, 0.1, 1.0);
    vec4 invDepth = 1.0 - depthValue;
    color_from_refract *= invertedFresnel * invDepth;
    color_from_refract += color_water * depthValue * invertedFresnel;
    vec4 rr = mix(color_from_mirror, color_from_refract, vec4(.5));
//    color = mix(color_water, rr, vec4(.5)).rgb;
    color = color_from_mirror + color_from_refract;

}
