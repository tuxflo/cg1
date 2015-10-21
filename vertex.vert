#version 120
#extension GL_ARB_draw_instanced : enable
attribute vec3 pos;
attribute vec3 normal;
attribute vec4 Incolor;

uniform mat4 MVP;
uniform mat4 MV;
uniform vec3 LightPos;
// Output data ; will be interpolated for each fragment.
varying vec4 color;

void main(void)
{
        // Pass some variables to the fragment shader
    vec4 tmp = MV * vec4(pos, 1.0);
    vec3 modelViewVertex = vec3(MV * vec4(pos, 1.0));
    vec3 modelViewNormal = vec3(MV * vec4(normal, 0.0));
    float distance = length(LightPos - modelViewVertex);
    vec3 lightVector = normalize(LightPos -modelViewVertex);

    float diffuse = max ( dot(modelViewNormal, lightVector), 0.1);
    diffuse = diffuse * (1.0 / (1.0 + (0.25 * distance * distance)));

    color = Incolor * diffuse;

    // Apply all matrix transformations to vert
    gl_Position = MVP * vec4(pos, 1);
}


