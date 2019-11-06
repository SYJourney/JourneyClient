//////////////////////////////////////////////////////////////////////////////////
//	This file is part of the continued Journey MMORPG client					//
//	Copyright (C) 2015-2019  Daniel Allendorf, Ryan Payton						//
//																				//
//	This program is free software: you can redistribute it and/or modify		//
//	it under the terms of the GNU Affero General Public License as published by	//
//	the Free Software Foundation, either version 3 of the License, or			//
//	(at your option) any later version.											//
//																				//
//	This program is distributed in the hope that it will be useful,				//
//	but WITHOUT ANY WARRANTY; without even the implied warranty of				//
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the				//
//	GNU Affero General Public License for more details.							//
//																				//
//	You should have received a copy of the GNU Affero General Public License	//
//	along with this program.  If not, see <https://www.gnu.org/licenses/>.		//
//////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../UIDragElement.h"
#include "../KeyAction.h"
#include "../KeyConfig.h"
#include "../Keyboard.h"

#include "../../Template/EnumMap.h"

namespace ms
{
	class UIKeyConfig : public UIDragElement<PosKEYCONFIG>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::KEYCONFIG;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIKeyConfig();

		void draw(float inter) const override;
		void update() override;

		void send_key(std::int32_t keycode, bool pressed, bool escape) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;
		bool send_icon(const Icon& icon, Point<int16_t> cursorpos) override;

		void remove_key(KeyAction::Id action);
		void add_key(Point<int16_t> cursorposition, KeyAction::Id action);

		void close();

	protected:
		Button::State button_pressed(std::uint16_t buttonid) override;

	private:
		void load_keys_pos();
		void load_icons_pos();
		void load_keys();
		void load_icons();
		void map_keys();
		void clear();
		void reset();

		KeyAction::Id icon_by_position(Point<int16_t> position) const;
		KeyConfig::Key key_by_position(Point<int16_t> position) const;
		KeyConfig::Key all_keys_by_position(Point<int16_t> position) const;
		std::int32_t get_tempkey(KeyAction::Id action) const;
		Keyboard::Mapping get_tempkey_mapping(std::int32_t keycode) const;
		KeyType::Id get_keytype(KeyAction::Id action) const;

		enum Buttons : std::uint16_t
		{
			CLOSE,
			CANCEL,
			DEFAULT,
			DELETE,
			KEYSETTING,
			OK
		};

		class KeyIcon : public Icon::Type
		{
		public:
			KeyIcon(KeyAction::Id keyId);

			void drop_on_stage() const override {}
			void drop_on_equips(Equipslot::Id) const override {}
			bool drop_on_items(InventoryType::Id, Equipslot::Id, std::int16_t, bool) const override { return true; }
			void drop_on_bindings(Point<int16_t> cursorposition, bool remove) const override;
			void set_count(std::int16_t) override {}

		private:
			KeyAction::Id source;
		};

		nl::node icon;
		EnumMap<KeyAction::Id, std::unique_ptr<Icon>> icons;
		EnumMap<KeyAction::Id, Point<int16_t>> icons_pos;

		nl::node key;
		EnumMap<KeyConfig::Key, Texture> keys;
		EnumMap<KeyConfig::Key, Point<int16_t>> keys_pos;

		Keyboard* keyboard = nullptr;
		std::vector<KeyAction::Id> found_actions;
		std::map<std::int32_t, Keyboard::Mapping> tempkeys;

