#ifndef MBGL_RENDERER_RASTERBUCKET
#define MBGL_RENDERER_RASTERBUCKET

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/util/raster.hpp>

namespace mbgl {

class RasterShader;
class VertexBuffer;
class VertexArrayObject;

class RasterBucket : public Bucket {
public:
    RasterBucket();
    ~RasterBucket();

    virtual void render(Painter& painter, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID& id);
    virtual bool hasData() const;

    bool setImage(const std::string &data);
    std::shared_ptr<Raster> getImage();

    void drawRaster(RasterShader& shader, VertexBuffer &vertices, VertexArrayObject &array);

private:
    std::shared_ptr<Raster> raster;
};

}

#endif
