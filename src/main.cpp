#include <Geode/Geode.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include "ICFrame.hpp"
#include "ICGamemodeNavbar.hpp"
#include "ICGamemodeIcons.hpp"

using namespace geode::prelude;

static const std::array<const char*, 9> nameGamemodes = {
    "icon",
	"ship", 
	"ball", 
	"bird", 
	"dart", 
	"robot", 
	"spider", 
	"swing", 
	"jetpack"
};

class $modify(GJGarageLayerModify, GJGarageLayer) {

	struct Fields {
		std::array<CCMenuItemSpriteExtra*, 9> gamemodeBtns{};
		int selectedMode = 0;
	};

	bool init() {

		if (!GJGarageLayer::init()) {
			return false;
		}

		// Garage button in the left  (shards-menu)
		auto interfaceSprite = CCSprite::createWithSpriteFrameName(
			"GJ_paintBtn_001.png"
		);
		auto interfaceBtn = CCMenuItemSpriteExtra::create(
			interfaceSprite,
			this,
			menu_selector(GJGarageLayerModify::onInterfaceBtn)
		);
		interfaceBtn->setID("interfaceBtn");

		auto menu = this->getChildByID("shards-menu");
		menu->addChild(interfaceBtn);
		menu->updateLayout();

		return true;
	}

	void onInterfaceBtn(CCObject*) {
		auto winSize = CCDirector::get()->getWinSize();
		auto frame = ICFrame::create(winSize.width * 0.8f, winSize.height * 0.9f);

		frame->setPosition(winSize / 2);
		this->addChild(frame, 100);

		// Draw navbar, icons and all colors
		///// Gamemodes navbar
		auto gamemodeNavbar = ICGamemodeNavbar::create(
			frame->getContentSize(),
			[this](int mode) { log::info("Gamemode {} selected", mode); }
		);

		frame->addChild(gamemodeNavbar);

		///// Icons


		///// Colors


	}
};