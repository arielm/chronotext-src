#ifdef GL_ES
  precision mediump float;
#endif

uniform sampler2D u_sampler;
uniform float u_fogDensity;
uniform vec3 u_fogColor;

varying vec2 v_coord;
varying vec4 v_color;

void main()
{
  float z = gl_FragCoord.z / gl_FragCoord.w;

  float fogFactor = exp2(-u_fogDensity * u_fogDensity * z * z * 1.442695); // EXP2

  vec3 rgb = mix(u_fogColor, v_color.rgb, clamp(fogFactor, 0.0, 1.0));
  float alpha = texture2D(u_sampler, v_coord).a;
  gl_FragColor = vec4(rgb, alpha * v_color.a);
}
