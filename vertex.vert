#version 120
#extension GL_ARB_draw_instanced : enable
attribute vec3 pos;
attribute vec3 normal;
attribute vec3 Incolor;

uniform mat4 MVP;
uniform vec3 ambientLight;
varying vec3 color;

void main(void)
{
    color = normal;
    gl_Position =MVP * vec4(pos.x, pos.y, pos.z, 1.0);
}


