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
#include "Gauge.h"

namespace ms
{
	Gauge::Gauge(Texture front, std::int16_t max, float percent) : barfront(front), maximum(max), percentage(percent)
	{
		target = percentage;
	}

	Gauge::Gauge(Texture front, Texture mid, std::int16_t max, float percent) : barfront(front), barmid(mid), maximum(max), percentage(percent)
	{
		target = percentage;
	}

	Gauge::Gauge(Texture front, Texture mid, Texture end, std::int16_t max, float percent) : barfront(front), barmid(mid), barend(end), maximum(max), percentage(percent)
	{
		target = percentage;
	}

	Gauge::Gauge() {}

	void Gauge::draw(const DrawArgument& args) const
	{
		std::int16_t length = static_cast<int16_t>(percentage * maximum);

		if (length > 0)
		{
			barfront.draw(args + DrawArgument(Point<int16_t>(0, 0), Point<int16_t>(length, 0)));
			barmid.draw(args);
			barend.draw(args + Point<int16_t>(length + 8, 20));
		}
	}

	void Gauge::update(float t)
	{
		if (target != t)
		{
			target = t;
			step = (target - percentage) / 24;
		}

		if (percentage != target)
		{
			percentage += step;

			if (step < 0.0f)
			{
				if (target - percentage >= step)
					percentage = target;
			}
			else if (step > 0.0f)
			{
				if (target - percentage <= step)
					percentage = target;
			}
		}
	}
}