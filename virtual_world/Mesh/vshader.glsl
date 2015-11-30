#version 330 core 
uniform float time;
in vec3 vpoint; ///< [-1,+1]
in vec3 vnormal;
in vec2 vtexcoord;
out float vheight; ///< for debug coloring
out vec3 fnormal_cam;
out vec2 uv;
out vec2 tex_uv;
out float scale=0.5;

uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJ;

uniform sampler2D tex;
float tex_at(vec2 uv){ return texture(tex,uv).r; }

void main() {
    /// UNCOMMENT ONE OF THESE
//    vheight = 0;
//    vheight = scale * sin(10*vpoint.x + time);
    fnormal_cam = inverse( transpose( mat3(VIEW * MODEL) )) * vnormal;
    vheight = scale * tex_at( .5*vec2(vpoint.x, vpoint.y)+.5 );
    if(vheight < 0.0) {
        vheight = 0.0;
    } else if(vheight > 1.0) {
        vheight = 1.0;
    }
//    vheight = vpoint.z;
    gl_Position = PROJ * VIEW * MODEL * vec4(vpoint.xy, vheight, 1.0);
    uv = vpoint.xy;
    tex_uv = vtexcoord;
}
