#include <mbgl/util/raster.hpp>

#include <memory>
#include <cassert>
#include <cstring>

#include <mbgl/platform/platform.hpp>
#include <mbgl/platform/gl.hpp>
#include <mbgl/util/time.hpp>
#include <mbgl/util/uv.hpp>
#include <mbgl/util/std.hpp>

#include <png.h>

using namespace mbgl;

Raster::Raster() {
}

Raster::~Raster() {
}

bool Raster::isLoaded() const {
    std::lock_guard<std::mutex> lock(mtx);
    return loaded;
}

bool Raster::load(const std::string &data) {
    img = std::make_unique<util::Image>(data);
    width = img->getWidth();
    height = img->getHeight();

    std::lock_guard<std::mutex> lock(mtx);
    if (img->getData()) {
        loaded = true;
    }
    return loaded;
}

void Raster::beginFadeInTransition() {
    timestamp start = util::now();
    fade_transition = std::make_shared<util::ease_transition<double>>(opacity, 1.0, opacity, start, 250_milliseconds);
}

bool Raster::needsTransition() const {
    return fade_transition != nullptr;
}

void Raster::updateTransitions(timestamp now) {
    if (fade_transition->update(now) == util::transition::complete) {
        fade_transition = nullptr;
    }
}
