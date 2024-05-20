#ifdef GL_ES
  precision mediump float;
#endif

uniform int u_fogSelector; // 1: EXP, otherwise: EXP2
uniform float u_fogDensity;
uniform vec3 u_fogColor;
varying vec4 v_color;

void main()
{
  float z = gl_FragCoord.z / gl_FragCoord.w;

  float fogFactor = 1.0;
  if (u_fogSelector == 1) fogFactor = exp(-u_fogDensity * z);
  else if (u_fogSelector == 2) fogFactor = exp2(-u_fogDensity * u_fogDensity * z * z * 1.442695);

  gl_FragColor = mix(vec4(u_fogColor, v_color.a), v_color, clamp(fogFactor, 0.0, 1.0));
}
