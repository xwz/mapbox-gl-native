#include <mbgl/map/transform_state.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/box.hpp>

using namespace mbgl;

const double R2D = 180.0 / M_PI;

#pragma mark - Matrix

void TransformState::matrixFor(mat4& matrix, const Tile::ID& id) const {
    const double tile_scale = std::pow(2, id.z);
    const double tile_size = scale * util::tileSize / tile_scale;

    matrix::identity(matrix);

    matrix::translate(matrix, matrix, 0.5f * (float)width, 0.5f * (float)height, 0);
    matrix::rotate_z(matrix, matrix, angle);
    matrix::translate(matrix, matrix, -0.5f * (float)width, -0.5f * (float)height, 0);

    matrix::translate(matrix, matrix, pixel_x() + id.x * tile_size, pixel_y() + id.y * tile_size, 0);

    // TODO: Get rid of the 8 (scaling from 4096 to tile size);
    float factor = scale / tile_scale / (4096.0f / util::tileSize);
    matrix::scale(matrix, matrix, factor, factor, 1);
}

box TransformState::cornersToBox(uint32_t z) const {
    const double ref_scale = std::pow(2, z);

    const double angle_sin = std::sin(-angle);
    const double angle_cos = std::cos(-angle);

    const double w_2 = width / 2;
    const double h_2 = height / 2;
    const double ss_0 = scale * util::tileSize;
    const double ss_1 = ref_scale / ss_0;
    const double ss_2 = ss_0 / 2.0;

    // Calculate the corners of the map view. The resulting coordinates will be
    // in fractional tile coordinates.
    box b;

    b.tl.x = ((-w_2) * angle_cos - (-h_2) * angle_sin + ss_2 - x) * ss_1;
    b.tl.y = ((-w_2) * angle_sin + (-h_2) * angle_cos + ss_2 - y) * ss_1;

    b.tr.x = ((+w_2) * angle_cos - (-h_2) * angle_sin + ss_2 - x) * ss_1;
    b.tr.y = ((+w_2) * angle_sin + (-h_2) * angle_cos + ss_2 - y) * ss_1;

    b.bl.x = ((-w_2) * angle_cos - (+h_2) * angle_sin + ss_2 - x) * ss_1;
    b.bl.y = ((-w_2) * angle_sin + (+h_2) * angle_cos + ss_2 - y) * ss_1;

    b.br.x = ((+w_2) * angle_cos - (+h_2) * angle_sin + ss_2 - x) * ss_1;
    b.br.y = ((+w_2) * angle_sin + (+h_2) * angle_cos + ss_2 - y) * ss_1;

    b.center.x = (ss_2 - x) * ss_1;
    b.center.y = (ss_2 - y) * ss_1;

    return b;
}


#pragma mark - Dimensions

bool TransformState::hasSize() const {
    return width && height;
}

uint16_t TransformState::getWidth() const {
    return width;
}

uint16_t TransformState::getHeight() const {
    return height;
}

uint16_t TransformState::getFramebufferWidth() const {
    return framebuffer[0];
}

uint16_t TransformState::getFramebufferHeight() const {
    return framebuffer[1];
}

const std::array<uint16_t, 2> TransformState::getFramebufferDimensions() const {
    return framebuffer;
}

float TransformState::getPixelRatio() const {
    return pixelRatio;
}

float TransformState::worldSize() const {
    return scale * util::tileSize;
}

float TransformState::lngX(float lon) const {
    return (180 + lon) * worldSize() / 360;
}

float TransformState::latY(float lat) const {
    float lat_y = 180 / M_PI * std::log(std::tan(M_PI / 4 + lat * M_PI / 360));
    return (180 - lat_y) * worldSize() / 360;
}

std::array<float, 2> TransformState::locationCoordinate(float lon, float lat) const {
    float k = std::pow(2, getIntegerZoom()) / worldSize();
    return {{
        lngX(lon) * k,
        latY(lat) * k
    }};
}

void TransformState::getLonLat(double &lon, double &lat) const {
    const double s = scale * util::tileSize;
    const double Bc = s / 360;
    const double Cc = s / (2 * M_PI);

    lon = -x / Bc;
    lat = R2D * (2 * std::atan(std::exp(y / Cc)) - 0.5 * M_PI);
}


#pragma mark - Zoom

float TransformState::getNormalizedZoom() const {
    return std::log(scale * util::tileSize / 512.0f) / M_LN2;
}

double TransformState::getZoom() const {
    return std::log(scale) / M_LN2;
}

int32_t TransformState::getIntegerZoom() const {
    return std::floor(getZoom());
}

double TransformState::getZoomFraction() const {
    return getZoom() - getIntegerZoom();
}

double TransformState::getScale() const {
    return scale;
}


#pragma mark - Rotation

float TransformState::getAngle() const {
    return angle;
}


#pragma mark - Changing

bool TransformState::isChanging() const {
    return rotating || scaling || panning;
}


#pragma mark - (private helper functions)


double TransformState::pixel_x() const {
    const double center = (width - scale * util::tileSize) / 2;
    return center + x;
}

double TransformState::pixel_y() const {
    const double center = (height - scale * util::tileSize) / 2;
    return center + y;
}