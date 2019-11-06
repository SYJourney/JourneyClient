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
#include "../Messages.h"

#include "../Components/Textfield.h"
#include "../Components/Slider.h"

namespace ms
{
	class UIChatbar : public UIDragElement<PosCHAT>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::CHATBAR;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		enum LineType
		{
			UNK0,
			WHITE,
			RED,
			BLUE,
			YELLOW
		};

		UIChatbar();

		void draw(float inter) const override;
		void update() override;

		void send_key(std::int32_t keycode, bool pressed, bool escape) override;

		bool is_in_range(Point<int16_t> cursorpos) const override;
		Cursor::State send_cursor(bool clicking, Point<int16_t> cursorpos) override;
		Cursor::State check_dragtop(bool clicking, Point<int16_t> cursorpos);

		void send_chatline(const std::string& line, LineType type);
		void display_message(Messages::Type line, UIChatbar::LineType type);
		void toggle_chat();
		void toggle_chat(bool chat_open);
		void toggle_chatfield();
		void toggle_chatfield(bool chatfield_open);
		bool is_chatopen();

	protected:
		Button::State button_pressed(std::uint16_t buttonid) override;

	private:
		bool indragrange(Point<int16_t> cursorpos) const override;

		std::int16_t getchattop(bool chat_open) const;
		std::int16_t getchatbarheight() const;
		Rectangle<int16_t> getbounds(Point<int16_t> additional_area) const;

		static constexpr std::int16_t CHATROWHEIGHT = 13;
		static constexpr std::int16_t MINCHATROWS = 1;
		static constexpr std::int16_t MAXCHATROWS = 16;
		static constexpr std::int16_t DIMENSION_Y = 17;
		static constexpr time_t MESSAGE_COOLDOWN = 1'000;

		enum Buttons : std::uint16_t
		{
			BT_OPENCHAT,
			BT_CLOSECHAT,
			BT_CHAT,
			BT_HELP,
			BT_LINK,
			BT_TAB_0,
			BT_TAB_1,
			BT_TAB_2,
			BT_TAB_3,
			BT_TAB_4,
			BT_TAB_5,
			BT_TAB_ADD,
			BT_CHAT_TARGET
		};

		enum ChatTab
		{
			CHT_ALL,
			CHT_BATTLE,
			CHT_PARTY,
			CHT_FRIEND,
			CHT_GUILD,
			CHT_ALLIANCE,
			NUM_CHATTAB
		};

		std::vector<std::string> ChatTabText =
		{
			"All",
			"Battle",
			"Party",
			"Friend",
			"Guild",
			"Alliance"
		};

		bool chatopen;
		bool chatopen_persist;
		bool chatfieldopen;
		Texture chatspace[4];
		Texture chatenter;
		Texture chatcover;
		Textfield chatfield;
		Point<int16_t> closechat;

		Text chattab_text[UIChatbar::ChatTab::NUM_CHATTAB];
		std::int16_t chattab_x;
		std::int16_t chattab_y;
		std::int16_t chattab_span;

		Slider slider;

		EnumMap<Messages::Type, time_t> message_cooldowns;
		std::vector<std::string> lastentered;
		std::size_t lastpos;

		std::int16_t chatrows;
		std::int16_t rowpos;
		std::int16_t rowmax;
		std::unordered_map<int16_t, Text> rowtexts;

		bool dragchattop;
	};
}