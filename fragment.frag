#version 330 core  // 450, 330 core , compatibility
uniform mat4 m;
uniform sampler2D tex;
uniform samplerCube cube_texure;
uniform vec4 lightPos;

struct Light {
   vec3 position;
   vec3 intensities; //a.k.a the color of the light
} light;

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragVert;
in vec3 texcoords;

out vec4 finalColor;

void main() {
	light.position = lightPos.xyz;
	light.intensities = vec3(1.0);
    //calculate normal in world coordinates
    mat3 normalMatrix = transpose(inverse(mat3(m)));
    vec3 normal = normalize(normalMatrix * fragNormal);
    
    //calculate the location of this fragment (pixel) in world coordinates
    vec3 fragPosition = vec3(m * vec4(fragVert, 1));
    
    //calculate the vector from this pixels surface to the light source
    vec3 surfaceToLight = light.position - fragPosition;

    //calculate the cosine of the angle of incidence
    float brightness = dot(normal, surfaceToLight) / (length(surfaceToLight) * length(normal));
    brightness = clamp(brightness, 0, 1);

    //calculate final color of the pixel, based on:
    // 1. The angle of incidence: brightness
    // 2. The color/intensities of the light: light.intensities
    // 3. The texture and texture coord: texture(tex, fragTexCoord)

    vec4 surfaceColor = texture(cube_texure, texcoords);
    finalColor = vec4(brightness * light.intensities * surfaceColor.rgb, surfaceColor.a);
}
