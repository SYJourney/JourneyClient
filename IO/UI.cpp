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
#include "UI.h"
#include "UIStateLogin.h"
#include "UIStateGame.h"
#include "Window.h"

#include "../IO/UITypes/UIStatsinfo.h"
#include "../IO/UITypes/UIItemInventory.h"
#include "../IO/UITypes/UIEquipInventory.h"
#include "../IO/UITypes/UISkillbook.h"
#include "../IO/UITypes/UIQuestLog.h"
#include "../IO/UITypes/UIWorldMap.h"
#include "../IO/UITypes/UIUserList.h"
#include "../IO/UITypes/UIChatbar.h"
#include "../IO/UITypes/UIStatusbar.h"
#include "../IO/UITypes/UINotice.h"
#include "../IO/UITypes/UIShop.h"
#include "../IO/UITypes/UIChannel.h"
#include "../IO/UITypes/UIJoypad.h"
#include "../IO/UITypes/UIEvent.h"
#include "../IO/UITypes/UIChat.h"
#include "../IO/UITypes/UIKeyConfig.h"
#include "../IO/UITypes/UIOptionMenu.h"
#include "../IO/UITypes/UIQuit.h"

namespace ms
{
	UI::UI()
	{
		state = std::make_unique<UIStateNull>();
		enabled = true;
	}

	void UI::init()
	{
		cursor.init();
		change_state(State::LOGIN);
	}

	void UI::draw(float alpha) const
	{
		state->draw(alpha, cursor.get_position());

		scrollingnotice.draw(alpha);
		cursor.draw(alpha);
	}

	void UI::update()
	{
		state->update();

		scrollingnotice.update();
		cursor.update();
	}

	void UI::enable()
	{
		enabled = true;
	}

	void UI::disable()
	{
		enabled = false;
	}

	void UI::change_state(State id)
	{
		switch (id)
		{
		case State::LOGIN:
			state = std::make_unique<UIStateLogin>();
			break;
		case State::GAME:
			state = std::make_unique<UIStateGame>();
			break;
		}
	}

	void UI::quit()
	{
		quitted = true;
	}

	bool UI::not_quitted() const
	{
		return !quitted;
	}

	void UI::send_cursor(Point<int16_t> cursorpos, Cursor::State cursorstate)
	{
		Cursor::State nextstate = state->send_cursor(cursorstate, cursorpos);
		cursor.set_state(nextstate);
		cursor.set_position(cursorpos);
	}

	void UI::send_focus(int focused)
	{
		if (focused)
		{
			// The window gained input focus
			std::uint8_t sfxvolume = Setting<SFXVolume>::get().load();
			Sound::set_sfxvolume(sfxvolume);

			std::uint8_t bgmvolume = Setting<BGMVolume>::get().load();
			Music::set_bgmvolume(bgmvolume);
		}
		else
		{
			// The window lost input focus
			Sound::set_sfxvolume(0);
			Music::set_bgmvolume(0);
		}
	}

	void UI::send_scroll(double yoffset)
	{
		state->send_scroll(yoffset);
	}

	void UI::send_close()
	{
		state->send_close();
	}

	void UI::send_cursor(bool pressed)
	{
		Cursor::State cursorstate = (pressed && enabled) ? Cursor::State::CLICKING : Cursor::State::IDLE;
		Point<int16_t> cursorpos = cursor.get_position();
		send_cursor(cursorpos, cursorstate);

		if (focusedtextfield && pressed)
		{
			Cursor::State tstate = focusedtextfield->send_cursor(cursorpos, pressed);

			switch (tstate)
			{
			case Cursor::State::IDLE:
				focusedtextfield = {};
				break;
			}
		}
	}

	void UI::send_cursor(Point<int16_t> pos)
	{
		send_cursor(pos, cursor.get_state());
	}

	void UI::rightclick()
	{
		Point<int16_t> pos = cursor.get_position();
		state->rightclick(pos);
	}

	void UI::doubleclick()
	{
		Point<int16_t> pos = cursor.get_position();
		state->doubleclick(pos);
	}

