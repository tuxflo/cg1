#version 120
#extension GL_ARB_draw_instanced : enable
uniform mat4 MVP;
varying vec4 color;

void main(void)
{
    if(gl_InstanceID  > 0)
    {
        vec4 position = gl_Vertex;
        position.x += 5.0;
        color = vec4(1.0, 1.0, 0.0, 1.0);
        gl_Position = MVP * position;
    }
    else
    {
        color = vec4(0.0);
        gl_Position =MVP * gl_Vertex;
    }
}


