#ifdef GL_ES
precision mediump float;
#endif

uniform float u_fogDensity;
uniform vec3 u_fogColor;
varying vec4 v_color;

void main()
{
    float z = gl_FragCoord.z / gl_FragCoord.w;

    float fogFactor = exp(-u_fogDensity * z); // EXP

    gl_FragColor = mix(vec4(u_fogColor, v_color.a), v_color, clamp(fogFactor, 0.0, 1.0));
}
