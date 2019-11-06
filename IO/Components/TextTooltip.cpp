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
#include "TextTooltip.h"

#include "../../Constants.h"

#include <nlnx/nx.hpp>

namespace ms
{
	TextTooltip::TextTooltip()
	{
		nl::node Frame = nl::nx::ui["UIToolTip.img"]["Item"]["Frame2"];

		frame = Frame;
		cover = Frame["cover"];

		text = "";
	}

	void TextTooltip::draw(Point<int16_t> pos) const
	{
		if (text_label.empty())
			return;

		std::int16_t fillwidth = text_label.width();
		std::int16_t fillheight = text_label.height();

		std::int16_t max_width = Constants::Constants::get().get_viewwidth();
		std::int16_t max_height = Constants::Constants::get().get_viewheight();
		std::int16_t cur_width = pos.x() + fillwidth + 21;
		std::int16_t cur_height = pos.y() + fillheight + 40;

		std::int16_t adj_x = cur_width - max_width;
		std::int16_t adj_y = cur_height - max_height;

		if (adj_x > 0)
			pos.shift_x(adj_x * -1);

		if (adj_y > 0)
			pos.shift_y(adj_y * -1);

		frame.draw(pos + Point<int16_t>(fillwidth / 2 + 2, fillheight - 7), fillwidth - 14, fillheight - 18);
		cover.draw(pos + Point<int16_t>(-5, -2));

		if (fillheight > 18)
			text_label.draw(pos);
		else
			text_label.draw(pos + Point<int16_t>(-1, -3));
	}

	bool TextTooltip::set_text(std::string t)
	{
		if (text == t)
			return false;

		text = t;

		if (text.empty())
			return false;

		text_label = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, text, 340, true, 2);

		return true;
	}
}