#include <mbgl/text/glyph_store.hpp>
#include <mbgl/text/glyph_pbf.hpp>
#include <mbgl/text/font_stack.hpp>

#include <mbgl/util/std.hpp>

namespace mbgl {

GlyphStore::GlyphStore(Environment& env_)
    : env(env_), observer(nullptr) {
}

GlyphStore::~GlyphStore() {
    observer = nullptr;
}

void GlyphStore::setURL(const std::string &url) {
    glyphURL = url;
}

bool GlyphStore::requestGlyphRangesIfNeeded(const std::string& fontStack,
                                            const std::set<GlyphRange>& glyphRanges) {
    bool requestIsNeeded = false;

    if (glyphRanges.empty()) {
        return requestIsNeeded;
    }

    auto callback = [this, fontStack](GlyphPBF* glyph) {
        glyph->parse(*createFontStack(fontStack));
        emitGlyphRangeLoaded();
    };

    std::lock_guard<std::mutex> lock(rangesMutex);
    auto& rangeSets = ranges[fontStack];

    for (const auto& range : glyphRanges) {
        const auto& rangeSets_it = rangeSets.find(range);
        if (rangeSets_it == rangeSets.end()) {
            auto glyph = util::make_unique<GlyphPBF>(glyphURL, fontStack, range, env, callback);
            rangeSets.emplace(range, std::move(glyph));
            requestIsNeeded = true;
            continue;
        }

        if (!rangeSets_it->second->isParsed()) {
            requestIsNeeded = true;
        }
    }

    return requestIsNeeded;
}

FontStack* GlyphStore::createFontStack(const std::string &fontStack) {
    std::lock_guard<std::mutex> lock(stacksMutex);

    auto stack_it = stacks.find(fontStack);
    if (stack_it == stacks.end()) {
        stack_it = stacks.emplace(fontStack, util::make_unique<FontStack>()).first;
    }

    return stack_it->second.get();
}

FontStack* GlyphStore::getFontStack(const std::string &fontStack) {
    std::lock_guard<std::mutex> lock(stacksMutex);

    const auto& stack_it = stacks.find(fontStack);
    if (stack_it == stacks.end()) {
        return nullptr;
    }

    return stack_it->second.get();
}

void GlyphStore::setObserver(Observer* observer_) {
    observer = observer_;
}

void GlyphStore::emitGlyphRangeLoaded() {
    if (observer) {
        observer->onGlyphRangeLoaded();
    }
}

}
