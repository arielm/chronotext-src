attribute vec4 a_position;
attribute vec4 a_color;
attribute mat4 a_matrix;

uniform mat4 u_mvp_matrix;

varying vec4 v_color;

void main() {
  v_color = a_color;

  gl_Position = u_mvp_matrix * a_matrix * a_position;
}
