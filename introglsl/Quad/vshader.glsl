#version 330 core
in vec3 vpoint;
in vec2 vtexcoord;
out vec2 uv;
uniform float time;
const vec3 COLORS[1] = vec3[](
    vec3(0.0,0.0,0.0));
out vec3 fcolor;

mat4 S(float s){
    return mat4(mat3(s));
}

void main() {
    // gl_Position = S(.5) * R(10) * vec4(vpoint, 1.0);
    gl_Position = vec4(vpoint, 1.0);
    uv = vtexcoord;
    fcolor = COLORS[gl_VertexID];
}
