#ifndef MBGL_UTIL_RASTER
#define MBGL_UTIL_RASTER

#include <mbgl/util/transition.hpp>
#include <mbgl/util/image.hpp>

#include <string>
#include <mutex>
#include <memory>

typedef struct uv_loop_s uv_loop_t;

namespace mbgl {

class Raster : public std::enable_shared_from_this<Raster> {

public:
    Raster();
    ~Raster();

    // load image data
    bool load(const std::string &img);

    // loaded status
    bool isLoaded() const;

    // transitions
    void beginFadeInTransition();
    bool needsTransition() const;
    void updateTransitions(timestamp now);

public:
    // loaded image dimensions
    uint32_t width = 0, height = 0;

    // texture opacity
    double opacity = 0;

    // the raw pixels
    std::unique_ptr<util::Image> img;

private:
    mutable std::mutex mtx;

    // raw pixels have been loaded
    bool loaded = false;

    // fade in transition
    std::shared_ptr<util::transition> fade_transition = nullptr;
};

}

#endif
