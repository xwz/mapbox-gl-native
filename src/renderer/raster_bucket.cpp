#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/renderer/painter.hpp>

using namespace mbgl;

RasterBucket::RasterBucket()
    : raster() {
}

RasterBucket::~RasterBucket() {
}

void RasterBucket::render(Painter &painter, std::shared_ptr<StyleLayer> layer_desc, const Tile::ID &id) {
    painter.renderRaster(*this, layer_desc, id);
}

bool RasterBucket::setImage(const std::string &data) {
    return raster.load(data);
}

Raster& RasterBucket::getImage() {
    return raster;
}

void RasterBucket::drawRaster(RasterShader& shader, VertexBuffer &vertices, VertexArrayObject &array) {
    // texture bound by raster tile data
    shader.setImage(0);
    array.bind(shader, vertices, BUFFER_OFFSET(0));
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.index());
}

bool RasterBucket::hasData() const {
    return raster.isLoaded();
}
