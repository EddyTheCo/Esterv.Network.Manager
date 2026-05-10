#version 440
#define PI     3.14159265
layout(location = 0) in vec2 qt_TexCoord0;
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;
    float qt_Opacity;
    float strength;
    vec2 pixelStep;
    vec4 fcolor;
};
layout(binding = 1) uniform sampler2D src;

float sdPie( in vec2 p, in vec2 c, in float r )
{
    p.x = abs(p.x);
    float l = length(p) - r;
    float m = length(p-c*clamp(dot(p,c),0.0,r));
    return max(l,m*sign(c.y*p.x-c.x*p.y));
}

void main( void)
{
    vec2 st=vec2(qt_TexCoord0.x,1.0-qt_TexCoord0.y);

    float b = 0.004;
    vec2 center = vec2(0.5, 0.1);
    vec2 dir = vec2(0.7071, 0.7071);
    float radius = 0.5;
    float d =0.0;
    float shrink = 0.95;

    float d_outer = sdPie(st - center, dir, radius);

    float pie_outer = 1.0 - smoothstep(0.0, b, d_outer);

    float d_inner = sdPie((st - center) / shrink-vec2(0.0,shrink*radius*0.02), dir, radius);
    float pie_inner = 1.0 - smoothstep(0.0, b, d_inner);
    float pie_border = clamp(pie_outer - pie_inner, 0.0, 1.0);


    float d_fill = sdPie(st - center, dir, strength*radius);

    float pie_fill = 1.0 - smoothstep(0.0, b, d_fill);
    float wifi = clamp(pie_border + pie_fill, 0.0, 1.0);
    d += wifi;
    vec4 bcolor=texture(src, st).rgba;


    fragColor=mix(bcolor, fcolor, d);

}
