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
#include "MapleFrame.h"

#include <algorithm>

#include <nlnx/nx.hpp>

namespace ms
{
	MapleFrame::MapleFrame(nl::node src)
	{
		center = src["c"];
		east = src["e"];
		northeast = src["ne"];
		north = src["n"];
		northwest = src["nw"];
		west = src["w"];
		southwest = src["sw"];
		south = src["s"];
		southeast = src["se"];

		xtile = std::max<int16_t>(north.width(), 1);
		ytile = std::max<int16_t>(west.height(), 1);
	}

	MapleFrame::MapleFrame() {}

	void MapleFrame::draw(Point<int16_t> position, std::int16_t rwidth, std::int16_t rheight) const
	{
		std::int16_t numhor = rwidth / xtile + 2;
		std::int16_t numver = rheight / ytile;
		std::int16_t width = numhor * xtile;
		std::int16_t height = numver * ytile;
		std::int16_t left = position.x() - width / 2;
		std::int16_t top = position.y() - height;
		std::int16_t right = left + width;
		std::int16_t bottom = top + height;

		northwest.draw(DrawArgument(left, top));
		southwest.draw(DrawArgument(left, bottom));

		for (std::int16_t y = top; y < bottom; y += ytile)
		{
			west.draw(DrawArgument(left, y));
			east.draw(DrawArgument(right, y));
		}

		center.draw(DrawArgument(Point<int16_t>(left, top), Point<int16_t>(width, height)));

		for (std::int16_t x = left; x < right; x += xtile)
		{
			north.draw(DrawArgument(x, top));
			south.draw(DrawArgument(x, bottom));
		}

		northeast.draw(DrawArgument(right, top));
		southeast.draw(DrawArgument(right, bottom));
	}
}