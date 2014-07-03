#include <llmr/renderer/raster_bucket.hpp>
#include <llmr/renderer/painter.hpp>
#include <llmr/map/tile.hpp>

using namespace llmr;

RasterBucket::RasterBucket(const std::shared_ptr<RasterTileAtlas>& rasterTileAtlas, const std::shared_ptr<Texturepool>& texturepool)
    : raster(texturepool),
      rasterTileAtlas(rasterTileAtlas),
      rect(Rect<uint16_t> { 0, 0, 0, 0 }) {
}

RasterBucket::~RasterBucket() {
//    rasterTileAtlas->removeTile()
}

void RasterBucket::render(Painter &painter, const std::string &layer_name, const Tile::ID &id) {
    painter.renderRaster(*this, layer_name, id);
}

bool RasterBucket::setImage(const std::string &data) {
    return raster.load(data);
}

void RasterBucket::addToTextureAtlas(const Tile::ID& id) {
    rect = rasterTileAtlas->addTile("test", id.to_uint64(), raster);
//    platform::log_current_thread();
}

void RasterBucket::drawRaster(RasterShader& shader, VertexBuffer &vertices, VertexArrayObject &array) {
    raster.bind(true);
//    rasterTileAtlas->bind(Rect<uint16_t> { 0, 0, 256, 256 });
    shader.setImage(0);
    array.bind(shader, vertices, BUFFER_OFFSET(0));
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.index());
}

bool RasterBucket::hasData() const {
    return raster.isLoaded();
}
