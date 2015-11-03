attribute vec4 v_coord;
attribute vec3 v_normal;
attribute vec4 v_color;
attribute vec2 v_UV;

uniform mat4 m, v, p;
uniform mat3 m_3x3_inv_transp;
uniform vec4 lightPos;
uniform sampler2D myTextureSampler;
varying vec4 color;
varying vec2 UV;
//struct lightSource
//{
//  vec4 position;
//  vec4 diffuse;
//  float constantAttenuation, linearAttenuation, quadraticAttenuation;
//  float spotCutoff, spotExponent;
//  vec3 spotDirection;
//};
//lightSource light0 = lightSource(
//    lightPos,
//    vec4(1.0, 1.0, 1.0, 1.0),
//    0.0, 0.5, 0.0,
//    50.0, 20.0,
//    vec3(-1.0, -0.5, -1.0)
//);
//
//struct material
//{
//  vec4 diffuse;
//};
//material mymaterial = material(v_color);
//
//void main(void)
//{
//    //texture
//    UV = v_UV;
//  mat4 mvp = p*v*m;
//  vec3 normalDirection = normalize(m_3x3_inv_transp * v_normal);
//  vec3 lightDirection;
//  float attenuation;
//
//  if (light0.position.w == 0.0) // directional light
//    {
//      attenuation = 1.0; // no attenuation
//      lightDirection = normalize(vec3(light0.position));
//    }
//  else // point or spot light (or other kind of light)
//    {
//      vec3 vertexToLightSource = vec3(light0.position - m * v_coord);
//      float distance = length(vertexToLightSource);
//      lightDirection = normalize(vertexToLightSource);
//      attenuation = 1.0 / (light0.constantAttenuation
//                           + light0.linearAttenuation * distance
//                           + light0.quadraticAttenuation * distance * distance);
//
//      if (light0.spotCutoff <= 90.0) // spotlight
//        {
//          float clampedCosine = max(0.0, dot(-lightDirection, normalize(light0.spotDirection)));
//          if (clampedCosine < cos(light0.spotCutoff * 3.14159 / 180.0)) // outside of spotlight cone
//            {
//              attenuation = 0.0;
//            }
//          else
//            {
//              attenuation = attenuation * pow(clampedCosine, light0.spotExponent);
//            }
//        }
//    }
//  vec3 diffuseReflection = attenuation
//    * vec3(light0.diffuse) * vec3(mymaterial.diffuse)
//    * max(0.0, dot(normalDirection, lightDirection));
//
//  color = vec4(diffuseReflection, 1.0);
//  gl_Position = mvp * v_coord;
//}
//

void main() {
    mat4 mvp = p*v*m;

    //calculate normal in world coordinates
    mat3 normalMatrix = m_3x3_inv_transp;
    vec3 normal = normalize(normalMatrix * v_normal);

    //calculate the location of this fragment (pixel) in world coordinates
    vec3 fragPosition = vec3(m * v_coord);

    //calculate the vector from this pixels surface to the light source
    vec3 surfaceToLight = vec3(lightPos) - fragPosition;

    //calculate the cosine of the angle of incidence
    float brightness = dot(normal, surfaceToLight) / (length(surfaceToLight) * length(normal));
    brightness = clamp(brightness, 0, 1);

    //calculate final color of the pixel, based on:
    // 1. The angle of incidence: brightness
    // 2. The color/intensities of the light: light.intensities
    // 3. The texture and texture coord: texture(tex, fragTexCoord)
    vec4 surfaceColor = texture2D(myTextureSampler, v_UV);
    vec4 finalColor = vec4(brightness * vec3(1.0) * vec3(0.5), 1.0);
    color = finalColor;
    gl_Position = mvp * v_coord;
}
