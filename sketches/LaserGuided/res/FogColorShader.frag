#ifdef GL_ES
  precision mediump float;
#endif

uniform float u_fogDensity;
uniform vec3 u_fogColor;
varying vec4 v_color;

void main()
{
  const float LOG2 = 1.442695;
  float z = gl_FragCoord.z / gl_FragCoord.w;
  float fogFactor = exp2(-u_fogDensity * u_fogDensity * z * z * LOG2);

  gl_FragColor = mix(vec4(u_fogColor, v_color.a), v_color, clamp(fogFactor, 0.0, 1.0));
}
