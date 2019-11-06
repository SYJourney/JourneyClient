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

#include "ItemData.h"

#include "../Character/Equipstat.h"
#include "../Character/Maplestat.h"
#include "../Template/EnumMap.h"

#include "../Character/Look/Equipslot.h"

namespace ms
{
	// Contains information about an equip.
	class EquipData : public Cache<EquipData>
	{
	public:
		// Returns whether the equip was loaded correctly.
		bool is_valid() const;
		// Returns whether the equip was loaded correctly.
		explicit operator bool() const;

		// Returns whether this equip has equipslot WEAPON.
		bool is_weapon() const;
		// Returns a required base stat.
		std::int16_t get_reqstat(Maplestat::Id stat) const;
		// Returns a default stat.
		std::int16_t get_defstat(Equipstat::Id stat) const;
		// Returns the equip slot.
		Equipslot::Id get_eqslot() const;
		// Returns the category name.
		const std::string& get_type() const;
		// Returns the general item data (name, price, etc.).
		const ItemData& get_itemdata() const;

	private:
		// Allow the cache to use the constructor.
		friend Cache<EquipData>;
		// Load an equip from the game files.
		EquipData(std::int32_t id);

		const ItemData& itemdata;

		EnumMap<Maplestat::Id, std::int16_t> reqstats;
		EnumMap<Equipstat::Id, std::int16_t> defstats;
		std::string type;
		Equipslot::Id eqslot;
		std::uint8_t slots;
		bool cash;
		bool tradeblock;
	};
}