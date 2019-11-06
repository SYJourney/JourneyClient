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

#include "Stance.h"

#include "../../Template/Point.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace ms
{
	// A frame of animation for a skill or similar 'meta-stance'. 
	// This simply redirects to a different stance and frame to use.
	class BodyAction
	{
	public:
		BodyAction(nl::node src)
		{
			stance = Stance::by_string(src["action"]);
			frame = src["frame"];
			move = src["move"];

			std::int16_t sgndelay = src["delay"];

			if (sgndelay == 0)
				sgndelay = 100;

			if (sgndelay > 0)
			{
				delay = sgndelay;
				attackframe = true;
			}
			else if (sgndelay < 0)
			{
				delay = -sgndelay;
				attackframe = false;
			}
		}

		BodyAction() {}

		bool isattackframe() const
		{
			return attackframe;
		}

		std::uint8_t get_frame() const
		{
			return frame;
		}

		std::uint16_t get_delay() const
		{
			return delay;
		}

		Point<int16_t> get_move() const
		{
			return move;
		}

		Stance::Id get_stance() const
		{
			return stance;
		}

	private:
		Stance::Id stance;
		std::uint8_t frame;
		std::uint16_t delay;
		Point<int16_t> move;
		bool attackframe;
	};

	class BodyDrawinfo
	{
	public:
		void init();

		Point<int16_t> get_body_position(Stance::Id stance, std::uint8_t frame) const;
		Point<int16_t> get_arm_position(Stance::Id stance, std::uint8_t frame) const;
		Point<int16_t> get_hand_position(Stance::Id stance, std::uint8_t frame) const;
		Point<int16_t> get_head_position(Stance::Id stance, std::uint8_t frame) const;
		Point<int16_t> gethairpos(Stance::Id stance, std::uint8_t frame) const;
		Point<int16_t> getfacepos(Stance::Id stance, std::uint8_t frame) const;
		std::uint8_t nextframe(Stance::Id stance, std::uint8_t frame) const;
		std::uint16_t get_delay(Stance::Id stance, std::uint8_t frame) const;

		std::uint16_t get_attackdelay(std::string action, std::size_t no) const;
		std::uint8_t next_actionframe(std::string action, std::uint8_t frame) const;
		const BodyAction* get_action(std::string action, std::uint8_t frame) const;

	private:
		std::unordered_map<std::uint8_t, Point<int16_t>> body_positions[Stance::Id::LENGTH];
		std::unordered_map<std::uint8_t, Point<int16_t>> arm_positions[Stance::Id::LENGTH];
		std::unordered_map<std::uint8_t, Point<int16_t>> hand_positions[Stance::Id::LENGTH];
		std::unordered_map<std::uint8_t, Point<int16_t>> head_positions[Stance::Id::LENGTH];
		std::unordered_map<std::uint8_t, Point<int16_t>> hair_positions[Stance::Id::LENGTH];
		std::unordered_map<std::uint8_t, Point<int16_t>> face_positions[Stance::Id::LENGTH];
		std::unordered_map<std::uint8_t, std::uint16_t> stance_delays[Stance::Id::LENGTH];

		std::unordered_map<std::string, std::unordered_map<std::uint8_t, BodyAction>> body_actions;
		std::unordered_map<std::string, std::vector<std::uint16_t>> attack_delays;
	};
}