#pragma once
#include <Geode/Geode.hpp>
#include "../../include/utils.hpp"

using namespace geode::prelude;

inline CCMenuItemSpriteExtra* createIconButton(GameManager* gm, InfoGamemode const& info, float scale, std::function<void(CCMenuItemSpriteExtra*)> onClick, bool locked = false) {
    // Draw icon
    auto icon = SimplePlayer::create(info.id);
    icon->updatePlayerFrame(info.id, info.type);
    if (locked) {
        icon->setColor(ccc3(80, 80, 80));          // gray out
        icon->setSecondColor(ccc3(80, 80, 80));
    } else {
        icon->setColor(gm->colorForIdx(info.primaryColor));
        icon->setSecondColor(gm->colorForIdx(info.secondaryColor));
        if (info.hasGlow) {
            icon->setGlowOutline(gm->colorForIdx(info.glowColor));
        }
    }

    // Clickable area
    auto wrapper = CCNode::create();
    wrapper->setContentSize({ 30.f, 30.f });
    icon->setPosition(wrapper->getContentSize() / 2.f);
    wrapper->addChild(icon);

    if (locked) {
        auto lock = CCSprite::createWithSpriteFrameName("GJ_lock_001.png");
        lock->setScale(0.6f);
        lock->setPosition(wrapper->getContentSize() / 2.f);
        wrapper->addChild(lock, 5);
    }
    wrapper->setScale(scale);

    auto btn = CCMenuItemExt::createSpriteExtra(wrapper, onClick);
    // If locked, can't be clicked
    btn->setEnabled(!locked);
    return btn;
}