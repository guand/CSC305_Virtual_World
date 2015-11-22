#version 330 core
out vec3 color;
in vec3 fpoint;
in float vheight; ///< for debug coloring
in float scale; ///< to normalize color range

vec3 Y = vec3(1,.98,.98);
vec3 G = vec3(1,1,1);
vec3 LM = vec3(.804,.522,.247);
vec3 M = vec3(.627,.322,.176);
vec3 LF = vec3(.604,.804,.196);
vec3 F = vec3(.133,.545,.133);
vec3 O = vec3(.255,.412,.882);
vec3 L = vec3(.498,1,.831);
void main() {

  float fscale = (vheight /scale) * 4;
  if(vheight > 0.2)
  {
    color = mix(M,Y, fscale/2);
  } else if (vheight > 0.1)
  {
    color = mix(LM,M, fscale);
  } else if (vheight > 0.0) {
    color = mix(LF,F, fscale);
  } else {
    color = mix(O,L, fscale);
  }
}
