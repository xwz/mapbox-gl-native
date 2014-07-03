#ifndef LLMR_RENDERER_RASTERBUCKET
#define LLMR_RENDERER_RASTERBUCKET

#include <llmr/renderer/bucket.hpp>
#include <llmr/style/bucket_description.hpp>
#include <llmr/geometry/raster_tile_atlas.hpp>
#include <llmr/util/raster.hpp>

namespace llmr {

class RasterShader;
class VertexBuffer;
class VertexArrayObject;

class RasterBucket : public Bucket {
public:
    RasterBucket(const std::shared_ptr<RasterTileAtlas>& rasterTileAtlas, const std::shared_ptr<Texturepool>& texturepool);
    ~RasterBucket();

    virtual void render(Painter& painter, const std::string& layer_name, const Tile::ID& id);
    virtual bool hasData() const;

    bool setImage(const std::string &data);
    void addToTextureAtlas(const Tile::ID& id);

    void drawRaster(RasterShader& shader, VertexBuffer &vertices, VertexArrayObject &array);

private:
    Raster raster;
    const std::shared_ptr<RasterTileAtlas>& rasterTileAtlas;
    Rect<uint16_t> rect;
};

}

#endif