		std::map<std::int32_t, Keyboard::Mapping> alternate_keys = {
		   { KeyConfig::Key::ESCAPE, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::MAINMENU) },
		   { KeyConfig::Key::F1, Keyboard::Mapping(KeyType::Id::FACE, KeyAction::Id::FACE1) },
		   { KeyConfig::Key::F2, Keyboard::Mapping(KeyType::Id::FACE, KeyAction::Id::FACE2) },
		   { KeyConfig::Key::F3, Keyboard::Mapping(KeyType::Id::FACE, KeyAction::Id::FACE3) },
		   { KeyConfig::Key::F5, Keyboard::Mapping(KeyType::Id::FACE, KeyAction::Id::FACE4) },
		   { KeyConfig::Key::F6, Keyboard::Mapping(KeyType::Id::FACE, KeyAction::Id::FACE5) },
		   { KeyConfig::Key::F7, Keyboard::Mapping(KeyType::Id::FACE, KeyAction::Id::FACE6) },
		   { KeyConfig::Key::F8, Keyboard::Mapping(KeyType::Id::FACE, KeyAction::Id::FACE7) },
		   { KeyConfig::Key::SCROLL_LOCK, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::SCREENSHOT) },
		   { KeyConfig::Key::GRAVE_ACCENT, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::CASHSHOP) },
		   { KeyConfig::Key::INSERT, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::SAY) },
		   { KeyConfig::Key::HOME, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::PARTYCHAT) },
		   { KeyConfig::Key::PAGE_UP, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::FRIENDSCHAT) },
		   { KeyConfig::Key::T, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::BOSSPARTY) },
		   { KeyConfig::Key::Y, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::ITEMPOT) },
		   { KeyConfig::Key::U, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::EQUIPMENT) },
		   { KeyConfig::Key::I, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::ITEMS) },
		   { KeyConfig::Key::P, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::PARTY) },
		   { KeyConfig::Key::LEFT_BRACKET, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::MENU) },
		   { KeyConfig::Key::RIGHT_BRACKET, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::QUICKSLOTS) },
		   { KeyConfig::Key::BACKSLASH, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::KEYBINDINGS) },
		   { KeyConfig::Key::DELETE, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::GUILDCHAT) },
		   { KeyConfig::Key::END, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::ALLIANCECHAT) },
		   { KeyConfig::Key::G, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::GUILD) },
		   { KeyConfig::Key::H, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::WHISPER) },
		   { KeyConfig::Key::J, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::QUESTLOG) },
		   { KeyConfig::Key::K, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::SKILLS) },
		   { KeyConfig::Key::L, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::NOTIFIER) },
		   { KeyConfig::Key::SEMICOLON, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::MEDALS) },
		   { KeyConfig::Key::APOSTROPHE, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::TOGGLECHAT) },
		   { KeyConfig::Key::Z, Keyboard::Mapping(KeyType::Id::ACTION, KeyAction::Id::PICKUP) },
		   { KeyConfig::Key::X, Keyboard::Mapping(KeyType::Id::ACTION, KeyAction::Id::SIT) },
		   { KeyConfig::Key::C, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::STATS) },
		   { KeyConfig::Key::V, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::EVENT) },
		   { KeyConfig::Key::B, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::PROFESSION) },
		   { KeyConfig::Key::N, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::WORLDMAP) },
		   { KeyConfig::Key::M, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::MINIMAP) },
		   { KeyConfig::Key::PERIOD, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::FRIENDS) },
		   { KeyConfig::Key::LEFT_CONTROL, Keyboard::Mapping(KeyType::Id::ACTION, KeyAction::Id::ATTACK) },
		   { KeyConfig::Key::LEFT_ALT, Keyboard::Mapping(KeyType::Id::ACTION, KeyAction::Id::JUMP) },
		   { KeyConfig::Key::SPACE, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::INTERACT_HARVEST) },
		   { KeyConfig::Key::RIGHT_ALT, Keyboard::Mapping(KeyType::Id::ACTION, KeyAction::Id::JUMP) },
		   { KeyConfig::Key::RIGHT_CONTROL, Keyboard::Mapping(KeyType::Id::ACTION, KeyAction::Id::ATTACK) }
		};

		std::map<std::int32_t, Keyboard::Mapping> basic_keys = {
		   { KeyConfig::Key::ESCAPE, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::MAINMENU) },
		   { KeyConfig::Key::F1, Keyboard::Mapping(KeyType::Id::FACE, KeyAction::Id::FACE1) },
		   { KeyConfig::Key::F2, Keyboard::Mapping(KeyType::Id::FACE, KeyAction::Id::FACE2) },
		   { KeyConfig::Key::F3, Keyboard::Mapping(KeyType::Id::FACE, KeyAction::Id::FACE3) },
		   { KeyConfig::Key::F5, Keyboard::Mapping(KeyType::Id::FACE, KeyAction::Id::FACE4) },
		   { KeyConfig::Key::F6, Keyboard::Mapping(KeyType::Id::FACE, KeyAction::Id::FACE5) },
		   { KeyConfig::Key::F7, Keyboard::Mapping(KeyType::Id::FACE, KeyAction::Id::FACE6) },
		   { KeyConfig::Key::F8, Keyboard::Mapping(KeyType::Id::FACE, KeyAction::Id::FACE7) },
		   { KeyConfig::Key::SCROLL_LOCK, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::SCREENSHOT) },
		   { KeyConfig::Key::GRAVE_ACCENT, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::CASHSHOP) },
		   { KeyConfig::Key::NUM1, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::SAY) },
		   { KeyConfig::Key::NUM2, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::PARTYCHAT) },
		   { KeyConfig::Key::NUM3, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::FRIENDSCHAT) },
		   { KeyConfig::Key::NUM4, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::GUILDCHAT) },
		   { KeyConfig::Key::NUM5, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::ALLIANCECHAT) },
		   { KeyConfig::Key::Q, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::QUESTLOG) },
		   { KeyConfig::Key::W, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::WORLDMAP) },
		   { KeyConfig::Key::E, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::EQUIPMENT) },
		   { KeyConfig::Key::R, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::FRIENDS) },
		   { KeyConfig::Key::T, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::BOSSPARTY) },
		   { KeyConfig::Key::Y, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::ITEMPOT) },
		   { KeyConfig::Key::U, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::GUIDE) },
		   { KeyConfig::Key::I, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::ITEMS) },
		   { KeyConfig::Key::O, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::ENHANCEEQUIP) },
		   { KeyConfig::Key::P, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::PARTY) },
		   { KeyConfig::Key::LEFT_BRACKET, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::MENU) },
		   { KeyConfig::Key::RIGHT_BRACKET, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::QUICKSLOTS) },
		   { KeyConfig::Key::BACKSLASH, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::KEYBINDINGS) },
		   { KeyConfig::Key::S, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::STATS) },
		   { KeyConfig::Key::G, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::GUILD) },
		   { KeyConfig::Key::H, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::WHISPER) },
		   { KeyConfig::Key::K, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::SKILLS) },
		   { KeyConfig::Key::L, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::NOTIFIER) },
		   { KeyConfig::Key::SEMICOLON, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::MEDALS) },
		   { KeyConfig::Key::APOSTROPHE, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::TOGGLECHAT) },
		   { KeyConfig::Key::Z, Keyboard::Mapping(KeyType::Id::ACTION, KeyAction::Id::PICKUP) },
		   { KeyConfig::Key::X, Keyboard::Mapping(KeyType::Id::ACTION, KeyAction::Id::SIT) },
		   { KeyConfig::Key::C, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::MAPLECHAT) },
		   { KeyConfig::Key::V, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::EVENT) },
		   { KeyConfig::Key::B, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::PROFESSION) },
		   { KeyConfig::Key::M, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::MINIMAP) },
		   { KeyConfig::Key::LEFT_CONTROL, Keyboard::Mapping(KeyType::Id::ACTION, KeyAction::Id::ATTACK) },
		   { KeyConfig::Key::LEFT_ALT, Keyboard::Mapping(KeyType::Id::ACTION, KeyAction::Id::JUMP) },
		   { KeyConfig::Key::SPACE, Keyboard::Mapping(KeyType::Id::MENU, KeyAction::Id::INTERACT_HARVEST) },
		   { KeyConfig::Key::RIGHT_ALT, Keyboard::Mapping(KeyType::Id::ACTION, KeyAction::Id::JUMP) },
		   { KeyConfig::Key::RIGHT_CONTROL, Keyboard::Mapping(KeyType::Id::ACTION, KeyAction::Id::ATTACK) }
		};
	};
}