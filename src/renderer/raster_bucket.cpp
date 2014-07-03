#include <llmr/renderer/raster_bucket.hpp>
#include <llmr/renderer/painter.hpp>
#include <llmr/map/tile.hpp>
#include <llmr/util/rect.hpp>

using namespace llmr;

RasterBucket::RasterBucket()
    : raster() {
}

RasterBucket::~RasterBucket() {
}

void RasterBucket::render(Painter &painter, const std::string &layer_name, const Tile::ID &id) {
    painter.renderRaster(*this, layer_name, id);
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
