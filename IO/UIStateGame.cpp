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

#include "UITypes/UIBuffList.h"
#include "UITypes/UIChannel.h"
#include "UITypes/UICharInfo.h"
#include "UITypes/UIChat.h"
#include "UITypes/UIChatBar.h"
#include "UITypes/UIEquipInventory.h"
#include "UITypes/UIEvent.h"
#include "UITypes/UIItemInventory.h"
#include "UITypes/UIKeyConfig.h"
#include "UITypes/UIMiniMap.h"
#include "UITypes/UIQuestLog.h"
#include "UITypes/UIQuit.h"
#include "UITypes/UIShop.h"
#include "UITypes/UISkillBook.h"
#include "UITypes/UIStatsInfo.h"
#include "UITypes/UIStatusBar.h"
#include "UITypes/UIStatusMessenger.h"
#include "UITypes/UIUserList.h"
#include "UITypes/UIWorldMap.h"

#include "../Net/Packets/GameplayPackets.h"

namespace ms
{
	UIStateGame::UIStateGame() : stats(Stage::get().get_player().get_stats()), dragged(nullptr)
	{
		focused = UIElement::Type::NONE;
		tooltipparent = Tooltip::Parent::NONE;

		const CharLook& look = Stage::get().get_player().get_look();
		const Inventory& inventory = Stage::get().get_player().get_inventory();

		emplace<UIStatusMessenger>();
		emplace<UIStatusBar>(stats);
		emplace<UIChatBar>();
		emplace<UIMiniMap>(stats);
		emplace<UIBuffList>();
		emplace<UIShop>(look, inventory);

		VWIDTH = Constants::Constants::get().get_viewwidth();
		VHEIGHT = Constants::Constants::get().get_viewheight();
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
		bool update_screen = false;
		int16_t new_width = Constants::Constants::get().get_viewwidth();
		int16_t new_height = Constants::Constants::get().get_viewheight();

		if (VWIDTH != new_width || VHEIGHT != new_height)
		{
			update_screen = true;
			VWIDTH = new_width;
			VHEIGHT = new_height;

			UI::get().remove(UIElement::Type::STATUSBAR);

			const CharStats& stats = Stage::get().get_player().get_stats();
			emplace<UIStatusBar>(stats);
		}

		for (auto& type : elementorder)
		{
			auto& element = elements[type];

			if (element && element->is_active())
			{
				element->update();

				if (update_screen)
					element->update_screen(new_width, new_height);
			}
		}
	}

	bool UIStateGame::drop_icon(const Icon& icon, Point<int16_t> cursor_position)
	{
		if (UIElement* front = get_front(cursor_position))
			return front->send_icon(icon, cursor_position);
		else
			icon.drop_on_stage();

		return true;
	}

	void UIStateGame::remove_icon()
	{
		draggedicon->reset();
		draggedicon = {};
	}

	void UIStateGame::remove_cursors()
	{
		for (auto type : elementorder)
		{
			auto element = elements[type].get();

			if (element && element->is_active())
				element->remove_cursor();
		}
	}

	void UIStateGame::remove_cursor(UIElement::Type t)
	{
		for (auto type : elementorder)
		{
			auto element = elements[type].get();

			if (element && element->is_active() && element->get_type() != t)
				element->remove_cursor();
		}
	}

	void UIStateGame::doubleclick(Point<int16_t> pos)
	{
		if (UIElement* front = get_front(pos))
			front->doubleclick(pos);
	}

	void UIStateGame::rightclick(Point<int16_t> pos)
	{
		if (UIElement* front = get_front(pos))
			front->rightclick(pos);
	}

