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
#include "Misc.h"

namespace ms
{
	namespace string_format
	{
		void split_number(std::string& input)
		{
			for (std::size_t i = input.size(); i > 3; i -= 3)
				input.insert(i - 3, 1, ',');
		}

		std::string extend_id(std::int32_t id, std::size_t length)
		{
			std::string strid = std::to_string(id);

			if (strid.size() < length)
				strid.insert(0, length - strid.size(), '0');

			return strid;
		}
	}

	namespace bytecode
	{
		bool compare(std::int32_t mask, std::int32_t value)
		{
			return (mask & value) != 0;
		}
	}
}