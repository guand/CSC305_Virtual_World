#version 330 core
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJ;
in vec3 vpoint;
in vec3 vnormal;
in vec2 vtexcoord;
out vec2 uv;
out vec3 reflect;
out vec4 clipSpace;

void main() {
    clipSpace = MODEL * PROJ * VIEW * vec4(vpoint, 1.0);
    vec3 D = vec3(clipSpace.x - vpoint.x, clipSpace.y - vpoint.y, clipSpace.z - vpoint.z);
    vec3 D_normal = vnormal * dot(D, vnormal);
    vec3 D_parallel = D - D_normal;
    vec3 D_reflect = D_parallel - D_normal;
    vec3 Reflected = vpoint + D_reflect;
    reflect = vec3(Reflected.x, Reflected.y, Reflected.z);
//    vec3 vertexWorld = (MODEL * vec4(vpoint, 1.0)).xyz;
//    vec3 normalWorld = (MODEL * vec4(vnormal, 0.0)).xyz;
//    vec3 VertexToEyeWorld = normalize(vec3(vertexWorld.x - clipSpace.x, vertexWorld.y - clipSpace.y, vertexWorld.z - clipSpace.z));
//    ViewReflectWorld = normalize(VertexToEyeWorld - 2.0 * dot(normalWorld, VertexToEyeWorld) * normalWorld);


    gl_Position = clipSpace;
    uv = vpoint.xy;
}
