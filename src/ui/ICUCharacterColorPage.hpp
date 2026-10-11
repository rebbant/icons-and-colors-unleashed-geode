#pragma once
#include <Geode/Geode.hpp>
#include <Geode/binding/ButtonSprite.hpp>
#include <Geode/modify/CharacterColorPage.hpp>

#include "ICUIconButton.hpp"
#include "ICUIconPicker.hpp"
#include "../../include/state.hpp"

using namespace geode::prelude;

class ICUCharacterColorPage : public CharacterColorPage {
protected:
    static constexpr float fIconScale = 0.55f;
    static constexpr float fRowGap    = 26.f;

    int m_selPlayer = 1;
    int m_editSlot = 0;
    ButtonSprite* m_slotSprs[3] = {};
    IconType m_selType = IconType::Cube;
    CCSprite* m_selFrame = nullptr;
    std::vector<CCMenuItemSpriteExtra*> m_iconBtns;
    std::vector<CCMenuItemSpriteExtra*> m_swatches;
    std::vector<std::pair<IconType, CCPoint>> m_slots;

    bool customInit() {
        if (!CharacterColorPage::init()) return false;
        this->setID("ic-color-page");
        if (!m_buttonMenu) return true;

        // Set callback for each color square
        for (auto node : CCArrayExt<CCNode*>(m_buttonMenu->getChildren())) {
            auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node);
            if (!btn || btn->getChildrenCount() == 0) continue;
            if (typeinfo_cast<ColorChannelSprite*>(btn->getChildren()->objectAtIndex(0))) {
                btn->setTarget(this, menu_selector(ICUCharacterColorPage::onColorClicked));
                m_swatches.push_back(btn);
            }
        }
        
