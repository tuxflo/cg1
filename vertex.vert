#version 120
#extension GL_ARB_draw_instanced : enable
attribute vec3 pos;
attribute vec3 Incolor;

uniform mat4 MVP;
varying vec4 color;

void main(void)
{
    color = vec4(Incolor.x, Incolor.y, Incolor.z, 1.0);
    gl_Position =MVP * vec4(pos.x, pos.y, pos.z, 1.0);
}


