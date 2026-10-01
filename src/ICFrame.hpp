#include <Geode/Geode.hpp>
using namespace geode::prelude;

class ICFrame : public CCScale9Sprite {
protected:
    bool init(float width, float height) {
        if (!CCScale9Sprite::initWithFile("GJ_square01.png")) {
            return false;
        }

        this->setContentSize({ width, height });
        this->setID("ic-frame");

        // Close button
        auto closeSprite = CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
        auto closeBtn = CCMenuItemSpriteExtra::create(
            closeSprite,
            this,
            menu_selector(ICFrame::onClose)
        );
        closeBtn->setID("close-btn");
        closeBtn->setPosition({ 6, height - 6 });

        auto menu = CCMenu::create();
        menu->setPosition({ 0, 0 });
        menu->addChild(closeBtn);
        this->addChild(menu);

        return true;
    }

    void onClose(CCObject*) {
        this->removeFromParent();
    }

public:
    static ICFrame* create(float width, float height) {
        auto ret = new ICFrame();
        if (ret->init(width, height)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};