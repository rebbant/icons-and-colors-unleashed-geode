#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

#include "../include/state.hpp"
#include "../include/utils.hpp"

using namespace geode::prelude;

static int customIconId(PlayerObject* p, IconType type, int original) {
    if (!p->m_gameLayer) return original;

    // Default Mini Icon
    if ((type == IconType::Cube || type == IconType::Ball)
        && p->m_defaultMiniIcon && p->m_vehicleSize != 1.0f) {
        return original;
    }

    bool p2 = p == p->m_gameLayer->m_player2;
    if (!p2 && p != p->m_gameLayer->m_player1) return original;

    auto& st = CurrentState::get();
    int id = gmFor(p2 ? st.infoPlayerP2 : st.infoPlayerP1, type).id;
    return id > 0 ? id : original;
}

class $modify(PlayerObject) {
    // Update icons drawings
    void updatePlayerFrame(int id) {
        PlayerObject::updatePlayerFrame(customIconId(this, IconType::Cube, id));
        applyColors();
    }
    void updatePlayerShipFrame(int id) {
        PlayerObject::updatePlayerShipFrame(customIconId(this, IconType::Ship, id));
        applyColors();
    }
    void updatePlayerRollFrame(int id) {
        PlayerObject::updatePlayerRollFrame(customIconId(this, IconType::Ball, id));
        applyColors();
    }
    void updatePlayerBirdFrame(int id) {
        PlayerObject::updatePlayerBirdFrame(customIconId(this, IconType::Ufo, id));
        applyColors();
    }
    void updatePlayerDartFrame(int id) {
        PlayerObject::updatePlayerDartFrame(customIconId(this, IconType::Wave, id));
        applyColors();
    }
    void updatePlayerSwingFrame(int id) {
        PlayerObject::updatePlayerSwingFrame(customIconId(this, IconType::Swing, id));
        applyColors();
    }
    void updatePlayerJetpackFrame(int id) {
        PlayerObject::updatePlayerJetpackFrame(customIconId(this, IconType::Jetpack, id));
        applyColors();
    }

    // Robot and spider are different
    void createRobot(int frame)  { 
        PlayerObject::createRobot(
            customIconId(
                this, 
                IconType::Robot, 
                frame
            )
        );
    }
    void createSpider(int frame) { 
        PlayerObject::createSpider(
            customIconId(
                this, 
                IconType::Spider, 
                frame
            )
        ); 
    }
    void toggleRobotMode(bool enable, bool noEffects) {
        if (m_robotSprite) {
            int target = customIconId(this, IconType::Robot, m_robotSprite->m_iconRequestID);
            if (m_robotSprite->m_iconRequestID != target) this->createRobot(target);
        }
        PlayerObject::toggleRobotMode(enable, noEffects);
    }
    void toggleSpiderMode(bool enable, bool noEffects) {
        if (m_spiderSprite) {
            int target = customIconId(this, IconType::Spider, m_spiderSprite->m_iconRequestID);
            if (m_spiderSprite->m_iconRequestID != target) this->createSpider(target);
        }
        PlayerObject::toggleSpiderMode(enable, noEffects);
    }

    // Recolor icon while playing
    void applyColors() {
        if (!m_gameLayer) return;
        bool p2 = this == m_gameLayer->m_player2;
        if (this != m_gameLayer->m_player1 && !p2) return;

        auto gm = GameManager::get();
        auto& st = CurrentState::get();
        InfoPlayer& info = p2 ? st.infoPlayerP2 : st.infoPlayerP1;

        InfoGamemode const* g = &info.infoGmCube;
        if      (m_isShip)   g = m_isPlatformer ? &info.infoGmJetpack : &info.infoGmShip;
        else if (m_isBird)   g = &info.infoGmUFO;
        else if (m_isBall)   g = &info.infoGmBall;
        else if (m_isDart)   g = &info.infoGmWave;
        else if (m_isRobot)  g = &info.infoGmRobot;
        else if (m_isSpider) g = &info.infoGmSpider;
        else if (m_isSwing)  g = &info.infoGmSwing;

        auto c1 = gm->colorForIdx(g->primaryColor);
        auto c2 = gm->colorForIdx(g->secondaryColor);

        this->setColor(c1);
        this->setSecondColor(c2);

        // Glow management
        this->m_glowColor = gm->colorForIdx(g->glowColor);
        if (m_hasGlow != g->hasGlow) { 
            m_hasGlow = g->hasGlow;
            this->updatePlayerGlow();
        }
        this->updateGlowColor();

        if (m_ghostTrail) m_ghostTrail->m_color = c1;
        if (m_waveTrail)  m_waveTrail->setColor(m_switchWaveTrailColor ? c1 : c2);

        // Mini cube inside the ship/UFO keeps the cube's colors
        if (m_isShip || m_isBird) {
            auto const& cube = info.infoGmCube;
            if (m_iconSprite) {
                m_iconSprite->setColor(gm->colorForIdx(cube.primaryColor));
            }
            if (m_iconSpriteSecondary) {
                m_iconSpriteSecondary->setColor(gm->colorForIdx(cube.secondaryColor));
            }
            if (m_iconGlow) {
                m_iconGlow->setColor(gm->colorForIdx(cube.glowColor));
                m_iconGlow->setVisible(cube.hasGlow);
            }
        }
    }
};