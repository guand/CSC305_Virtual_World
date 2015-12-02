#version 330 core
out vec3 color;
in vec3 fpoint;
in vec3 fnormal_cam;
uniform float time;

in vec3 position;
uniform samplerCube skybox;

vec3 R = vec3(1,0,0);
vec3 G = vec3(0,1,0);
vec3 B = vec3(0,0,1);
vec3 K = vec3(0,0,0);

void main() {
    color = texture(skybox, position).rgb;
        
//     color = mix(R,G,fpoint[2]);
        
//    int index = int( mod(gl_PrimitiveID,6) );
//    color = COLORS[index];
}
