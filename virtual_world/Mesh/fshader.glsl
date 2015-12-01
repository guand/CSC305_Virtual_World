 #version 330 core
out vec3 color;
in vec3 fpoint;
in vec3 fnormal_cam;
in float vheight; ///< for debug coloring
in float scale; ///< to normalize color range
in vec2 tex_uv;
in vec2 uv;

uniform sampler2D _tex;
uniform sampler2D grass_tex;
uniform sampler2D rock_tex;
uniform sampler2D sand_tex;
uniform sampler2D snow_tex;
uniform sampler2D water_tex;

vec3 Y = vec3(0,0,1);
vec3 G = vec3(0,1,0);
float textureSize = 512;
float texelSize = 1.0/textureSize;
float normalStrength = 8.0;
float terrainStrength = 8.0;
vec2 texCoord = .5*uv+.5;
vec2 texTerrain = uv;
//float tex_at(vec2 uv){ return texture(_tex,uv).r; }

void main() {
  // normal calculation
  vec3 L = vec3(0,1,1);

  float tl = texture(_tex, texCoord + texelSize * vec2(-1, -1)).x;
  float l = texture(_tex, texCoord + texelSize * vec2(-1, 0)).x;
  float bl = texture(_tex, texCoord + texelSize * vec2(-1, 1)).x;
  float t = texture(_tex, texCoord + texelSize * vec2(0, -1)).x;
  float b = texture(_tex, texCoord + texelSize * vec2(0, 1)).x;
  float tr = texture(_tex, texCoord + texelSize * vec2(1, -1)).x;
  float r = texture(_tex, texCoord + texelSize * vec2(1, 0)).x;
  float br = texture(_tex, texCoord + texelSize * vec2(1, 1)).x;
  float dX = tr + 2*r + br - tl - 2*l - bl;
  float dY = bl + 2*b + br - tl - 2*t - tr;

  float tl2 = texture(water_tex, texTerrain + texelSize * vec2(-1, -1)).x;
  float l2 = texture(water_tex, texTerrain + texelSize * vec2(-1, 0)).x;
  float bl2 = texture(water_tex, texTerrain + texelSize * vec2(-1, 1)).x;
  float t2 = texture(water_tex, texTerrain + texelSize * vec2(0, -1)).x;
  float b2 = texture(water_tex, texTerrain + texelSize * vec2(0, 1)).x;
  float tr2 = texture(water_tex, texTerrain + texelSize * vec2(1, -1)).x;
  float r2 = texture(water_tex, texTerrain + texelSize * vec2(1, 0)).x;
  float br2 = texture(water_tex, texTerrain + texelSize * vec2(1, 1)).x;
  float dX2 = tr2 + 2*r2 + br2 - tl2 - 2*l2 - bl2;
  float dY2 = bl2 + 2*b2 + br2 - tl2 - 2*t2 - tr2;
  vec3 NW = normalize(vec3(dX2, 1.0f / terrainStrength, dY2)) + .3;
  float lamb_water = max(dot( NW, normalize(L) ), 0.0);

  float tl4 = texture(rock_tex, texTerrain + texelSize * vec2(-1, -1)).x;
  float l4 = texture(rock_tex, texTerrain + texelSize * vec2(-1, 0)).x;
  float bl4 = texture(rock_tex, texTerrain + texelSize * vec2(-1, 1)).x;
  float t4 = texture(rock_tex, texTerrain + texelSize * vec2(0, -1)).x;
  float b4 = texture(rock_tex, texTerrain + texelSize * vec2(0, 1)).x;
  float tr4 = texture(rock_tex, texTerrain + texelSize * vec2(1, -1)).x;
  float r4 = texture(rock_tex, texTerrain + texelSize * vec2(1, 0)).x;
  float br4 = texture(rock_tex, texTerrain + texelSize * vec2(1, 1)).x;
  float dX4 = tr4 + 2*r4 + br4 - tl4 - 2*l4 - bl4;
  float dY4 = bl4 + 2*b4 + br4 - tl4 - 2*t4 - tr4;
  vec3 RS = normalize(vec3(dX4, 1.0f / terrainStrength, dY4)) + .3;
  float lamb_rock = max(dot( RS, normalize(L) ), 0.0);

 // textures
  vec3 snow_color = texture(snow_tex, uv).rgb;
  vec3 rock_color = texture(rock_tex, uv).rgb;
  vec3 grass_color = texture(grass_tex, uv).rgb;
  vec3 sand_color = texture(sand_tex, uv).rgb;
  vec3 water_color = texture(water_tex, tex_uv).rgb;
  vec3 tex_color = texture(_tex, texCoord).rgb;
  vec3 base_color;

  // calculations
  vec3 N1 = normalize(vec3(dX, 1.0f / normalStrength, dY));
  vec3 N2 = normalize(fnormal_cam);
  vec3 N = cross(N1, N2) * .5 + .5;
  float slope = dot(N1 , vec3(0, 1, 0));
  float PI = 3.1415926535897932384626433832795;
  float angle = acos(slope)*(180/PI);

  float lamb = max(dot( N1, normalize(L) ), 0.0);

  if(vheight > .45) {
      if(angle <= 40){
        color = mix(rock_color*lamb_rock, snow_color, pow(0.5, ((angle - 0.0) / (40.0 - 0.0))));
      } else {
        color = rock_color*lamb_rock;
      }
  } else if (vheight <= .45 && vheight > .35) {
      color = rock_color*lamb_rock;
  } else if (vheight <= 35 && vheight > .06) {
      if(angle <= 40) {
        color = mix(grass_color, rock_color*lamb_rock, (angle - 0.0) / (40.0 - 0.0));
      } else {
        color = rock_color*lamb_rock;
      }
  } else if (vheight <= .06 && vheight > .02) {

      color = mix(sand_color, grass_color, (vheight - 0.03)/(0.06 - 0.03));
  } else {


      color = mix(water_color*lamb_water, sand_color, (vheight - 0.0)/(0.02 - 0.0));
  }
  color *= lamb;
}
