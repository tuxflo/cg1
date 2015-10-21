#version 120
uniform mat4 M;

// material settings
uniform float materialShininess;
uniform vec3 materialSpecularColor;

struct Light {
   vec3 position;
   vec3 intensities; //a.k.a the color of the light
} light;

varying vec2 fragTexCoord;
varying vec3 fragNormal;
varying vec3 fragVert;
void main(){
    light.position = vec3(5.0,5.0,5.0);
    light.intensities = vec3(1.0,1.0,1.0);
//calculate normal in world coordinates
    mat3 normalMatrix = transpose(inverse(mat3(M)));
    vec3 normal = normalize(normalMatrix * fragNormal);

    //calculate the location of this fragment (pixel) in world coordinates
    vec3 fragPosition = vec3(M * vec4(fragVert, 1));

    //calculate the vector from this pixels surface to the light source
    vec3 surfaceToLight = light.position - fragPosition;

    //calculate the cosine of the angle of incidence
    float brightness = dot(normal, surfaceToLight) / (length(surfaceToLight) * length(normal));
    brightness = clamp(brightness, 0, 1);

    //calculate final color of the pixel, based on:
    // 1. The angle of incidence: brightness
    // 2. The color/intensities of the light: light.intensities
    // 3. The texture and texture coord: texture(tex, fragTexCoord)
    vec4 surfaceColor = vec4(0.4, 0.4, 0.4, 1.0);
    gl_FragColor = vec4(brightness * light.intensities * surfaceColor.rgb, surfaceColor.a);
}
