#include <mbgl/map/view.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/still_image.hpp>

namespace mbgl {

void View::initialize(Map *map_) {
    assert(map_);
    map = map_;
}

std::unique_ptr<StillImage> View::readStillImage() {
    return nullptr;
}

void View::notifyMapChange(MapChange, Duration) {
    // no-op
}


}
