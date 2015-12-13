#version 330 core
out vec3 color;
in vec2 uv;
uniform sampler2D tex;
uniform sampler2D tex_mirror;

void main() {
   /// TODO: 1. Query window_width/height using the textureSize function on tex_mirror
   /// Use gl_FragCoord to build a new [_u,_v] coordinate to query the framebuffer
   /// This will give you the screen pixel coordinate of current fragment.
   /// NOTE: Make sure you normalize gl_FragCoord by window_width/height
   /// You will have to flip the "v" coordinate as framebuffer is upside/down
   /// TODO: 2. Mix the texture(tex,uv).rgb with the value you fetch by
   /// texture(tex_mirror,vec2(_u,_v)).rgb
 
    float window_width = textureSize(tex_mirror, 0).x;
    float window_height = textureSize(tex_mirror, 0).y;
    float ScreenU =     gl_FragCoord.x/window_width;
    float ScreenV = 1 - gl_FragCoord.y/window_height;
    vec3 color_from_texture = texture(tex,uv).rgb;
    vec3 color_from_mirror = texture(tex_mirror,vec2(ScreenU,ScreenV)).rgb;
    color = mix(color_from_texture, color_from_mirror, vec3(.15));

}