	void UIStateGame::send_key(KeyType::Id type, int32_t action, bool pressed, bool escape)
	{
		if (UIElement* focusedelement = get(focused))
		{
			if (focusedelement->is_active())
			{
				return focusedelement->send_key(action, pressed, escape);
			}
			else
			{
				focused = UIElement::NONE;

				return;
			}
		}
		else
		{
			switch (type)
			{
				case KeyType::Id::MENU:
				{
					if (pressed)
					{
						switch (action)
						{
							case KeyAction::Id::EQUIPMENT:
							{
								emplace<UIEquipInventory>(
									Stage::get().get_player().get_inventory()
									);

								break;
							}
							case KeyAction::Id::ITEMS:
							{
								emplace<UIItemInventory>(
									Stage::get().get_player().get_inventory()
									);

								break;
							}
							case KeyAction::Id::STATS:
							{
								emplace<UIStatsInfo>(
									Stage::get().get_player().get_stats()
									);

								break;
							}
							case KeyAction::Id::SKILLS:
							{
								emplace<UISkillBook>(
									Stage::get().get_player().get_stats(),
									Stage::get().get_player().get_skills()
									);

								break;
							}
							case KeyAction::Id::FRIENDS:
							case KeyAction::Id::PARTY:
							case KeyAction::Id::BOSSPARTY:
							{
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

								auto userlist = UI::get().get_element<UIUserList>();

								if (userlist && userlist->get_tab() != tab && userlist->is_active())
								{
									userlist->change_tab(tab);
								}
								else
								{
									emplace<UIUserList>(tab);

									if (userlist && userlist->get_tab() != tab && userlist->is_active())
										userlist->change_tab(tab);
								}

								break;
							}
							case KeyAction::Id::WORLDMAP:
							{
								emplace<UIWorldMap>();
								break;
							}
							case KeyAction::Id::MAPLECHAT:
							{
								auto chat = UI::get().get_element<UIChat>();

								if (!chat || !chat->is_active())
									emplace<UIChat>();

								break;
							}
							case KeyAction::Id::MINIMAP:
							{
								if (auto minimap = UI::get().get_element<UIMiniMap>())
									minimap->send_key(action, pressed, escape);

								break;
							}
							case KeyAction::Id::QUESTLOG:
							{
								emplace<UIQuestLog>(
									Stage::get().get_player().get_quests()
									);

								break;
							}
							case KeyAction::Id::KEYBINDINGS:
							{
								auto keyconfig = UI::get().get_element<UIKeyConfig>();

								if (!keyconfig || !keyconfig->is_active())
								{
									emplace<UIKeyConfig>(
										Stage::get().get_player().get_inventory(),
										Stage::get().get_player().get_skills()
										);
								}
								else if (keyconfig && keyconfig->is_active())
								{
									keyconfig->close();
								}

								break;
							}
							case KeyAction::Id::SAY:
							case KeyAction::Id::PARTYCHAT:
							case KeyAction::Id::FRIENDSCHAT:
							case KeyAction::Id::GUILDCHAT:
							case KeyAction::Id::ALLIANCECHAT:
							case KeyAction::Id::TOSPOUSE:
							{
								if (auto chatbar = UI::get().get_element<UIChatBar>())
									chatbar->change_target(action);

								break;
							}
							case KeyAction::Id::MENU:
							{
								if (auto statusbar = UI::get().get_element<UIStatusBar>())
									statusbar->toggle_menu();

								break;
							}
							case KeyAction::Id::QUICKSLOTS:
							{
								if (auto statusbar = UI::get().get_element<UIStatusBar>())
									statusbar->toggle_qs();

								break;
							}
							case KeyAction::Id::TOGGLECHAT:
							{
								if (auto chatbar = UI::get().get_element<UIChatBar>())
								{
									if (chatbar->has_input())
										chatbar->send_key(action, pressed, escape);
									else
										chatbar->toggle_view();
								}

								break;
							}
							case KeyAction::Id::CASHSHOP:
							{
								EnterCashShopPacket().dispatch();
								break;
							}
							case KeyAction::Id::EVENT:
							{
								emplace<UIEvent>();
								break;
							}
							case KeyAction::Id::CHARINFO:
							{
								emplace<UICharInfo>(
									Stage::get().get_player().get_oid()
									);

								break;
							}
							case KeyAction::Id::CHANGECHANNEL:
							{
								emplace<UIChannel>();
								break;
							}
							case KeyAction::Id::MAINMENU:
							{
								if (auto statusbar = UI::get().get_element<UIStatusBar>())
									statusbar->send_key(action, pressed, escape);

								break;
							}
							default:
							{
								std::cout << "Unknown KeyAction::Id action: [" << action << "]" << std::endl;
								break;
							}
						}
					}

					break;
				}
				case KeyType::Id::ACTION:
				case KeyType::Id::FACE:
				case KeyType::Id::ITEM:
				case KeyType::Id::SKILL:
				{
					Stage::get().send_key(type, action, pressed);
					break;
				}
			}
		}
	}

