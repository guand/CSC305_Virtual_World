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
vec2 texCoord = .5*uv+.5;
float tex_at(vec2 uv){ return texture(_tex,uv).r; }

void main() {

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

//  vec3 snow_color = texture(snow_tex, uv).rgb;
//  vec3 rock_color = texture(rock_tex, uv).rgb;
//  vec3 grass_color = texture(grass_tex, uv).rgb;
//  vec3 sand_color = texture(sand_tex, uv).rgb;
//  vec3 water_color = texture(water_tex, tex_uv).rgb;


  vec3 snow_color = texture(snow_tex, uv).rgb;
  vec3 rock_color = texture(rock_tex, uv).rgb;
  vec3 grass_color = texture(grass_tex, uv).rgb;
  vec3 sand_color = texture(sand_tex, uv).rgb;
  vec3 water_color = texture(water_tex, tex_uv).rgb;
  vec3 tex_color = texture(_tex, texCoord).rgb;
  vec3 base_color = mix(Y, G, vheight/scale);

  vec3 L = vec3(0,1,1);
//  float slope = dot();
  vec3 N1 = normalize(vec3(dX, 1.0f / normalStrength, dY));

  vec3 N2 = normalize(fnormal_cam);
  vec3 N = cross(N1, N2) * .5 + .5;
  float slope = dot(N1 , vec3(0, 1, 0));
  float PI = 3.1415926535897932384626433832795;
  float angle = acos(slope)*(180/PI);

  float lamb = dot( N, normalize(L) );
  color = base_color;
  if(vheight > .45) {
      if(angle <= 40){
        color = mix(rock_color, snow_color, pow(0.5, ((angle - 0.0) / (40.0 - 0.0))));
      } else {
        color = rock_color;
      }
  } else if (vheight <= .45 && vheight > .35) {
      color = rock_color;
  } else if (vheight <= 35 && vheight > .06) {
      if(angle <= 40) {
        color = mix(grass_color, rock_color, (angle - 0.0) / (40.0 - 0.0));
      } else {
        color = rock_color;
      }
  } else if (vheight <= .06 && vheight > .02) {
      color = mix(sand_color, grass_color, (vheight - 0.03)/(0.06 - 0.03));
  } else {
      color = mix(water_color, sand_color, (vheight - 0.0)/(0.02 - 0.0));
  }

//  if (vheight == 0.0) {
//      color = mix(water_color, sand_color, vheight*4);
//  } else if(angle < .1 && vheight < .09 ) {
//      color = sand_color*lamb;
//  }
//  else if(angle < 1.2 && vheight > .26) {
//      color = snow_color;
//  } else if(angle < .3 && vheight < .12 ) {
//      color = grass_color*lamb;
//  } else if(angle < .7 && vheight < .2 ) {
//      color = grass_color*lamb;
//  } else if(angle < .9 && vheight <= .26) {
//      color = grass_color*lamb;
//  } else {
//      color = rock_color*lamb;
//  }
}
