#version 120

varying vec4 color;
//uniform sampler2D myTextureSampler;
void main(){
    gl_FragColor = color;
 //   vec4 tex = texture2D ( myTextureSampler, UV );
      //gl_FragColor = vec4(tex.r, tex.g, tex.b, 1.0);
  //  gl_FragColor = vec4(color.x * tex.r, color.y * tex.g, color.z * tex.b, color.w * tex.a);

    //gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