	Cursor::State UIStateGame::send_cursor(Point<int16_t> cursor_position, Cursor::State cursor_state)
	{
		if (draggedicon)
		{
			if (cursor_state == Cursor::State::CLICKING)
			{
				if (drop_icon(*draggedicon, cursor_position))
					remove_icon();

				return cursor_state;
			}

			return Cursor::State::GRABBING;
		}
		else
		{
			bool clicked = cursor_state == Cursor::State::CLICKING || cursor_state == Cursor::State::VSCROLLIDLE || (dragged && (cursor_state == Cursor::State::CHATBARMOVE || cursor_state == Cursor::State::CHATBARVDRAG || cursor_state == Cursor::State::CHATBARHDRAG || cursor_state == Cursor::State::CHATBARBLTRDRAG || cursor_state == Cursor::State::CHATBARBRTLDRAG));

			if (auto focusedelement = get(focused))
			{
				if (focusedelement->is_active())
				{
					remove_cursor(focusedelement->get_type());

					return focusedelement->send_cursor(clicked, cursor_position);
				}
				else
				{
					focused = UIElement::Type::NONE;

					return cursor_state;
				}
			}
			else
			{
				if (!clicked)
				{
					dragged = nullptr;

					if (auto front = get_front(cursor_position))
					{
						UIElement::Type front_type = front->get_type();

						if (tooltipparent != UIElement::Type::NONE)
							if (front_type != tooltipparent)
								clear_tooltip(tooltipparent);

						remove_cursor(front_type);

						return front->send_cursor(clicked, cursor_position);
					}
					else
					{
						remove_cursors();

						return Stage::get().send_cursor(clicked, cursor_position);
					}
				}
				else
				{
					if (!dragged)
					{
						UIElement::Type drag_element_type = UIElement::Type::NONE;

						for (auto iter = elementorder.rbegin(); iter != elementorder.rend(); ++iter)
						{
							auto& element = elements[*iter];

							if (element && element->is_active() && element->is_in_range(cursor_position))
							{
								dragged = element.get();
								drag_element_type = *iter;
								break;
							}
						}

						if (drag_element_type != UIElement::Type::NONE)
						{
							elementorder.remove(drag_element_type);
							elementorder.push_back(drag_element_type);
						}
					}

					if (dragged)
						return dragged->send_cursor(clicked, cursor_position);
					else
						return Stage::get().send_cursor(clicked, cursor_position);
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
			matooltip.reset();
			tooltip = {};
			tooltipparent = Tooltip::Parent::NONE;
		}
	}

	void UIStateGame::show_equip(Tooltip::Parent parent, int16_t slot)
	{
		eqtooltip.set_equip(parent, slot);

		if (slot)
		{
			tooltip = eqtooltip;
			tooltipparent = parent;
		}
	}

	void UIStateGame::show_item(Tooltip::Parent parent, int32_t itemid)
	{
		ittooltip.set_item(itemid);

		if (itemid)
		{
			tooltip = ittooltip;
			tooltipparent = parent;
		}
	}

	void UIStateGame::show_skill(Tooltip::Parent parent, int32_t skill_id, int32_t level, int32_t masterlevel, int64_t expiration)
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

	void UIStateGame::show_map(Tooltip::Parent parent, std::string title, std::string description, int32_t mapid, bool bolded, bool portal)
	{
		matooltip.set_title(parent, title, bolded);
		matooltip.set_desc(description);
		matooltip.set_mapid(mapid, portal);

		if (!title.empty())
		{
			tooltip = matooltip;
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

			auto silent_types = {
				UIElement::Type::STATUSMESSENGER,
				UIElement::Type::STATUSBAR,
				UIElement::Type::CHATBAR,
				UIElement::Type::MINIMAP,
				UIElement::Type::BUFFLIST,
				UIElement::Type::NPCTALK,
				UIElement::Type::SHOP
			};

			if (std::find(silent_types.begin(), silent_types.end(), T::TYPE) == silent_types.end())
			{
				if (T::TYPE == UIElement::Type::WORLDMAP)
					Sound(Sound::Name::WORLDMAPOPEN).play();
				else
					Sound(Sound::Name::MENUUP).play();

				UI::get().send_cursor(false);
			}
		}
	}

	UIState::Iterator UIStateGame::pre_add(UIElement::Type type, bool is_toggled, bool is_focused)
	{
		auto& element = elements[type];

		if (element && is_toggled)
		{
			elementorder.remove(type);
			elementorder.push_back(type);

			bool active = element->is_active();

			element->toggle_active();

			if (active != element->is_active())
			{
				if (element->is_active())
				{
					if (type == UIElement::Type::WORLDMAP)
						Sound(Sound::Name::WORLDMAPOPEN).play();
					else
						Sound(Sound::Name::MENUUP).play();

					UI::get().send_cursor(false);
				}
				else
				{
					if (type == UIElement::Type::WORLDMAP)
						Sound(Sound::Name::WORLDMAPCLOSE).play();
					else
						Sound(Sound::Name::MENUDOWN).play();

					element->remove_cursor();

					if (draggedicon)
						if (element->get_type() == icon_map[draggedicon.get()->get_type()])
							remove_icon();

					UI::get().send_cursor(false);
				}
			}

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

		if (auto& element = elements[type])
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

		for (auto& iter = begin; iter != end; ++iter)
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

	UIElement* UIStateGame::get_front(Point<int16_t> cursor_position)
	{
		auto begin = elementorder.rbegin();
		auto end = elementorder.rend();

		for (auto& iter = begin; iter != end; ++iter)
		{
			auto& element = elements[*iter];

			if (element && element->is_active() && element->is_in_range(cursor_position))
				return element.get();
		}

		return nullptr;
	}
}