	void UI::send_key(std::int32_t keycode, bool pressed)
	{
		if ((is_key_down[GLFW_KEY_LEFT_ALT] || is_key_down[GLFW_KEY_RIGHT_ALT]) && (is_key_down[GLFW_KEY_ENTER] || is_key_down[GLFW_KEY_KP_ENTER]))
		{
			Window::get().toggle_fullscreen();

			is_key_down[GLFW_KEY_LEFT_ALT] = false;
			is_key_down[GLFW_KEY_RIGHT_ALT] = false;
			is_key_down[GLFW_KEY_ENTER] = false;
			is_key_down[GLFW_KEY_KP_ENTER] = false;

			return;
		}

		if (is_key_down[keyboard.capslockcode()])
			caps_lock_enabled = !caps_lock_enabled;

		if (focusedtextfield)
		{
			bool ctrl = is_key_down[keyboard.leftctrlcode()] || is_key_down[keyboard.rightctrlcode()];

			if (ctrl)
			{
				if (!pressed)
				{
					KeyAction::Id action = keyboard.get_ctrl_action(keycode);

					switch (action)
					{
					case KeyAction::Id::COPY:
						Window::get().setclipboard(focusedtextfield->get_text());
						break;
					case KeyAction::Id::PASTE:
						focusedtextfield->add_string(Window::get().getclipboard());
						break;
					}
				}
			}
			else
			{
				bool shift = is_key_down[keyboard.leftshiftcode()] || is_key_down[keyboard.rightshiftcode()] || caps_lock_enabled;
				Keyboard::Mapping mapping = keyboard.get_text_mapping(keycode, shift);
				focusedtextfield->send_key(mapping.type, mapping.action, pressed);
			}
		}
		else
		{
			Keyboard::Mapping mapping = keyboard.get_mapping(keycode);

			bool sent = false;
			std::list<UIElement::Type> types;

			bool escape = keycode == GLFW_KEY_ESCAPE;
			bool tab = keycode == GLFW_KEY_TAB;
			bool enter = keycode == GLFW_KEY_ENTER || keycode == GLFW_KEY_KP_ENTER;
			bool up_down = keycode == GLFW_KEY_UP || keycode == GLFW_KEY_DOWN;
			bool left_right = keycode == GLFW_KEY_LEFT || keycode == GLFW_KEY_RIGHT;
			bool arrows = up_down || left_right;

			auto statusbar = UI::get().get_element<UIStatusbar>();
			auto channel = UI::get().get_element<UIChannel>();
			auto worldmap = UI::get().get_element<UIWorldMap>();
			auto optionmenu = UI::get().get_element<UIOptionMenu>();
			auto shop = UI::get().get_element<UIShop>();
			auto joypad = UI::get().get_element<UIJoypad>();
			auto rank = UI::get().get_element<UIRank>();
			auto quit = UI::get().get_element<UIQuit>();
			//auto report = UI::get().get_element<UIReport>();
			//auto whisper = UI::get().get_element<UIWhisper>();

			if (statusbar && statusbar->is_menu_active())
			{
				statusbar->send_key(mapping.action, pressed, escape);
				sent = true;
			}
			else if (channel && channel->is_active())
			{
				channel->send_key(mapping.action, pressed, escape);
				sent = true;
			}
			else if (worldmap && worldmap->is_active() && mapping.action != KeyAction::Id::WORLDMAP)
			{
				worldmap->send_key(mapping.action, pressed, escape);
				sent = true;
			}
			else if (optionmenu && optionmenu->is_active())
			{
				optionmenu->send_key(mapping.action, pressed, escape);
				sent = true;
			}
			else if (shop && shop->is_active())
			{
				shop->send_key(mapping.action, pressed, escape);
				sent = true;
			}
			else if (joypad && joypad->is_active())
			{
				joypad->send_key(mapping.action, pressed, escape);
				sent = true;
			}
			else if (rank && rank->is_active())
			{
				rank->send_key(mapping.action, pressed, escape);
				sent = true;
			}
			else if (quit && quit->is_active())
			{
				quit->send_key(mapping.action, pressed, escape);
				sent = true;
			}
			else
			{
				// All
				if (escape || tab || enter || arrows)
				{
					// Login
					types.emplace_back(UIElement::Type::WORLDSELECT);
					types.emplace_back(UIElement::Type::CHARSELECT);
					types.emplace_back(UIElement::Type::RACESELECT);			// No tab
					types.emplace_back(UIElement::Type::CLASSCREATION);			// No tab (No arrows, but shouldn't send else where)
					types.emplace_back(UIElement::Type::LOGINNOTICE);			// No tab (No arrows, but shouldn't send else where)
					types.emplace_back(UIElement::Type::LOGINNOTICE_CONFIRM);	// No tab (No arrows, but shouldn't send else where)
					types.emplace_back(UIElement::Type::LOGINWAIT);				// No tab (No arrows, but shouldn't send else where)
				}

				if (escape)
				{
					// Login
					types.emplace_back(UIElement::Type::SOFTKEYBOARD);

					// Game
					types.emplace_back(UIElement::Type::NOTICE);
					types.emplace_back(UIElement::Type::KEYCONFIG);
					types.emplace_back(UIElement::Type::CHAT);
					types.emplace_back(UIElement::Type::EVENT);
					types.emplace_back(UIElement::Type::STATSINFO);
					types.emplace_back(UIElement::Type::ITEMINVENTORY);
					types.emplace_back(UIElement::Type::EQUIPINVENTORY);
					types.emplace_back(UIElement::Type::SKILLBOOK);
					types.emplace_back(UIElement::Type::QUESTLOG);
					types.emplace_back(UIElement::Type::USERLIST);
				}
				else if (enter)
				{
					// Login
					types.emplace_back(UIElement::Type::SOFTKEYBOARD);

					// Game
					types.emplace_back(UIElement::Type::NOTICE);
				}
				else if (tab)
				{
					// Game
					types.emplace_back(UIElement::Type::ITEMINVENTORY);
					types.emplace_back(UIElement::Type::SKILLBOOK);
					types.emplace_back(UIElement::Type::QUESTLOG);
					types.emplace_back(UIElement::Type::USERLIST);
				}

				if (types.size() > 0)
				{
					auto element = state->get_front(types);

					if (element && element != nullptr)
					{
						element->send_key(mapping.action, pressed, escape);
						sent = true;
					}
				}
			}

			if (!sent)
			{
				auto chatbar = UI::get().get_element<UIChatbar>();

				if (escape)
				{
					if (chatbar && chatbar->is_chatopen())
						chatbar->send_key(mapping.action, pressed, escape);
					else
						state->send_key(mapping.type, mapping.action, pressed, escape);
				}
				else if (enter)
				{
					if (chatbar)
						chatbar->send_key(mapping.action, pressed, escape);
					else
						state->send_key(mapping.type, mapping.action, pressed, escape);
				}
				else
				{
					state->send_key(mapping.type, mapping.action, pressed, escape);
				}
			}
		}

		is_key_down[keycode] = pressed;
	}