        // Color1 | Color2 | Glow buttons
        char const* slotIds[3] = { "col1-button", "col2-button", "glow-button" };
        for (int i = 0; i < 3; i++) {
            auto b = typeinfo_cast<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildByID(slotIds[i]));
            if (!b) continue;
            b->setTarget(this, menu_selector(ICUCharacterColorPage::onSlotClicked));
            if (b->getChildrenCount()) {
                m_slotSprs[i] = typeinfo_cast<ButtonSprite*>(b->getChildren()->objectAtIndex(0));
            }
        }
        updateSlotButtons();

        // Display glow checkbox
        if (m_glowToggler) {
            m_glowToggler->setTarget(
                this, 
                menu_selector(ICUCharacterColorPage::onGlowToggled)
            );
        }
        updateGlowToggle();

        // Hide vanilla icons
        std::pair<char const*, IconType> const vanilla[] = {
            {"cube-icon", IconType::Cube},   
            {"ball-icon", IconType::Ball},
            {"ufo-icon", IconType::Ufo},     
            {"wave-icon", IconType::Wave},
            {"robot-icon", IconType::Robot}, 
            {"spider-icon", IconType::Spider},
            {"swing-icon", IconType::Swing},
        };
        for (auto const& [id, type] : vanilla) {
            if (auto icon = m_mainLayer->getChildByID(id)) {
                auto world = m_mainLayer->convertToWorldSpaceAR(icon->getPosition());
                m_slots.push_back({ type, m_buttonMenu->convertToNodeSpaceAR(world) });
                icon->setVisible(false);
            }
        }
        
        // Ignore the platform mode ship (TODO!)
        auto ship = findFirstChildRecursive<SimplePlayer>(m_buttonMenu, [](auto n) {
            return typeinfo_cast<SimplePlayer*>(n) != nullptr;
        });
        if (ship) {
            if (auto shipBtn = typeinfo_cast<CCMenuItem*>(ship->getParent())) {
                m_slots.push_back({ IconType::Ship, shipBtn->getPosition() });
                shipBtn->setVisible(false);
                shipBtn->setEnabled(false);
            }
        }

        if (m_slots.size() >= 2) {
            std::sort(m_slots.begin(), m_slots.end(),
                [](auto const& a, auto const& b) { return a.second.x < b.second.x; });

            float minX = m_slots.front().second.x;
            float maxX = m_slots.back().second.x;
            float y = 0.f;
            for (auto const& s : m_slots) y += s.second.y;
            y /= m_slots.size();

            m_slots.push_back({ IconType::Jetpack, { maxX, y } });

            float step = (maxX - minX) / (m_slots.size() - 1);
            for (size_t i = 0; i < m_slots.size(); i++) {
                m_slots[i].second = CCPoint(minX + step * static_cast<float>(i), y);
            }
        }
        
        // Draw the selected sprite
        m_selFrame = CCSprite::createWithSpriteFrameName("GJ_select_001.png");
        m_selFrame->setScale(fIconScale * 1.3f);
        m_mainLayer->addChild(m_selFrame, 10);

        // Swap Player 1 with Player 2 button
        auto swapSpr = ButtonSprite::create(
            "Swap", 
            30, 
            true, 
            "bigFont.fnt", 
            "GJ_button_01.png", 
            20.f, 
            0.4f
        );
        auto swapBtn = CCMenuItemSpriteExtra::create(
            swapSpr, this, 
            menu_selector(ICUCharacterColorPage::onSwapPlayers)
        );
        // Top right of the window
        auto winSize = CCDirector::get()->getWinSize();
        CCPoint world = { winSize.width / 2 + m_width / 2 + 30.0f, winSize.height * 0.92f };
        swapBtn->setPosition(m_buttonMenu->convertToNodeSpace(world));
        m_buttonMenu->addChild(swapBtn);

        // Draw things
        buildRows();
        updateCursors();

        return true;
    }

    // Draw grid of icons
    void buildRows() {
        for (auto n : m_iconBtns) n->removeFromParent();
        m_iconBtns.clear();

        auto gm = GameManager::get();
        auto& st = CurrentState::get();
        for (auto const& [type, pos] : m_slots) {
            for (int player : { 1, 2 }) {
                auto& info = gmFor(player == 1 ? st.infoPlayerP1 : st.infoPlayerP2, type);
                auto btn = createIconButton(gm, info, fIconScale, [this, player, type](auto*) {
                    onIconSelected(player, type);
                });
                btn->setTag(player * 100 + static_cast<int>(type));
                float y = pos.y + (player == 1 ? fRowGap / 2.f : -fRowGap / 2.f);
                btn->setPosition({ pos.x, y });
                m_buttonMenu->addChild(btn);
                m_iconBtns.push_back(btn);
            }
        }

        updateSelection();
    }

    // Move selection color swatches to the color icons
    void updateCursors() {
        auto& st = CurrentState::get();
        auto const& g = selected();
        int colors[3] = { g.primaryColor, g.secondaryColor, g.glowColor };
        char const* ids[3] = { "cursor-col1", "cursor-col2", "cursor-glow" };

        for (int i = 0; i < 3; i++) {
            auto cur = typeinfo_cast<CCSprite*>(m_mainLayer->getChildByID(ids[i]));
            if (!cur) continue;
            for (auto b : m_swatches) {
                if (b->getTag() != colors[i]) continue;
                auto world = m_buttonMenu->convertToWorldSpaceAR(b->getPosition());
                cur->setPosition(m_mainLayer->convertToNodeSpaceAR(world));
                break;
            }
            cur->setColor(i == m_editSlot ? ccc3(255, 255, 255) : ccc3(50, 50, 50));
            cur->setVisible(true);
        }
    }

    /////////////////////////////
    // Callbacks
    void onColorClicked(CCObject* sender) {
        int colorId = sender->getTag();
        auto& g = selected();
        switch (m_editSlot) {
            case 0: g.primaryColor = colorId; break;
            case 1: g.secondaryColor = colorId; break;
            default: g.glowColor = colorId; g.hasGlow = true; break;
        }
        CurrentState::get().save();
        buildRows();
        updateCursors();
    }

    void onIconSelected(int player, IconType type) {
        // Tapping the selected icon again
        if (player == m_selPlayer && type == m_selType) {
            openIconPicker();
            return;
        }
        m_selPlayer = player;
        m_selType = type;
        updateSelection();
        updateGlowToggle();
        updateCursors();
    }

    void onGlowToggled(CCObject*) {
        selected().hasGlow = m_glowToggler->isToggled();
        CurrentState::get().save();
        buildRows();
    }

    void onSlotClicked(CCObject* sender) {
        m_editSlot = std::clamp(sender->getTag(), 0, 2);
        updateSlotButtons();
        updateGlowToggle();
        updateCursors();
    }

    void onSwapPlayers(CCObject*) {
        auto& st = CurrentState::get();
        std::swap(st.infoPlayerP1, st.infoPlayerP2);
        st.save();

        buildRows();
        updateGlowToggle();
        updateCursors();
    }

    /////////////////////////////
    // Helper functions
    // Set checkbox if glow is enabled for the icon, resets if not 
    void updateGlowToggle() {
        bool show = (m_editSlot == 2);
        if (m_glowToggler) {
            m_glowToggler->setVisible(show);
            m_glowToggler->setEnabled(show);
            m_glowToggler->toggle(!selected().hasGlow);
        }
        if (m_glowLabel) m_glowLabel->setVisible(show);
    }

    // Update image of color1 color2 glow buttons
    void updateSlotButtons() {
        for (int i = 0; i < 3; i++) {
            if (!m_slotSprs[i]) continue;
            m_slotSprs[i]->updateBGImage(i == m_editSlot ? "GJ_button_01.png" : "GJ_button_04.png");      
        }
    }

    // Returns selected gamemode
    InfoGamemode& selected() {
        auto& st = CurrentState::get();
        return gmFor(m_selPlayer == 1 ? st.infoPlayerP1 : st.infoPlayerP2, m_selType);
    }

    // Shows popup to select an icon
    void openIconPicker() {
        int player = m_selPlayer;
        IconType type = m_selType;
        ICUIconPicker::create(selected(), [this, player, type](int id) {
            auto& st = CurrentState::get();
            gmFor(player == 1 ? st.infoPlayerP1 : st.infoPlayerP2, type).id = id;
            st.save();
            buildRows();
            updateCursors();
        })->show();
    }

    // Updates selection sprite
    void updateSelection() {
        int tag = m_selPlayer * 100 + static_cast<int>(m_selType);
        for (auto b : m_iconBtns) {
            if (b->getTag() != tag) continue;
            auto world = m_buttonMenu->convertToWorldSpaceAR(b->getPosition());
            m_selFrame->setPosition(m_mainLayer->convertToNodeSpaceAR(world));
            m_selFrame->setVisible(true);
            return;
        }
        m_selFrame->setVisible(false);
    }

public:
    static ICUCharacterColorPage* create() {
        auto ret = new ICUCharacterColorPage();
        if (ret->customInit()) { ret->autorelease(); return ret; }
        delete ret;
        return nullptr;
    }
};