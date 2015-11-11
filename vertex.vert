#version 330 core  // 450, 330 core , compatibility
layout(location = 0) in vec4 v_coord;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec4 v_color;
layout(location = 3) in vec2 v_UV;

uniform mat4 m, v, p;
uniform mat3 m_3x3_inv_transp;
uniform vec4 lightPos;
uniform sampler2D myTextureSampler;

out vec3 fragVert;
out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 texcoords;

uniform mat4 MVP;

void main() {
    mat4 mvp = p*v*m;

	    // Pass some variables to the fragment shader
    fragTexCoord = v_UV;
    fragNormal = v_normal;
    fragVert = v_coord.xyz;
    texcoords = v_coord.xyz;

    gl_Position = mvp * v_coord;
}
