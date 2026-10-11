#pragma once
#include <Geode/Geode.hpp>
#include "utils.hpp"

using namespace geode::prelude;

class CurrentState {
public:
    int m_defaultP1ColorP = 0;
    int m_defaultP1ColorS = 0;
    int m_defaultP1ColorG = 0;
    bool m_defaultP1HasG = 0;

    int m_defaultP2ColorP = 0;
    int m_defaultP2ColorS = 0;
    int m_defaultP2ColorG = 0;
    bool m_defaultP2HasG = 0;

    InfoPlayer infoPlayerP1;
    InfoPlayer infoPlayerP2;

    static CurrentState& get() {
        static CurrentState s;
        static bool loaded = false;
        if (!loaded) { 
            loaded = true; 
            s.load(); 
        }
        return s;
    }

    // Builds a full player with default icons/colors
    static InfoPlayer makeDefaultPlayer(int colorP, int colorS, int colorG, bool hasGlow) {
        auto gm = GameManager::get();
        auto mk = [&](IconType t) {
            InfoGamemode g;
            g.type = t;
            g.id = gm->activeIconForType(t);
            g.primaryColor = colorP;
            g.secondaryColor = colorS;
            g.glowColor = colorG;
            g.hasGlow = hasGlow;
            return g;
        };

        InfoPlayer p;
        p.infoGmCube = mk(IconType::Cube);   
        p.infoGmShip = mk(IconType::Ship);
        p.infoGmUFO = mk(IconType::Ufo);     
        p.infoGmBall = mk(IconType::Ball);
        p.infoGmWave = mk(IconType::Wave);   
        p.infoGmRobot = mk(IconType::Robot);
        p.infoGmSpider = mk(IconType::Spider); 
        p.infoGmSwing = mk(IconType::Swing);
        p.infoGmJetpack = mk(IconType::Jetpack);

        return p;
    }

    // Load from save data
    void load() {
        auto gm = GameManager::get();
        m_defaultP1ColorP = gm->getPlayerColor();
        m_defaultP1ColorS = gm->getPlayerColor2();
        m_defaultP1ColorG = gm->getPlayerGlowColor();
        m_defaultP1HasG   = gm->getPlayerGlow();

        m_defaultP2ColorP = m_defaultP1ColorS;
        m_defaultP2ColorS = m_defaultP1ColorP;
        m_defaultP2ColorG = m_defaultP1ColorP;
        m_defaultP2HasG   = gm->getPlayerGlow();

        auto mod = Mod::get();
        // the second argument is used when nothing has been saved yet
        infoPlayerP1 = mod->getSavedValue<InfoPlayer>(
            "player1", 
            makeDefaultPlayer(
                m_defaultP1ColorP, 
                m_defaultP1ColorS, 
                m_defaultP1ColorG, 
                m_defaultP1HasG
            )
        );
        infoPlayerP2 = mod->getSavedValue<InfoPlayer>(
            "player2", 
            makeDefaultPlayer(
                m_defaultP2ColorP, 
                m_defaultP2ColorS, 
                m_defaultP2ColorG, 
                m_defaultP2HasG
            )
        );
    }

    // Save the colors and icons
    void save() {
        Mod::get()->setSavedValue("player1", infoPlayerP1);
        Mod::get()->setSavedValue("player2", infoPlayerP2);
    }
};