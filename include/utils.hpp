#pragma once
#include <Geode/Enums.hpp>
#include <Geode/Geode.hpp>

struct InfoGamemode {
    IconType type = IconType::Cube;
    int id = 1;
    int primaryColor = 0;
    int secondaryColor = 0;
    int glowColor = 0;
    bool hasGlow = false;
};

struct InfoPlayer {
    InfoGamemode infoGmCube;
    InfoGamemode infoGmShip;
    InfoGamemode infoGmUFO;
    InfoGamemode infoGmBall;
    InfoGamemode infoGmWave;
    InfoGamemode infoGmRobot;
    InfoGamemode infoGmSpider;
    InfoGamemode infoGmSwing;
    InfoGamemode infoGmJetpack;
};

// Returns if hte icon is unlocked. Checks the enable all icons setting
inline bool isIconAvailable(int id, IconType type) {
    return geode::Mod::get()->getSettingValue<bool>("unlock-all-icons") || GameManager::get()->isIconUnlocked(id, type);
}

// Return  gamemode
inline InfoGamemode& gmFor(InfoPlayer& p, IconType t) {
    switch (t) {
        case IconType::Ship:   
            return p.infoGmShip;
        case IconType::Ball:   
            return p.infoGmBall;
        case IconType::Ufo:    
            return p.infoGmUFO;
        case IconType::Wave:   
            return p.infoGmWave;
        case IconType::Robot:  
            return p.infoGmRobot;
        case IconType::Spider: 
            return p.infoGmSpider;
        case IconType::Swing:  
            return p.infoGmSwing;
        case IconType::Jetpack:
            return p.infoGmJetpack;
        default:               
            return p.infoGmCube;
    }
}

template <>
struct matjson::Serialize<InfoGamemode> {
    static matjson::Value toJson(InfoGamemode const& g) {
        return matjson::makeObject({
            {"type", static_cast<int>(g.type)},
            {"id", g.id},
            {"primaryColor", g.primaryColor},
            {"secondaryColor", g.secondaryColor},
            {"glowColor", g.glowColor},
            {"hasGlow", g.hasGlow},
        });
    }
    static geode::Result<InfoGamemode> fromJson(matjson::Value const& v) {
        InfoGamemode g;
        g.type = static_cast<IconType>(v["type"].as<int>().unwrapOr(0));
        g.id = v["id"].as<int>().unwrapOr(1);
        g.primaryColor = v["primaryColor"].as<int>().unwrapOr(0);
        g.secondaryColor = v["secondaryColor"].as<int>().unwrapOr(0);
        g.glowColor = v["glowColor"].as<int>().unwrapOr(0);
        g.hasGlow = v["hasGlow"].as<bool>().unwrapOr(false);
        return geode::Ok(g);
    }
};

template <>
struct matjson::Serialize<InfoPlayer> {
    static matjson::Value toJson(InfoPlayer const& p) {
        return matjson::makeObject({
            {"cube", p.infoGmCube}, 
            {"ship", p.infoGmShip}, 
            {"ufo", p.infoGmUFO},
            {"ball", p.infoGmBall}, 
            {"wave", p.infoGmWave}, 
            {"robot", p.infoGmRobot},
            {"spider", p.infoGmSpider}, 
            {"swing", p.infoGmSwing},
            {"jetpack", p.infoGmJetpack},
        });
    }
    static geode::Result<InfoPlayer> fromJson(matjson::Value const& v) {
        InfoPlayer p;
        auto rd = [&](char const* key, InfoGamemode& out) {
            if (auto r = v[key].as<InfoGamemode>()) out = r.unwrap();
        };
        
        rd("cube", p.infoGmCube);
        rd("ship", p.infoGmShip);
        rd("ufo", p.infoGmUFO);
        rd("ball", p.infoGmBall);
        rd("wave", p.infoGmWave);
        rd("robot", p.infoGmRobot);
        rd("spider", p.infoGmSpider);
        rd("swing", p.infoGmSwing);
        rd("jetpack", p.infoGmJetpack);

        return geode::Ok(p);
    }
};

// Icontypes
static constexpr IconType IconTypeGamemodes[] = {
    IconType::Cube,
    IconType::Ship, 
    IconType::Ball, 
    IconType::Ufo, 
    IconType::Wave,
    IconType::Robot, 
    IconType::Spider, 
    IconType::Swing, 
    IconType::Jetpack
};

static const char* iconTypeString(IconType t) {
    switch (t) {
        case IconType::Cube:    
            return "Cube";
        case IconType::Ship:    
            return "Ship";
        case IconType::Ball:    
            return "Ball";
        case IconType::Ufo:     
            return "UFO";
        case IconType::Wave:    
            return "Wave";
        case IconType::Robot:   
            return "Robot";
        case IconType::Spider:      
            return "Spider";
        case IconType::Swing:   
            return "Swing";
        case IconType::Jetpack: 
            return "Jetpack";
        default:                
            return "Unknown";
    }
};
