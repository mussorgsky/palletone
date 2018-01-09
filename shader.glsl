#version 300 es
precision mediump float;

uniform vec2 resolution;
uniform float colors;
uniform sampler2D texture;
uniform sampler2D palette;

out vec4 color;

void main() {
    vec2 UV = gl_FragCoord.xy / resolution;
    UV.y = (UV.y - 0.5) * -1. + 0.5;
    int i = int(texture2D(texture, UV).r * colors);
    int h = int(sqrt(colors));
    vec2 pUV = vec2(float(i % h), float(i / h)) / vec2(h);
	color = vec4(texture2D(palette, pUV));
}