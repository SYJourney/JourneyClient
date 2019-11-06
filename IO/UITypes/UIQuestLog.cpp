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
#include "UIQuestLog.h"

#include "../../IO/Components/MapleButton.h"
#include "../../IO/Components/TwoSpriteButton.h"

#include <nlnx/nx.hpp>

namespace ms
{
	UIQuestLog::UIQuestLog(const Questlog& ql) : UIDragElement<PosQUEST>(Point<int16_t>(172, 20)), questlog(ql)
	{
		tab = Buttons::TAB0;

		nl::node close = nl::nx::ui["Basic.img"]["BtClose"];
		nl::node quest = nl::nx::ui["UIWindow2.img"]["Quest"];
		nl::node list = quest["list"];

		nl::node backgrnd = list["backgrnd"];

		sprites.emplace_back(backgrnd);
		sprites.emplace_back(list["backgrnd2"]);
		sprites.emplace_back(list["searchArea"]);

		notice_sprites.emplace_back(list["notice0"]);
		notice_sprites.emplace_back(list["notice1"]);
		notice_sprites.emplace_back(list["notice2"]);

		nl::node taben = list["Tab"]["enabled"];
		nl::node tabdis = list["Tab"]["disabled"];

		buttons[Buttons::TAB0] = std::make_unique<TwoSpriteButton>(tabdis["0"], taben["0"]);
		buttons[Buttons::TAB1] = std::make_unique<TwoSpriteButton>(tabdis["1"], taben["1"]);
		buttons[Buttons::TAB2] = std::make_unique<TwoSpriteButton>(tabdis["2"], taben["2"]);
		buttons[Buttons::CLOSE] = std::make_unique<MapleButton>(close, Point<int16_t>(281, 12));
		buttons[Buttons::SEARCH] = std::make_unique<MapleButton>(list["BtSearch"]);
		buttons[Buttons::ALL_LEVEL] = std::make_unique<MapleButton>(list["BtAllLevel"]);
		buttons[Buttons::MY_LOCATION] = std::make_unique<MapleButton>(list["BtMyLocation"]);

		search = Textfield(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BOULDER, Rectangle<int16_t>(Point<int16_t>(124, 141), Point<int16_t>(320, 153)), 50);
		placeholder = Text(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::BOULDER, "Enter the quest name.");

		slider = Slider(Slider::Type::DEFAULT, Range<int16_t>(0, 279), 150, 20, 5, [](bool) {});

		change_tab(tab);

		dimension = Texture(backgrnd).get_dimensions();
	}

	void UIQuestLog::draw(float alpha) const
	{
		UIElement::draw(alpha);

		Point<int16_t> notice_position = Point<int16_t>(0, 26);

		if (tab == Buttons::TAB0)
			notice_sprites[tab].draw(position + notice_position + Point<int16_t>(9, 0), alpha);
		else if (tab == Buttons::TAB1)
			notice_sprites[tab].draw(position + notice_position + Point<int16_t>(0, 0), alpha);
		else
			notice_sprites[tab].draw(position + notice_position + Point<int16_t>(-10, 0), alpha);

		search.draw(position + Point<int16_t>(-85, -90));

		if (search.get_state() == Textfield::State::NORMAL && search.empty())
			placeholder.draw(position + Point<int16_t>(39, 51));

		slider.draw(position + Point<int16_t>(126, 75));
	}

	void UIQuestLog::send_key(std::int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
			{
				deactivate();
			}
			else if (keycode == KeyAction::Id::TAB)
			{
				std::uint16_t new_tab = tab;

				if (new_tab < Buttons::TAB2)
					new_tab++;
				else
					new_tab = Buttons::TAB0;

				change_tab(new_tab);
			}
		}
	}

	Cursor::State UIQuestLog::send_cursor(bool clicking, Point<int16_t> cursorpos)
	{
		if (Cursor::State new_state = search.send_cursor(cursorpos, clicking))
			return new_state;

		return UIDragElement::send_cursor(clicking, cursorpos);
	}

	Button::State UIQuestLog::button_pressed(std::uint16_t buttonid)
	{
		switch (buttonid)
		{
		case Buttons::TAB0:
		case Buttons::TAB1:
		case Buttons::TAB2:
			change_tab(buttonid);

			return Button::State::IDENTITY;
		case Buttons::CLOSE:
			deactivate();

			return Button::State::NORMAL;
		default:
			return Button::State::PRESSED;
		}
	}

	void UIQuestLog::change_tab(std::uint16_t tabid)
	{
		std::uint16_t oldtab = tab;
		tab = tabid;

		if (oldtab != tab)
			buttons[Buttons::TAB0 + oldtab]->set_state(Button::State::NORMAL);

		buttons[Buttons::TAB0 + tab]->set_state(Button::State::PRESSED);
	}
}