#version 330 core
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJ;
in vec3 vpoint;
in vec2 vtexcoord;
out vec2 uv;
out vec2 uv2;
out vec4 clipSpace;
out float vheight; ///< for debug coloring
out float scale=0.0;
uniform float time;

uniform sampler2D tex_displacement;
float tex_at(vec2 uv){ return texture(tex_displacement,uv).r; }

void main() {
    vec2 T = vec2(-2*cos(3.14/180 *(time*8)),2*sin(3.14/180*(time*8)));
    vec2 tex_value = .25*vec2(vpoint.yx)+.5;
    vheight = scale * tex_at( tex_value );
    clipSpace = MODEL * PROJ * VIEW * vec4(vpoint.xy, vheight, 1.0);
//    vec3 D = vec3(clipSpace.x - vpoint.x, clipSpace.y - vpoint.y, clipSpace.z - vpoint.z);
//    vec3 D_normal = vnormal * dot(D, vnormal);
//    vec3 D_parallel = D - D_normal;
//    vec3 D_reflect = D_parallel - D_normal;
//    vec3 Reflected = vpoint + D_reflect;
//    reflect = vec3(Reflected.x, Reflected.y, Reflected.z);
//    vec3 vertexWorld = (MODEL * vec4(vpoint, 1.0)).xyz;
//    vec3 normalWorld = (MODEL * vec4(vnormal, 0.0)).xyz;
//    vec3 VertexToEyeWorld = normalize(vec3(vertexWorld.x - clipSpace.x, vertexWorld.y - clipSpace.y, vertexWorld.z - clipSpace.z));
//    ViewReflectWorld = normalize(VertexToEyeWorld - 2.0 * dot(normalWorld, VertexToEyeWorld) * normalWorld);


    gl_Position = clipSpace;
    uv = vpoint.xy;
    uv2 = vpoint.yx;

}
