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
#include "UIStateGame.h"
#include "UI.h"

#include "UITypes/UIStatusMessenger.h"
#include "UITypes/UIStatusbar.h"
#include "UITypes/UIChatbar.h"
#include "UITypes/UIMiniMap.h"
#include "UITypes/UIBuffList.h"
#include "UITypes/UINpcTalk.h"
#include "UITypes/UIShop.h"
#include "UITypes/UIStatsinfo.h"
#include "UITypes/UIItemInventory.h"
#include "UITypes/UIEquipInventory.h"
#include "UITypes/UISkillbook.h"
#include "UITypes/UIChat.h"
#include "UITypes/UIQuestLog.h"
#include "UITypes/UIWorldMap.h"
#include "UITypes/UIUserList.h"
#include "UITypes/UIKeyConfig.h"
#include "UITypes/UIEvent.h"
#include "UITypes/UIChannel.h"
#include "UITypes/UIQuit.h"

#include "../Gameplay/Stage.h"

namespace ms
{
	UIStateGame::UIStateGame() : stats(Stage::get().get_player().get_stats())
	{
		focused = UIElement::Type::NONE;
		tooltipparent = Tooltip::Parent::NONE;

		const CharLook& look = Stage::get().get_player().get_look();
		const Inventory& inventory = Stage::get().get_player().get_inventory();

		emplace<UIStatusMessenger>();
		emplace<UIStatusbar>(stats);
		emplace<UIChatbar>();
		emplace<UIMiniMap>(stats);
		emplace<UIBuffList>();
		emplace<UINpcTalk>();
		emplace<UIShop>(look, inventory);

		VWIDTH = Constants::Constants::get().get_viewwidth();
		VHEIGHT = Constants::Constants::get().get_viewheight();

		start = ContinuousTimer::get().start();
		levelBefore = stats.get_stat(Maplestat::Id::LEVEL);
		expBefore = stats.get_exp();
	}

	void UIStateGame::draw(float inter, Point<int16_t> cursor) const
	{
		for (auto& type : elementorder)
		{
			auto& element = elements[type];

			if (element && element->is_active())
				element->draw(inter);
		}

		if (tooltip)
			tooltip->draw(cursor + Point<int16_t>(0, 22));

		if (draggedicon)
			draggedicon->dragdraw(cursor);
	}

	void UIStateGame::update()
	{
		std::int16_t new_width = Constants::Constants::get().get_viewwidth();
		std::int16_t new_height = Constants::Constants::get().get_viewheight();

		if (VWIDTH != new_width || VHEIGHT != new_height)
		{
			VWIDTH = new_width;
			VHEIGHT = new_height;

			UI::get().remove(UIElement::Type::STATUSBAR);

			const CharStats& stats = Stage::get().get_player().get_stats();
			emplace<UIStatusbar>(stats);
		}

		for (auto& type : elementorder)
		{
			auto& element = elements[type];

			if (element && element->is_active())
				element->update();
		}
	}

	bool UIStateGame::drop_icon(const Icon& icon, Point<int16_t> pos)
	{
		if (UIElement * front = get_front(pos))
			return front->send_icon(icon, pos);
		else
			icon.drop_on_stage();

		return true;
	}

	void UIStateGame::doubleclick(Point<int16_t> pos)
	{
		if (UIElement * front = get_front(pos))
			front->doubleclick(pos);
	}

	void UIStateGame::rightclick(Point<int16_t> pos)
	{
		if (UIElement * front = get_front(pos))
			front->rightclick(pos);
	}

