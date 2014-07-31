#ifndef MBGL_RENDERER_SHADER_RASTER
#define MBGL_RENDERER_SHADER_RASTER

#include <mbgl/shader/shader.hpp>

namespace mbgl {

class RasterShader : public Shader {
public:
    RasterShader();

    void bind(char *offset);

    void setImage(int32_t image);
//    void setOpacity(float opacity);
//    void setBuffer(float buffer);
    void setTextureSize(const std::array<float, 2>& texture_size);
    void setTileTopLeft(const std::array<float, 2>& tile_tl);
    void setTileBottomRight(const std::array<float, 2>& tile_br);

private:
    int32_t a_pos = -1;

    int32_t image = 0;
    int32_t u_image = -1;

//    float opacity = 0;
//    int32_t u_opacity = -1;
//
//    float buffer = 0;
//    int32_t u_buffer = -1;

    std::array<float, 2> texture_size = {{}};
    int32_t u_texture_size = -1;

    std::array<float, 2> tile_tl = {{}};
    int32_t u_tile_tl = -1;

    std::array<float, 2> tile_br = {{}};
    int32_t u_tile_br = -1;
};

}

#endif
