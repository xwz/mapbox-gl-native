#ifndef MBGL_TEXT_GLYPH_PBF
#define MBGL_TEXT_GLYPH_PBF

#include <mbgl/text/glyph.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <functional>
#include <string>
#include <atomic>
#include <mutex>

namespace mbgl {

class Environment;
class FontStack;
class Request;

class GlyphPBF : util::noncopyable {
public:
    using GlyphLoadedCallback = std::function<void(GlyphPBF*)>;

    GlyphPBF(const std::string &glyphURL,
             const std::string &fontStack,
             GlyphRange glyphRange,
             Environment &env,
             const GlyphLoadedCallback& callback);
    ~GlyphPBF();

    void parse(FontStack &stack);
    bool isParsed() const;

private:
    std::string data;
    std::atomic<bool> parsed;

    Environment& env;
    Request* req = nullptr;

    mutable std::mutex mtx;
};

}

#endif
