#include <Geode/Geode.hpp>
#include <Geode/modify/GJGarageLayer.hpp>

#include "ui/ICUCharacterColorPage.hpp"

using namespace geode::prelude;

class $modify(GJGarageLayerModify, GJGarageLayer) {

	bool init() {

		if (!GJGarageLayer::init()) {
			return false;
		}

		// Garage button in the left  (shards-menu)
		auto interfaceSprite = CircleButtonSprite::createWithSpriteFrameName(
			"GJ_paintBtn_001.png", 
			1,
			CircleBaseColor::Gray, 
			CircleBaseSize::Small
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
		ICUCharacterColorPage::create()->show();
	}
};