#ifndef MBGL_MAP_VIEW
#define MBGL_MAP_VIEW

#include <mbgl/util/time.hpp>

namespace mbgl {

class Map;

enum MapChange : uint8_t {
    MapChangeRegionWillChange = 0,
    MapChangeRegionDidChange = 1,
    MapChangeWillStartLoadingMap = 2,
    MapChangeDidFinishLoadingMap = 3,
    MapChangeDidFailLoadingMap = 4,
    MapChangeWillStartRenderingMap = 5,
    MapChangeDidFinishRenderingMap = 6,
};

class View {
public:
    virtual void initialize(Map *map) {
        this->map = map;
    }

    // Called from the render (=GL) thread. Signals that the context should
    // swap the front and the back buffer.
    virtual void swap() = 0;

    // Called from the render thread. Makes the GL context active in the current
    // thread. This is typically just called once at the beginning of the
    // renderer setup since the render thread doesn't switch the contexts.
    virtual void make_active() = 0;

    // Returns the base framebuffer object, if any, and 0 if using the system
    // provided framebuffer.
    virtual unsigned int root_fbo() {
        return 0;
    }

    // Notifies a watcher of map x/y/scale/rotation changes.
    // Must only be called from the same thread that caused the change.
    // Must not be called from the render thread.
    virtual void notify_map_change(MapChange change, timestamp delay = 0, void *context = nullptr) = 0;

protected:
    mbgl::Map *map = nullptr;
};
}

#endif
