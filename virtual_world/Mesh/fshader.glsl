#version 330 core
out vec3 color;
in vec3 fpoint;
in vec3 fnormal_cam;
in float vheight; ///< for debug coloring
in float scale; ///< to normalize color range
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
float normalStrength = 10.0;
void main() {
  const float range0 = 0.1f;
  const float range1 = 0.11f;
  const float range2 = 0.23f;
  vec2 texCoord = .5*uv+.5;
  float tl = scale * texture(_tex, texCoord + texelSize * vec2(-1, -1)).x;
  float l = scale * texture(_tex, texCoord + texelSize * vec2(-1, 0)).x;
  float bl = scale * texture(_tex, texCoord + texelSize * vec2(-1, 1)).x;
  float t = scale * texture(_tex, texCoord + texelSize * vec2(0, -1)).x;
  float b = scale * texture(_tex, texCoord + texelSize * vec2(0, 1)).x;
  float tr = scale * texture(_tex, texCoord + texelSize * vec2(1, -1)).x;
  float r = scale * texture(_tex, texCoord + texelSize * vec2(1, 0)).x;
  float br = scale * texture(_tex, texCoord + texelSize * vec2(1, 1)).x;
  float dX = tr + 2*r + br - tl - 2*l - bl;
  float dY = bl + 2*b + br - tl - 2*t - tr;
  vec3 N = normalize(vec3(dX, 1.0f / normalStrength, dY));
  vec3 snow_color = texture(snow_tex, uv).rgb;
  vec3 rock_color = texture(rock_tex, uv).rgb;
  vec3 grass_color = texture(grass_tex, uv).rgb;
  vec3 sand_color = texture(sand_tex, uv).rgb;
  vec3 water_color = texture(water_tex, uv).rgb;
  vec3 tex_color = texture(_tex, texCoord).rgb;
  vec3 base_color = mix(Y, G, vheight/scale);
//  vec3 N = normalize(fnormal_cam);
  vec3 L = vec3(0,0,1);
//  float slope = dot();

  float lamb = dot( N, L );
  color = base_color * lamb;
}
