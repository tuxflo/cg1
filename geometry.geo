#version 120
#extension GL_EXT_geometry_shader4 : enable

// a passthrough geometry shader for color and position
void main()
{
    // copy color
    gl_FrontColor = vec3(0.0, 0.0, 0.0);

    // copy position
    gl_Position = gl_PositionIn[i] - vec3(-5-0, 0.0, 0.0);

    // done with the vertex
    EmitVertex();
  }
}
