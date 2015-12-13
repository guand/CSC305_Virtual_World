#version 330 core 
uniform float time;
in vec3 vpoint; ///< [-.5, +.5]^3
in vec3 vnormal;
out vec3 fpoint; ///< for debug
out vec3 fnormal_cam;

out vec3 position;
uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJ;
uniform float reverse;



void main() {
    mat3 R = mat3(1);
    float alpha = radians(45 /*degrees*/);
    R[0][0] =  cos(alpha);
    R[0][1] =  sin(alpha);
    R[1][0] = -sin(alpha);
    R[1][1] =  cos(alpha);

   gl_Position = PROJ * VIEW * vec4(vpoint, 1.0);
   position = vec3(vpoint.x, vpoint.z*reverse, vpoint.y);
}
