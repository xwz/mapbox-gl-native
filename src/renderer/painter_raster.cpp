#include <mbgl/renderer/painter.hpp>
#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/geometry/raster_tile_atlas.hpp>
#include <mbgl/style/style_layer.hpp>
#include <mbgl/map/map.hpp>

using namespace mbgl;

void Painter::renderRaster(RasterBucket& bucket, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id) {
    if (pass == Translucent) return;

    const RasterProperties &properties = layer_desc->getProperties<RasterProperties>();

    depthMask(false);

    RasterTileAtlas &rasterTileAtlas = *map.getRasterTileAtlas();

    Rect<uint16_t> tilePos = rasterTileAtlas.addTile("test", id.to_uint64(), bucket.getImage());

    useProgram(rasterShader->program);
    rasterShader->setMatrix(matrix);
//    rasterShader->setBuffer(0);
//    rasterShader->setOpacity(1);
    // rasterShader->setOpacity(properties.opacity * tile_data->raster->opacity);
//    rasterShader->setDimension({{
    rasterShader->setTextureSize({{
        rasterTileAtlas.getWidth(),
        rasterTileAtlas.getHeight(),
    }});

    rasterShader->setTileTopLeft({{
        float(tilePos.x), // / rasterTileAtlas.getWidth(),
        float(tilePos.y), // / rasterTileAtlas.getHeight()
    }});
    rasterShader->setTileBottomRight({{
        float(tilePos.x + tilePos.w), // / rasterTileAtlas.getWidth(),
        float(tilePos.y + tilePos.h), // / rasterTileAtlas.getHeight()
    }});

    rasterTileAtlas.bind(tilePos);

    glDepthRange(strata + strata_epsilon, 1.0f);
    bucket.drawRaster(*rasterShader, tileStencilBuffer, coveringRasterArray);

    depthMask(true);
}
