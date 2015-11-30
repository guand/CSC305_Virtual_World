#version 330 core
out vec3 color;
in vec3 fpoint;
in vec3 fnormal_cam;
in float vheight; ///< for debug coloring
in float scale; ///< to normalize color range
in vec2 uv;

uniform sampler2D grass_tex;
uniform sampler2D rock_tex;
uniform sampler2D sand_tex;
uniform sampler2D snow_tex;
uniform sampler2D water_tex;


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
//  float fscale_rock_snow = (vheight /0.25f);
//  float fscale_sand_tree = (vheight /0.05f);
//  float fscale_tree_rock = (vheight /0.05f);
  const float range0 = 0.1f;
  const float range1 = 0.11f;
  const float range2 = 0.23f;
  vec3 base_color;
  if(vheight <= 0.0)
  {
    base_color = texture(water_tex, uv).rgb;
  } else if(vheight > 0.0 && vheight <= range0)
  {
    vec3 sand_color = texture(sand_tex, uv).rgb;
    vec3 grass_color = texture(grass_tex, uv).rgb;
    base_color = sand_color;
  } else if(vheight > range0 && vheight <= range1)
  {
      vec3 grass_color = texture(grass_tex, uv).rgb;
      vec3 rock_color = texture(rock_tex, uv).rgb;
      vec3 sand_color = texture(sand_tex, uv).rgb;
      base_color = grass_color;
  } else if(vheight > range1 && vheight <= range2) {
      vec3 rock_color = texture(rock_tex, uv).rgb;
       base_color = rock_color;
  }else {
      vec3 rock_color = texture(rock_tex, uv).rgb;
      vec3 snow_color = texture(snow_tex, uv).rgb;
      base_color = snow_color;
  }
  vec3 N = normalize(fnormal_cam);
  vec3 L = vec3(0,0,1);
  float lamb = dot( N, L );
  color = base_color * lamb;
//    color = base_color;
}
