#version 330 core
out vec3 color;
const vec3 COLORS[2] = vec3[](
    vec3(1.0,0.0,0.0),
    vec3(0.0,1.0,0.0));
in vec3 fcolor; ///< passed by vshader

void main() {
    color = fcolor; /// fcolor is interpolated!
}


