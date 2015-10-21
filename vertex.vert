#version 120
#extension GL_ARB_draw_instanced : enable
attribute vec3 pos;
attribute vec3 normal;
attribute vec3 Incolor;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 camera;

// Output data ; will be interpolated for each fragment.
varying vec3 fragVert;
varying vec3 fragNormal;

void main(void)
{
        // Pass some variables to the fragment shader
    fragNormal = normal;
    fragVert = pos;

    // Apply all matrix transformations to vert
    gl_Position = camera * M * vec4(pos, 1);
}


