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

#include "../../Character/Questlog.h"
#include "../Components/Textfield.h"
#include "../Components/Slider.h"

namespace ms
{
	class UIQuestLog : public UIDragElement<PosQUEST>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::QUESTLOG;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIQuestLog(const Questlog& questLog);

		void draw(float inter) const override;

		void send_key(std::int32_t keycode, bool pressed, bool escape) override;
		Cursor::State send_cursor(bool clicking, Point<int16_t> cursorpos) override;

	protected:
		Button::State button_pressed(std::uint16_t buttonid) override;

	private:
		void change_tab(std::uint16_t tabid);

		enum Buttons : std::uint16_t
		{
			TAB0,
			TAB1,
			TAB2,
			CLOSE,
			SEARCH,
			ALL_LEVEL,
			MY_LOCATION
		};

		const Questlog& questlog;

		std::uint16_t tab;
		std::vector<Sprite> notice_sprites;
		Textfield search;
		Text placeholder;
		Slider slider;
	};
}