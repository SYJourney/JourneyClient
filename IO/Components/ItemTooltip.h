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

#include "Tooltip.h"

#include "../../Template/BoolPair.h"
#include "MapleFrame.h"
#include "Graphics/Text.h"

namespace ms
{
	class ItemTooltip : public Tooltip
	{
	public:
		ItemTooltip();

		void draw(Point<int16_t> position) const override;

		bool set_item(std::int32_t itemid);

	private:
		std::int32_t itemid;
		std::int16_t fillwidth;
		std::int16_t fillheight;
		std::int16_t descdelta;
		Texture itemicon;

		Text name;
		Text desc;
		Text qual;
		MapleFrame frame;
		Texture cover;
		Texture base;
		Texture itemcover;
		BoolPair<Texture> type;
		bool untradable;
		bool unique;
	};
}