	void UI::set_scrollnotice(const std::string& notice)
	{
		scrollingnotice.setnotice(notice);
	}

	void UI::focus_textfield(Textfield* tofocus)
	{
		if (focusedtextfield)
		{
			focusedtextfield->set_state(Textfield::State::NORMAL);
		}

		focusedtextfield = tofocus;
	}

	void UI::remove_textfield()
	{
		focusedtextfield = {};
	}

	void UI::drag_icon(Icon* icon)
	{
		state->drag_icon(icon);
	}

	void UI::add_keymapping(std::uint8_t no, std::uint8_t type, std::int32_t action)
	{
		keyboard.assign(no, type, action);
	}

	void UI::clear_tooltip(Tooltip::Parent parent)
	{
		state->clear_tooltip(parent);
	}

	void UI::show_equip(Tooltip::Parent parent, std::int16_t slot)
	{
		state->show_equip(parent, slot);
	}

	void UI::show_item(Tooltip::Parent parent, std::int32_t item_id)
	{
		state->show_item(parent, item_id);
	}

	void UI::show_skill(Tooltip::Parent parent, std::int32_t skill_id,
		std::int32_t level, std::int32_t masterlevel, int64_t expiration) {

		state->show_skill(parent, skill_id, level, masterlevel, expiration);
	}

	void UI::show_text(Tooltip::Parent parent, std::string text)
	{
		state->show_text(parent, text);
	}

	Keyboard& UI::get_keyboard()
	{
		return keyboard;
	}

	int64_t UI::get_uptime()
	{
		return state->get_uptime();
	}

	std::uint16_t UI::get_uplevel()
	{
		return state->get_uplevel();
	}

	int64_t UI::get_upexp()
	{
		return state->get_upexp();
	}

	void UI::remove(UIElement::Type type)
	{
		focusedtextfield = {};
		state->remove(type);
	}
}