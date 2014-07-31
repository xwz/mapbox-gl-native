uniform sampler2D u_image;

uniform vec2 u_texture_size;
uniform vec2 u_tile_tl;
uniform vec2 u_tile_br;
//uniform float u_opacity;

varying vec2 v_pos;

void main() {
    gl_FragColor = texture2D(u_image, v_pos); // * u_opacity;

//    vec2 pos = (v_tex + (gl_PointCoord - 0.5) * u_size) / u_dimension;
//    gl_FragColor = u_color * texture2D(u_image, pos);

//    vec2 tilecoord = mod(v_pos / u_texture_size, 1.0);
//    vec2 pos = mix(u_tile_tl, u_tile_br, tilecoord);
//    vec4 color = texture2D(u_image, pos);
//
//    gl_FragColor = color; // * (1.0 - color.a);
}
