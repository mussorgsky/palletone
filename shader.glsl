#version 300 es
precision mediump float;

uniform vec2 Resolution;
uniform float ColCount;
uniform sampler2D Texture;
uniform sampler2D Palette;

out vec4 Color;

#define PI 3.1415

void main() {
    vec2 UV = gl_FragCoord.xy / Resolution;
    UV.y = (UV.y - 0.5) * -1. + 0.5;
    
    int i = int(texture2D(Texture, UV).r * ColCount);
    int h = int(sqrt(ColCount));

    vec2 pUV = vec2(float(i % h), float(i / h)) / vec2(h);
    
	Color = vec4(texture2D(Palette, pUV));
}