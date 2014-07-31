uniform mat4 u_matrix;
//uniform float u_buffer;

attribute vec2 a_pos;
//attribute vec2 a_tex;

varying vec2 v_pos;


void main() {
    v_pos = a_pos / 4096.0;
    gl_Position = u_matrix * vec4(a_pos, 0, 1);
//    float dimension = (4096.0 + 2.0 * u_buffer);
//    v_pos = (a_pos / dimension) + (u_buffer / dimension);
//    v_tex = a_tex; // * u_ratio;
}
