#include <llmr/util/raster.hpp>

#include <memory>
#include <cassert>
#include <cstring>

#include <llmr/platform/platform.hpp>
#include <llmr/platform/gl.hpp>
#include <llmr/util/time.hpp>
#include <llmr/util/uv.hpp>
#include <llmr/util/std.hpp>

#include <png.h>

using namespace llmr;

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
    time start = util::now();
    fade_transition = std::make_shared<util::ease_transition<double>>(opacity, 1.0, opacity, start, 250_milliseconds);
}

bool Raster::needsTransition() const {
    return fade_transition != nullptr;
}

void Raster::updateTransitions(time now) {
    if (fade_transition->update(now) == util::transition::complete) {
        fade_transition = nullptr;
    }
}
