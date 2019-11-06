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

#include "../UIElement.h"

namespace ms
{
	class UIGender : public UIElement
	{
	public:
		static constexpr Type TYPE = UIElement::Type::GENDER;
		static constexpr bool FOCUSED = true;
		static constexpr bool TOGGLED = false;

		UIGender(std::function<void()> okhandler);

		void draw(float inter) const override;
		void update() override;

		bool remove_cursor(bool clicked, Point<int16_t> cursorpos) override;
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursorpos) override;

	protected:
		Button::State button_pressed(std::uint16_t buttonid) override;

	private:
		enum Buttons : std::uint16_t
		{
			NO,
			YES,
			SELECT
		};

		Texture gender_sprites[3];
		std::uint16_t CUR_TIMESTEP;
		std::function<void()> okhandler;
	};
}