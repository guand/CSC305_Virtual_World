#version 330 core
out vec3 color;
in vec3 fpoint;
in vec3 fnormal_cam;
in float vheight; ///< for debug coloring
in float scale; ///< to normalize color range
in vec2 uv;

uniform sampler2D tex_grass;

//vec3 Y = vec3(1,.98,.98);
//vec3 G = vec3(1,1,1);
//vec3 LM = vec3(.804,.522,.247);
//vec3 M = vec3(.627,.322,.176);
//vec3 LF = vec3(.604,.804,.196);
//vec3 F = vec3(.133,.545,.133);
//vec3 O = vec3(.255,.412,.882);
//vec3 L = vec3(.498,1,.831);
vec3 Y = vec3(0,0,1);
vec3 G = vec3(0,1,0);
void main() {
  float fscale = (vheight /scale);
  vec3 base_color = mix(Y,G, fscale);
  vec3 N = normalize(fnormal_cam);
  vec3 L = vec3(0,0,1);
  float lamb = dot( N, L );
  color = base_color * lamb;
}