	void UIStateGame::send_key(KeyType::Id type, std::int32_t action, bool pressed, bool escape)
	{
		switch (type)
		{
		case KeyType::Id::MENU:
			if (pressed)
			{
				switch (action)
				{
				case KeyAction::Id::EQUIPMENT:
					emplace<UIEquipInventory>(
						Stage::get().get_player().get_inventory()
						);
					break;
				case KeyAction::Id::ITEMS:
				{
					auto iteminventory = UI::get().get_element<UIItemInventory>();

					if (!iteminventory)
					{
						emplace<UIItemInventory>(
							Stage::get().get_player().get_inventory()
							);
					}
					else
					{
						iteminventory->toggle_active();
					}
				}
				break;
				case KeyAction::Id::STATS:
					emplace<UIStatsinfo>(
						Stage::get().get_player().get_stats()
						);
					break;
				case KeyAction::Id::SKILLS:
				{
					auto skillbook = UI::get().get_element<UISkillbook>();

					if (!skillbook)
					{
						emplace<UISkillbook>(
							Stage::get().get_player().get_stats(),
							Stage::get().get_player().get_skills()
							);
					}
					else
					{
						if (!skillbook->is_skillpoint_enabled())
							skillbook->toggle_active();
					}
				}
				break;
				case KeyAction::Id::FRIENDS:
				case KeyAction::Id::PARTY:
				case KeyAction::Id::BOSSPARTY:
				{
					auto userlist = UI::get().get_element<UIUserList>();
					UIUserList::Tab tab;

					switch (action)
					{
					case KeyAction::Id::FRIENDS:
						tab = UIUserList::Tab::FRIEND;
						break;
					case KeyAction::Id::PARTY:
						tab = UIUserList::Tab::PARTY;
						break;
					case KeyAction::Id::BOSSPARTY:
						tab = UIUserList::Tab::BOSS;
						break;
					}

					if (!userlist)
					{
						emplace<UIUserList>(tab);
					}
					else
					{
						auto cur_tab = userlist->get_tab();
						auto is_active = userlist->is_active();

						if (cur_tab == tab)
						{
							if (is_active)
								userlist->deactivate();
							else
								userlist->makeactive();
						}
						else
						{
							if (!is_active)
								userlist->makeactive();

							userlist->change_tab(tab);
						}
					}
				}
				break;
				case KeyAction::Id::WORLDMAP:
					emplace<UIWorldMap>();
					break;
				case KeyAction::Id::MAPLECHAT:
				{
					auto chat = UI::get().get_element<UIChat>();

					if (!chat)
						emplace<UIChat>();
					else if (chat && !chat->is_active())
						chat->makeactive();
				}
				break;
				case KeyAction::Id::MINIMAP:
					if (auto minimap = UI::get().get_element<UIMiniMap>())
						minimap->send_key(action, pressed, escape);

					break;
				case KeyAction::Id::QUESTLOG:
					emplace<UIQuestLog>(
						Stage::get().get_player().get_quests()
						);
					break;
				case KeyAction::Id::MENU:
					if (auto statusbar = UI::get().get_element<UIStatusbar>())
						statusbar->toggle_menu();

					break;
				case KeyAction::Id::QUICKSLOTS:
					if (auto statusbar = UI::get().get_element<UIStatusbar>())
						statusbar->toggle_qs();

					break;
				case KeyAction::Id::TOGGLECHAT:
					if (auto chatbar = UI::get().get_element<UIChatbar>())
						chatbar->toggle_chat();

					break;
				case KeyAction::Id::KEYBINDINGS:
				{
					auto keyconfig = UI::get().get_element<UIKeyConfig>();

					if (!keyconfig || !keyconfig->is_active())
						emplace<UIKeyConfig>();
					else if (keyconfig && keyconfig->is_active())
						keyconfig->close();

					break;
				}
				case KeyAction::Id::MAINMENU:
					if (auto statusbar = UI::get().get_element<UIStatusbar>())
						statusbar->send_key(action, pressed, escape);

					break;
				case KeyAction::Id::EVENT:
					emplace<UIEvent>();
					break;
				case KeyAction::Id::CHANGECHANNEL:
					emplace<UIChannel>();
					break;
				default:
					std::cout << "Action (" << action << ") not handled!" << std::endl;
					break;
				}
			}
			break;
		case KeyType::Id::ACTION:
		case KeyType::Id::FACE:
		case KeyType::Id::ITEM:
		case KeyType::Id::SKILL:
			Stage::get().send_key(type, action, pressed);
			break;
		}
	}

	Cursor::State UIStateGame::send_cursor(Cursor::State mst, Point<int16_t> pos)
	{
		if (draggedicon)
		{
			switch (mst)
			{
			case Cursor::State::CLICKING:
				if (drop_icon(*draggedicon, pos))
				{
					draggedicon->reset();
					draggedicon = {};
				}

				return mst;
			default:
				return Cursor::State::GRABBING;
			}
		}
		else
		{
			bool clicked = mst == Cursor::State::CLICKING || mst == Cursor::State::VSCROLLIDLE;

			if (UIElement * focusedelement = get(focused))
			{
				if (focusedelement->is_active())
				{
					return focusedelement->send_cursor(clicked, pos);
				}
				else
				{
					focused = UIElement::Type::NONE;

					return mst;
				}
			}
			else
			{
				UIElement* front = nullptr;
				UIElement::Type fronttype = UIElement::Type::NONE;

				for (auto& type : elementorder)
				{
					auto& element = elements[type];
					bool found = false;

					if (element && element->is_active())
					{
						if (element->is_in_range(pos))
							found = true;
						else
							found = element->remove_cursor(clicked, pos);

						if (found)
						{
							if (front)
								element->remove_cursor(false, pos);

							front = element.get();
							fronttype = type;
						}
					}
				}

				if (fronttype != tooltipparent)
					clear_tooltip(tooltipparent);

				if (front)
				{
					if (clicked)
					{
						elementorder.remove(fronttype);
						elementorder.push_back(fronttype);
					}

					return front->send_cursor(clicked, pos);
				}
				else
				{
					return Stage::get().send_cursor(clicked, pos);
				}
			}
		}
	}

