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

#include "DrawArgument.h"
#include "GraphicsGL.h"

namespace ms
{
	class Geometry
	{
	public:
		virtual ~Geometry() {}

	protected:
		void draw(std::int16_t x, std::int16_t y, std::int16_t w, std::int16_t h, Color::Name color, float opacity) const;
	};

	class ColorBox : public Geometry
	{
	public:
		ColorBox(std::int16_t width, std::int16_t height, Color::Name color, float opacity);
		ColorBox();

		void setwidth(std::int16_t width);
		void setheight(std::int16_t height);
		void set_color(Color::Name color);
		void setopacity(float opacity);

		void draw(const DrawArgument& args) const;

	private:
		std::int16_t width;
		std::int16_t height;
		Color::Name color;
		float opacity;
	};

	class ColorLine : public Geometry
	{
	public:
		ColorLine(std::int16_t width, Color::Name color, float opacity);
		ColorLine();

		void setwidth(std::int16_t width);
		void set_color(Color::Name color);
		void setopacity(float opacity);

		void draw(const DrawArgument& args) const;

	private:
		std::int16_t width;
		Color::Name color;
		float opacity;
	};

	class MobHpBar : public Geometry
	{
	public:
		void draw(Point<int16_t> position, std::int16_t hppercent) const;

	private:
		static const std::int16_t WIDTH = 50;
		static const std::int16_t HEIGHT = 10;
	};
}