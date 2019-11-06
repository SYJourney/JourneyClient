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
#include "UIStatusMessenger.h"

#include "../../Constants.h"

namespace ms
{
	StatusInfo::StatusInfo(const std::string& str, Color::Name color)
	{
		text = Text(Text::Font::A12M, Text::Alignment::RIGHT, color, str);
		shadow = Text(Text::Font::A12M, Text::Alignment::RIGHT, Color::Name::BLACK, str);

		opacity.set(1.0f);
	}

	void StatusInfo::draw(Point<int16_t> position, float alpha) const
	{
		float interopc = opacity.get(alpha);

		shadow.draw(DrawArgument(position + Point<int16_t>(1, 1), interopc));
		text.draw(DrawArgument(position, interopc));
	}

	bool StatusInfo::update()
	{
		constexpr float FADE_STEP = Constants::TIMESTEP * 1.0f / FADE_DURATION;
		opacity -= FADE_STEP;

		return opacity.last() < FADE_STEP;
	}

	UIStatusMessenger::UIStatusMessenger()
	{
		position = Point<int16_t>(790, 500);
	}

	void UIStatusMessenger::draw(float inter) const
	{
		Point<int16_t> infopos = Point<int16_t>(position.x(), position.y());

		for (const StatusInfo& info : statusinfos)
		{
			info.draw(infopos, inter);
			infopos.shift_y(-16);
		}
	}

	void UIStatusMessenger::update()
	{
		for (StatusInfo& info : statusinfos)
			info.update();
	}

	void UIStatusMessenger::show_status(Color::Name color, const std::string& message)
	{
		statusinfos.push_front(StatusInfo(message, color));

		if (statusinfos.size() > MAX_MESSAGES)
			statusinfos.pop_back();
	}
}