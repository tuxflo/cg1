#version 120

varying vec3 fN;
varying vec3 fL;
void main(){
    vec3 N = normalize(fN);
    vec3 L = normalize(fL);

    float diffuse_intensity = max(dot(N,L), 0.1);
    vec4 color = vec4(diffuse_intensity, diffuse_intensity, diffuse_intensity, 0.5);

    gl_FragColor = color;
}
