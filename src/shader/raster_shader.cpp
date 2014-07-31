#include <mbgl/shader/raster_shader.hpp>
#include <mbgl/shader/shaders.hpp>
#include <mbgl/platform/gl.hpp>

#include <cstdio>

using namespace mbgl;

RasterShader::RasterShader()
    : Shader(
         shaders[RASTER_SHADER].vertex,
         shaders[RASTER_SHADER].fragment
         ) {
    if (!valid) {
#if defined(DEBUG)
        fprintf(stderr, "invalid raster shader\n");
#endif
        return;
    }

    a_pos = glGetAttribLocation(program, "a_pos");

    u_matrix = glGetUniformLocation(program, "u_matrix");
    u_image = glGetUniformLocation(program, "u_image");
//    u_opacity = glGetUniformLocation(program, "u_opacity");
//    u_buffer = glGetUniformLocation(program, "u_buffer");
//    u_dimension = glGetUniformLocation(program, "u_dimension");
    u_texture_size = glGetUniformLocation(program, "u_texture_size");
    u_tile_tl = glGetUniformLocation(program, "u_tile_tl");
    u_tile_br = glGetUniformLocation(program, "u_tile_br");

    // fprintf(stderr, "RasterShader:\n");
    // fprintf(stderr, "    - u_matrix: %d\n", u_matrix);
    // fprintf(stderr, "    - u_image: %d\n", u_image);
    // fprintf(stderr, "    - u_opacity: %f\n", u_opacity);
    // fprintf(stderr, "    - u_buffer: %f\n", u_buffer);
}

void RasterShader::bind(char *offset) {
    glEnableVertexAttribArray(a_pos);
    glVertexAttribPointer(a_pos, 2, GL_SHORT, false, 0, offset);
}

void RasterShader::setImage(int32_t new_image) {
    if (image != new_image) {
        glUniform1i(u_image, new_image);
        image = new_image;
    }
}

//void RasterShader::setOpacity(float new_opacity) {
//    if (opacity != new_opacity) {
//        glUniform1f(u_opacity, new_opacity);
//        opacity = new_opacity;
//    }
//}
//
//void RasterShader::setBuffer(float new_buffer) {
//    if (buffer != new_buffer) {
//        glUniform1f(u_buffer, new_buffer);
//        buffer = new_buffer;
//    }
//}
//
//void RasterShader::setDimension(const std::array<float, 2>& new_dimension) {
//    if (dimension != new_dimension) {
//        glUniform2fv(u_dimension, 1, new_dimension.data());
//        dimension = new_dimension;
//    }
//}

void RasterShader::setTextureSize(const std::array<float, 2>& new_texture_size) {
    if (texture_size != new_texture_size) {
        glUniform2fv(u_texture_size, 1, new_texture_size.data());
        texture_size = new_texture_size;
    }
}

void RasterShader::setTileTopLeft(const std::array<float, 2>& new_tile_tl) {
    if (tile_tl != new_tile_tl) {
        glUniform2fv(u_tile_tl, 1, new_tile_tl.data());
        tile_tl = new_tile_tl;
    }
}

void RasterShader::setTileBottomRight(const std::array<float, 2>& new_tile_br) {
    if (tile_br != new_tile_br) {
        glUniform2fv(u_tile_br, 1, new_tile_br.data());
        tile_br = new_tile_br;
    }
}
