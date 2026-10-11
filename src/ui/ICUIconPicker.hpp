#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>

#include "ICUIconButton.hpp"

using namespace geode::prelude;

class ICUIconPicker : public Popup {
protected:
    static constexpr int numCols = 10;
    static constexpr int numRows = 4;
    static constexpr int numPerPage = numCols * numRows;
    static constexpr float numGap = 36.0f;

    InfoGamemode m_base;
    std::function<void(int)> m_onPick;
    CCMenu* m_grid = nullptr;
    CCSprite* m_frame = nullptr;
    CCLabelBMFont* m_pageLabel = nullptr;
    int m_page = 0, m_pages = 1;

    bool init(InfoGamemode const& base, std::function<void(int)> onPick) {
        if (!Popup::init(420.f, 260.f)) return false;
        m_base = base;
        m_onPick = std::move(onPick);
        this->setTitle(fmt::format("Choose {}", iconTypeString(base.type)));

        // Total pages
        int total = GameManager::get()->countForType(base.type);
        m_pages = std::max(1, (total + numPerPage - 1) / numPerPage);
        //  Page to open is the one with selected icon
        m_page = (std::max(base.id, 1) - 1) / numPerPage;

        // Create grid
        m_grid = CCMenu::create();
        m_grid->setContentSize({ numCols * numGap, numRows * numGap });
        m_grid->ignoreAnchorPointForPosition(false);
        m_grid->setAnchorPoint({ 0.5f, 0.5f });
        m_mainLayer->addChildAtPosition(m_grid, Anchor::Center, { 0.f, 5.f });

        m_frame = CCSprite::createWithSpriteFrameName("GJ_select_001.png");
        m_mainLayer->addChild(m_frame, 10);

        // Prev / next page arrows
        auto prevSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
        auto nextSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
        nextSpr->setFlipX(true);
        auto prev = CCMenuItemExt::createSpriteExtra(prevSpr, [this](auto*) { goTo(m_page - 1); });
        auto next = CCMenuItemExt::createSpriteExtra(nextSpr, [this](auto*) { goTo(m_page + 1); });
        m_buttonMenu->addChildAtPosition(prev, Anchor::Left, { -24.f, 0.f });
        m_buttonMenu->addChildAtPosition(next, Anchor::Right, { 24.f, 0.f });

        // Page label
        m_pageLabel = CCLabelBMFont::create("", "goldFont.fnt");
        m_pageLabel->setScale(0.5f);
        m_mainLayer->addChildAtPosition(m_pageLabel, Anchor::Bottom, { 0.f, 14.f });

        buildPage();
        return true;
    }

    // Jump to page
    void goTo(int page) {
        m_page = (page + m_pages) % m_pages;
        buildPage();
    }

    // Draw icons 
    void buildPage() {
        m_grid->removeAllChildren();
        m_frame->setVisible(false);

        auto gm = GameManager::get();
        int total = gm->countForType(m_base.type);
        float h = m_grid->getContentHeight();

        for (int n = 0; n < numPerPage; n++) {
            int id = m_page * numPerPage + n + 1;
            if (id > total) break;

            // Create icon
            InfoGamemode info = m_base;
            info.id = id;
            bool unlocked = isIconAvailable(id, m_base.type);
            auto btn = createIconButton(gm, info, 0.85f, [this, id](auto*) { onPick(id); }, !unlocked);

            btn->setPosition({ numGap * (n % numCols + 0.5f), h - numGap * (n / numCols + 0.5f) });
            m_grid->addChild(btn);

            // Highlight current icon
            if (id == m_base.id) {
                m_frame->setPosition(m_mainLayer->convertToNodeSpace(m_grid->convertToWorldSpace(btn->getPosition())));
                m_frame->setVisible(true);
            }
        }
        m_pageLabel->setString(fmt::format("Page {}/{}", m_page + 1, m_pages).c_str());
    }

    // Callback when icon clicked
    void onPick(int id) {
        if (!isIconAvailable(id, m_base.type)) return;
        if (m_onPick) m_onPick(id);
        this->onClose(nullptr);
    }

public:
    static ICUIconPicker* create(InfoGamemode const& base, std::function<void(int)> onPick) {
        auto ret = new ICUIconPicker();
        if (ret->init(base, std::move(onPick))) { ret->autorelease(); return ret; }
        delete ret;
        return nullptr;
    }
};