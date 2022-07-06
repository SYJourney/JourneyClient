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

#ifdef USE_NX
#include <nlnx/nx.hpp>
#endif

#include <locale>
#include <sstream>

namespace ms
{
	namespace string_format
	{
		void split_number(std::string& input)
		{
			for (size_t i = input.size(); i > 3; i -= 3)
				input.insert(i - 3, 1, ',');
		}

		std::string extend_id(int32_t id, size_t length)
		{
			return pad_string(std::to_string(id), length);
		}

		std::string pad_string(std::string str, size_t length)
		{
			if (str.size() < length)
				str.insert(0, length - str.size(), '0');

			return str;
		}

		void format_with_ellipsis(Text& input, int16_t width, uint16_t ellipsis_width, bool word_break)
		{
			if (input.get_text().empty() || width == 0 || input.width() <= width)
				return;

			uint16_t i = 0;
			std::string ellipsis;
			std::string text = input.get_text();

			while (i < ellipsis_width)
			{
				ellipsis += ".";
				i++;
			}

			while (input.width() > width)
			{
				if (word_break)
				{
					size_t space = text.find_last_of(' ');

					if (space != std::string::npos)
						text = text.substr(0, space + 1);

					if (text.back() == ' ')
						text.pop_back();
				}
				else
				{
					text.pop_back();
				}

				input.change_text(text + ' ' + ellipsis);
			}
		}

		std::string tolower(std::string str)
		{
			std::locale loc;
			std::stringstream ss;

			for (std::string::size_type i = 0; i < str.length(); ++i)
				ss << std::tolower(str[i], loc);

			return ss.str();
		}
	}

	namespace bytecode
	{
		bool compare(int32_t mask, int32_t value)
		{
			return (mask & value) != 0;
		}
	}

	namespace NxHelper
	{
		namespace Map
		{
			// TODO: nl::nx::String["Map.img"]["victoria"][160070000] is returning invalid data, yet the WZ file is fine.
			MapInfo get_map_info_by_id(int32_t mapid)
			{
				std::string map_category = get_map_category(mapid);
				nl::node map_info = nl::nx::String["Map.img"][map_category][mapid];

				return {
					map_info["mapDesc"],
					map_info["mapName"],
					map_info["streetName"],
					map_info["streetName"] + " : " + map_info["mapName"]
				};
			}

			std::string get_map_category(int32_t mapid)
			{
				if (mapid < 100000000)
					return "maple";

				if (mapid < 200000000)
					return "victoria";

				if (mapid < 300000000)
					return "ossyria";

				if (mapid < 540000000)
					return "elin";

				if (mapid < 600000000)
					return "singapore";

				if (mapid < 670000000)
					return "MasteriaGL";

				if (mapid < 682000000)
				{
					int32_t prefix3 = (mapid / 1000000) * 1000000;
					int32_t prefix4 = (mapid / 100000) * 100000;

					if (prefix3 == 674000000 || prefix4 == 680100000 || prefix4 == 889100000)
						return "etc";

					if (prefix3 == 677000000)
						return "Episode1GL";

					return "weddingGL";
				}

				if (mapid < 683000000)
					return "HalloweenGL";

				if (mapid < 800000000)
					return "event";

				if (mapid < 900000000)
					return "jp";

				return "etc";
			}

			std::unordered_map<int64_t, std::pair<std::string, std::string>> get_life_on_map(int32_t mapid)
			{
				std::unordered_map<int64_t, std::pair<std::string, std::string>> map_life;

				nl::node portal = get_map_node_name(mapid);

				for (nl::node life : portal["life"])
				{
					int64_t life_id = life["id"];
					std::string life_type = life["type"];
					uint8_t hide_life = life["hide"];

					if (!hide_life)
					{
						if (life_type == "m")
						{
							// Mob
							nl::node life_name = nl::nx::String["Mob.img"][life_id]["name"];

							std::string life_id_str = string_format::extend_id(life_id, 7);
							nl::node life_level = nl::nx::Mob[life_id_str + ".img"]["info"]["level"];

							if (life_name && life_level)
								map_life[life_id] = { life_type, life_name + "(Lv. " + life_level + ")" };
						}
						else if (life_type == "n")
						{
							// NPC
							if (nl::node life_name = nl::nx::String["Npc.img"][life_id]["name"])
								map_life[life_id] = { life_type, life_name };
						}
					}
				}

				return map_life;
			}

			nl::node get_map_node_name(int32_t mapid)
			{
				std::string prefix = std::to_string(mapid / 100000000);
				std::string mapid_str = string_format::extend_id(mapid, 9);

				return nl::nx::Map002["Map"]["Map" + prefix][mapid_str + ".img"];
			}
		}
	}

	namespace single_console
	{
		void log_message(std::string message)
		{
			if (std::find(std::begin(log_history), std::end(log_history), message) == std::end(log_history))
			{
				log_history.push_back(message);

				std::cout << message << std::endl;
			}
		}
	}
}