	void UIStateGame::send_scroll(double yoffset)
	{
		for (auto& type : elementorder)
		{
			auto& element = elements[type];

			if (element && element->is_active())
				element->send_scroll(yoffset);
		}
	}

	void UIStateGame::send_close()
	{
		UI::get().emplace<UIQuit>(stats);
	}

	void UIStateGame::drag_icon(Icon* drgic)
	{
		draggedicon = drgic;
	}

	void UIStateGame::clear_tooltip(Tooltip::Parent parent)
	{
		if (parent == tooltipparent)
		{
			eqtooltip.set_equip(Tooltip::Parent::NONE, 0);
			ittooltip.set_item(0);
			tetooltip.set_text("");
			tooltip = {};
			tooltipparent = Tooltip::Parent::NONE;
		}
	}

	void UIStateGame::show_equip(Tooltip::Parent parent, std::int16_t slot)
	{
		eqtooltip.set_equip(parent, slot);

		if (slot)
		{
			tooltip = eqtooltip;
			tooltipparent = parent;
		}
	}

	void UIStateGame::show_item(Tooltip::Parent parent, std::int32_t itemid)
	{
		ittooltip.set_item(itemid);

		if (itemid)
		{
			tooltip = ittooltip;
			tooltipparent = parent;
		}
	}

	void UIStateGame::show_skill(Tooltip::Parent parent, std::int32_t skill_id, std::int32_t level, std::int32_t masterlevel, int64_t expiration)
	{
		sktooltip.set_skill(skill_id, level, masterlevel, expiration);

		if (skill_id)
		{
			tooltip = sktooltip;
			tooltipparent = parent;
		}
	}

	void UIStateGame::show_text(Tooltip::Parent parent, std::string text)
	{
		tetooltip.set_text(text);

		if (!text.empty())
		{
			tooltip = tetooltip;
			tooltipparent = parent;
		}
	}

	template <class T, typename...Args>
	void UIStateGame::emplace(Args&& ...args)
	{
		if (auto iter = pre_add(T::TYPE, T::TOGGLED, T::FOCUSED))
		{
			(*iter).second = std::make_unique<T>(
				std::forward<Args>(args)...
				);
		}
	}

	UIState::Iterator UIStateGame::pre_add(UIElement::Type type, bool is_toggled, bool is_focused)
	{
		auto& element = elements[type];

		if (element && is_toggled)
		{
			elementorder.remove(type);
			elementorder.push_back(type);
			element->toggle_active();

			return elements.end();
		}
		else
		{
			remove(type);
			elementorder.push_back(type);

			if (is_focused)
				focused = type;

			return elements.find(type);
		}
	}

	void UIStateGame::remove(UIElement::Type type)
	{
		if (type == focused)
			focused = UIElement::Type::NONE;

		if (type == tooltipparent)
			clear_tooltip(tooltipparent);

		elementorder.remove(type);

		if (auto & element = elements[type])
		{
			element->deactivate();
			element.release();
		}
	}

	UIElement* UIStateGame::get(UIElement::Type type)
	{
		return elements[type].get();
	}

	UIElement* UIStateGame::get_front(std::list<UIElement::Type> types)
	{
		auto begin = elementorder.rbegin();
		auto end = elementorder.rend();

		for (auto iter = begin; iter != end; ++iter)
		{
			if (std::find(types.begin(), types.end(), *iter) != types.end())
			{
				auto& element = elements[*iter];

				if (element && element->is_active())
					return element.get();
			}
		}

		return nullptr;
	}

	UIElement* UIStateGame::get_front(Point<int16_t> pos)
	{
		auto begin = elementorder.rbegin();
		auto end = elementorder.rend();

		for (auto iter = begin; iter != end; ++iter)
		{
			auto& element = elements[*iter];

			if (element && element->is_active() && element->is_in_range(pos))
				return element.get();
		}

		return nullptr;
	}

	int64_t UIStateGame::get_uptime()
	{
		return ContinuousTimer::get().stop(start);
	}

	std::uint16_t UIStateGame::get_uplevel()
	{
		return levelBefore;
	}

	int64_t UIStateGame::get_upexp()
	{
		return expBefore;
	}
}