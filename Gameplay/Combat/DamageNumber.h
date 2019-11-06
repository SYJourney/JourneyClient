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

#include "../Physics/PhysicsObject.h"
#include "../../Template/BoolPair.h"
#include "../../Template/Interpolated.h"
#include "../../Template/Point.h"

#include "../../IO/Components/Charset.h"

namespace ms
{
	class DamageNumber
	{
	public:
		static const std::size_t NUM_TYPES = 3;

		enum Type
		{
			NORMAL,
			CRITICAL,
			TOPLAYER
		};

		DamageNumber(Type type, std::int32_t damage, std::int16_t starty, std::int16_t x = 0);
		DamageNumber();

		void draw(double viewx, double viewy, float alpha) const;
		void set_x(std::int16_t headx);
		bool update();

		static std::int16_t rowheight(bool critical);
		static void init();

	private:
		std::int16_t getadvance(char c, bool first) const;

		static constexpr std::uint16_t FADE_TIME = 500;

		Type type;
		bool miss;
		bool multiple;
		std::int8_t firstnum;
		std::string restnum;
		std::int16_t shift;
		MovingObject moveobj;
		Linear<float> opacity;

		static BoolPair<Charset> charsets[NUM_TYPES];
	};
}