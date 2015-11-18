#version 330 core
in vec3 vpoint;
const vec3 COLORS[1] = vec3[](
    vec3(0.0,0.0,0.0));
out vec3 fcolor;

void main() {
    // gl_Position = S(.5) * R(10) * vec4(vpoint, 1.0);
    gl_Position = vec4(vpoint, 1.0);
    fcolor = COLORS[gl_VertexID];
}
