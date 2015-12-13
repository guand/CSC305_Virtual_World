#version 330 core
out vec3 color;
in vec2 uv;
in vec3 reflect;
uniform sampler2D tex;
uniform sampler2D tex_mirror;
uniform sampler2D tex_refract;
in vec4 clipSpace;

void main() {
   /// TODO: 1. Query window_width/height using the textureSize function on tex_mirror
   /// Use gl_FragCoord to build a new [_u,_v] coordinate to query the framebuffer
   /// This will give you the screen pixel coordinate of current fragment.
   /// NOTE: Make sure you normalize gl_FragCoord by window_width/height
   /// You will have to flip the "v" coordinate as framebuffer is upside/down
   /// TODO: 2. Mix the texture(tex,uv).rgb with the value you fetch by
   /// texture(tex_mirror,vec2(_u,_v)).rgb
    vec2 ndc = (clipSpace.xy/clipSpace.w) *.5 + .5;
    vec2 reflection = vec2(ndc.x, ndc.y);
    color = texture(tex_mirror,reflection).rgb;
    float window_width = textureSize(tex_mirror, 0).x;
    float window_height = textureSize(tex_mirror, 0).y;
    float ScreenU =     gl_FragCoord.x/1024;
    float ScreenV = 1 - gl_FragCoord.y/768;
    vec2 new_pos = vec2(ScreenU, ScreenV);
    vec3 color_from_texture = vec3(0.53, 0.80, 0.98);
    vec3 color_from_mirror = texture(tex_mirror,ndc).rgb;
    vec3 color_from_refract = texture(tex_refract,ndc).rgb;
    color = mix(color_from_mirror, color_from_refract, vec3(.5));

}
