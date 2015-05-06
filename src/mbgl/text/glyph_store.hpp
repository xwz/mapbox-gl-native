#ifndef MBGL_TEXT_GLYPH_STORE
#define MBGL_TEXT_GLYPH_STORE

#include <mbgl/text/glyph.hpp>

#include <string>
#include <set>
#include <unordered_map>
#include <mutex>

namespace mbgl {

class Environment;
class FontStack;
class GlyphPBF;

// Manages GlyphRange PBF loading.
class GlyphStore {
public:
    class Observer {
    public:
        virtual ~Observer() = default;

        virtual void onGlyphRangeLoaded() = 0;
    };

    GlyphStore(Environment &);
    ~GlyphStore();

    // Asynchronously request for GlyphRanges and when it gets loaded, notifies the
    // observer subscribed to this object. Successive requests for the same range are
    // going to be discarded. Returns true if a request was made or false if all the
    // GlyphRanges are already available, and thus, no request is performed.
    bool requestGlyphRangesIfNeeded(const std::string &fontStack, const std::set<GlyphRange> &glyphRanges);

    FontStack* getFontStack(const std::string &fontStack);

    void setURL(const std::string &url);

    void setObserver(Observer* observer);

private:
    void emitGlyphRangeLoaded();

    FontStack* createFontStack(const std::string &fontStack);

    std::string glyphURL;
    Environment &env;

    std::unordered_map<std::string, std::map<GlyphRange, std::unique_ptr<GlyphPBF>>> ranges;
    std::mutex rangesMutex;

    std::unordered_map<std::string, std::unique_ptr<FontStack>> stacks;
    std::mutex stacksMutex;

    Observer* observer;
};

}

#endif
