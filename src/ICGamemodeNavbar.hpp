#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ICGamemodeNavbar : public CCMenu {
protected:
    static constexpr std::array<const char*, 9> kNames = {
        "icon", "ship", "ball", "bird", "dart", "robot", "spider", "swing", "jetpack"
    };

    std::array<CCMenuItemSpriteExtra*, 9> m_buttons{};
    int m_selected = 0;
    std::function<void(int)> m_onSelect;

    bool init(CCSize frameSize, std::function<void(int)> onSelect) {
        if (!CCMenu::init()) return false;

        m_onSelect = std::move(onSelect);

        this->setContentSize(frameSize);
        this->setPosition({ 0, 0 });   // positions are relative to the frame's bottom-left
        this->setID("gamemode-menu");

        float margin = 40.f;
        float step = (frameSize.width - margin * 2) / (kNames.size() - 1);
        float y = frameSize.height - margin;

        for (size_t i = 0; i < kNames.size(); i++) {
            auto spr = CCSprite::createWithSpriteFrameName(
                fmt::format("gj_{}Btn_off_001.png", kNames[i]).c_str()
            );
            auto btn = CCMenuItemSpriteExtra::create(
                spr, this, menu_selector(ICGamemodeNavbar::onButton)
            );
            btn->setTag(static_cast<int>(i));
            btn->setID(fmt::format("gamemode-btn-{}", i));
            btn->setPosition({ margin + step * i, y });
            this->addChild(btn);

            m_buttons[i] = btn;
        }

        this->select(0);   // cube by default
        return true;
    }

    void onButton(CCObject* sender) {
        int mode = static_cast<CCNode*>(sender)->getTag();
        if (mode == m_selected) return;

        this->select(mode);
        if (m_onSelect) m_onSelect(mode);
    }

public:
    static ICGamemodeNavbar* create(CCSize frameSize, std::function<void(int)> onSelect = nullptr) {
        auto ret = new ICGamemodeNavbar();
        if (ret->init(frameSize, std::move(onSelect))) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    void select(int mode) {
        m_selected = mode;

        for (size_t i = 0; i < kNames.size(); i++) {
            auto spr = typeinfo_cast<CCSprite*>(m_buttons[i]->getNormalImage());
            if (!spr) continue;

            auto frameName = fmt::format(
                "gj_{}Btn_{}_001.png", kNames[i], static_cast<int>(i) == mode ? "on" : "off"
            );
            auto frame = CCSpriteFrameCache::get()->spriteFrameByName(frameName.c_str());
            if (frame) spr->setDisplayFrame(frame);
        }
    }

    int getSelected() const { return m_selected; }
};