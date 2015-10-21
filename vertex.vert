#version 120
#extension GL_ARB_draw_instanced : enable
attribute vec3 pos;
attribute vec3 normal;
attribute vec4 Incolor;

uniform mat4 MVP;
uniform mat4 MV;
uniform vec3 LightPos;
// Output data ; will be interpolated for each fragment.
varying vec3 fN;
varying vec3 fL;

void main(void)
{
    fN = vec3(MV * vec4(normal,1.0));
    fL = vec3(0.0, 0.0, 3.0);
        // Pass some variables to the fragment shader
    // Apply all matrix transformations to vert
    gl_Position = MVP * vec4(pos, 1);
}


