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

#include "UIState.h"

#include "Components/EquipTooltip.h"
#include "Components/ItemTooltip.h"
#include "Components/SkillTooltip.h"
#include "Components/TextTooltip.h"

#include "../Timer.h"

#include "../Template/EnumMap.h"
#include "../Template/Optional.h"
#include "../Character/Charstats.h"

#include <list>
#include <memory>

namespace ms
{
	class UIStateGame : public UIState
	{
	public:
		UIStateGame();

		void draw(float inter, Point<int16_t> cursor) const override;
		void update() override;

		void doubleclick(Point<int16_t> pos) override;
		void rightclick(Point<int16_t> pos) override;
		void send_key(KeyType::Id type, std::int32_t action, bool pressed, bool escape) override;
		Cursor::State send_cursor(Cursor::State mst, Point<int16_t> pos) override;
		void send_scroll(double yoffset) override;
		void send_close() override;

		void drag_icon(Icon* icon) override;
		void clear_tooltip(Tooltip::Parent parent) override;
		void show_equip(Tooltip::Parent parent, std::int16_t slot) override;
		void show_item(Tooltip::Parent parent, std::int32_t itemid) override;
		void show_skill(Tooltip::Parent parent, std::int32_t skill_id, std::int32_t level, std::int32_t masterlevel, int64_t expiration) override;
		void show_text(Tooltip::Parent parent, std::string text) override;

		Iterator pre_add(UIElement::Type type, bool toggled, bool focused);
		void remove(UIElement::Type type) override;
		UIElement* get(UIElement::Type type) override;
		UIElement* get_front(std::list<UIElement::Type> types) override;
		UIElement* get_front(Point<int16_t> pos) override;
		int64_t get_uptime() override;
		std::uint16_t get_uplevel() override;
		int64_t get_upexp() override;

	private:
		const CharStats& stats;

		bool drop_icon(const Icon& icon, Point<int16_t> pos);
		template <class T, typename...Args>
		void emplace(Args&& ...args);

		EnumMap<UIElement::Type, UIElement::UPtr, UIElement::Type::NUM_TYPES> elements;
		std::list<UIElement::Type> elementorder;
		UIElement::Type focused;

		EquipTooltip eqtooltip;
		ItemTooltip ittooltip;
		SkillTooltip sktooltip;
		TextTooltip tetooltip;
		Optional<Tooltip> tooltip;
		Tooltip::Parent tooltipparent;

		Optional<Icon> draggedicon;

		std::int16_t VWIDTH;
		std::int16_t VHEIGHT;
		std::chrono::time_point<std::chrono::steady_clock> start;
		std::uint16_t levelBefore;
		int64_t expBefore;
	};
}