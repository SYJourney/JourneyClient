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
#include "UILoginNotice.h"

#include "../UI.h"

#include "../Components/MapleButton.h"
#include "../../Audio/Audio.h"

#include <nlnx/nx.hpp>

namespace ms
{
	UILoginNotice::UILoginNotice(std::uint16_t message, std::function<void()> okhandler, std::function<void()> cancelhandler) : okhandler(okhandler), cancelhandler(cancelhandler)
	{
		multiple = false;

		nl::node Notice = nl::nx::ui["Login.img"]["Notice"];
		nl::node backgrnd;

		switch (message)
		{
		case Message::NAME_IN_USE:
		case Message::ILLEGAL_NAME:
		case Message::BLOCKED_ID:
		case Message::INCORRECT_PIC:
			backgrnd = Notice["backgrnd"]["1"];
			break;
		default:
			backgrnd = Notice["backgrnd"]["0"];
			break;
		}

		sprites.emplace_back(backgrnd);
		sprites.emplace_back(Notice["text"][message], Point<int16_t>(17, 13));

		if (message == Message::DELETE_CONFIRMATION)
		{
			multiple = true;

			buttons[Buttons::YES] = std::make_unique<MapleButton>(Notice["BtYes"], Point<int16_t>(70, 106));
			buttons[Buttons::NO] = std::make_unique<MapleButton>(Notice["BtNo"], Point<int16_t>(130, 106));
		}
		else
		{
			buttons[Buttons::YES] = std::make_unique<MapleButton>(Notice["BtYes"], Point<int16_t>(100, 106));
		}

		position = Point<int16_t>(275, 199);
		dimension = Texture(backgrnd).get_dimensions();
	}

	UILoginNotice::UILoginNotice(std::uint16_t message, std::function<void()> okhandler) : UILoginNotice(message, okhandler, []() {}) {}
	UILoginNotice::UILoginNotice(std::uint16_t message) : UILoginNotice(message, []() {}) {}

	void UILoginNotice::draw(float alpha) const
	{
		UIElement::draw(alpha);
	}

	void UILoginNotice::send_key(std::int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
			{
				if (!multiple)
					okhandler();
				else
					cancelhandler();

				deactivate();
			}
			else if (keycode == KeyAction::RETURN)
			{
				okhandler();
				deactivate();
			}
		}
	}

	Button::State UILoginNotice::button_pressed(std::uint16_t buttonid)
	{
		if (buttonid == Buttons::YES)
			okhandler();
		else if (buttonid == Buttons::NO)
			cancelhandler();

		deactivate();

		return Button::State::NORMAL;
	}

	UIQuitConfirm::UIQuitConfirm()
	{
		nl::node notice = nl::nx::ui["Login.img"]["Notice"];

		background = notice["backgrnd"]["0"];
		text = { notice["text"][UILoginNotice::Message::CONFIRM_EXIT], { 17, 13 } };

		buttons[BT_OK] = std::make_unique<MapleButton>(notice["BtYes"], Point<int16_t>(70, 106));
		buttons[BT_CANCEL] = std::make_unique<MapleButton>(notice["BtNo"], Point<int16_t>(130, 106));

		position = { 275, 199 };
		dimension = { 362, 219 };
		active = true;
	}

	void UIQuitConfirm::draw(float alpha) const
	{
		background.draw(position);
		text.draw(position, alpha);

		UIElement::draw(alpha);
	}

	void UIQuitConfirm::send_key(std::int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
			{
				active = false;
			}
			else if (keycode == KeyAction::RETURN)
			{
				UI::get().quit();
				active = false;
			}
		}
	}

	Button::State UIQuitConfirm::button_pressed(std::uint16_t buttonid)
	{
		if (buttonid == BT_OK)
			UI::get().quit();

		active = false;
		return Button::PRESSED;
	}

	UIClassConfirm::UIClassConfirm(std::uint8_t selected_class, bool unavailable, std::function<void()> okhandler) : okhandler(okhandler)
	{
		nl::node RaceSelect = nl::nx::ui["Login.img"]["RaceSelect_new"];
		nl::node type = unavailable ? RaceSelect["deny"] : RaceSelect["confirm"];
		nl::node backgrnd = type["backgrnd"];
		nl::node race = type["race"][selected_class];

		Point<int16_t> race_pos = Point<int16_t>(0, 0);

		// TODO: Calculate position based on width of race?
		switch (selected_class)
		{
		case Classes::RESISTANCE:
			race_pos = Point<int16_t>(57, 10);
			break;
		case Classes::EXPLORER:
			race_pos = Point<int16_t>(71, 10);
			break;
		case Classes::CYGNUSKNIGHTS:
			race_pos = Point<int16_t>(32, 10);
			break;
		case Classes::ARAN:
			race_pos = Point<int16_t>(95, 10);
			break;
		case Classes::EVAN:
			race_pos = Point<int16_t>(93, 10);
			break;
		case Classes::MERCEDES:
			race_pos = Point<int16_t>(65, 10);
			break;
		case Classes::DEMON:
			race_pos = Point<int16_t>(85, 10);
			break;
		case Classes::PHANTOM:
			race_pos = Point<int16_t>(71, 10);
			break;
		case Classes::DUALBLADE:
			race_pos = Point<int16_t>(54, 10);
			break;
		case Classes::MIHILE:
			race_pos = Point<int16_t>(85, 10);
			break;
		case Classes::LUMINOUS:
			race_pos = Point<int16_t>(66, 10);
			break;
		case Classes::KAISER:
			race_pos = Point<int16_t>(87, 10);
			break;
		case Classes::ANGELICBUSTER:
			race_pos = Point<int16_t>(41, 10);
			break;
		case Classes::CANNONEER:
			race_pos = Point<int16_t>(57, 10);
			break;
		case Classes::XENON:
			race_pos = Point<int16_t>(88, 10);
			break;
		case Classes::ZERO:
			break;
		case Classes::SHADE:
			race_pos = Point<int16_t>(86, 10);
			break;
		case Classes::JETT:
			race_pos = Point<int16_t>(101, 10);
			break;
		case Classes::HAYATO:
			race_pos = Point<int16_t>(81, 10);
			break;
		case Classes::KANNA:
			race_pos = Point<int16_t>(86, 10);
			break;
		case Classes::CHASE:
			break;
		case Classes::PINKBEAN:
			break;
		case Classes::KINESIS:
			race_pos = Point<int16_t>(84, 10);
			break;
		case Classes::CADENA:
			race_pos = Point<int16_t>(77, 10);
			break;
		case Classes::ILLIUM:
			race_pos = Point<int16_t>(92, 10);
			break;
		case Classes::ARK:
			race_pos = Point<int16_t>(100, 10);
			break;
		default:
			break;
		}

		sprites.emplace_back(backgrnd);
		sprites.emplace_back(race, race_pos + (Point<int16_t>)race["origin"]);

		if (unavailable)
		{
			buttons[Buttons::OK] = std::make_unique<MapleButton>(type["BtOK"]);
		}
		else
		{
			buttons[Buttons::OK] = std::make_unique<MapleButton>(type["BtOK"], Point<int16_t>(62, 107));
			buttons[Buttons::CANCEL] = std::make_unique<MapleButton>(type["BtCancel"], Point<int16_t>(137, 107));
		}

		position = Point<int16_t>(286, 179);
		dimension = Texture(backgrnd).get_dimensions();
	}

	void UIClassConfirm::draw(float inter) const
	{
		UIElement::draw(inter);
	}

	bool UIClassConfirm::remove_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		return false;
	}

	Cursor::State UIClassConfirm::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		for (auto& btit : buttons)
		{
			if (btit.second->is_active() && btit.second->bounds(position).contains(cursorpos))
			{
				if (btit.second->get_state() == Button::State::NORMAL)
				{
					Sound(Sound::Name::BUTTONOVER).play();

					btit.second->set_state(Button::State::MOUSEOVER);
				}
				else if (btit.second->get_state() == Button::State::MOUSEOVER)
				{
					if (clicked)
					{
						Sound(Sound::Name::BUTTONCLICK).play();

						btit.second->set_state(button_pressed(btit.first));
					}
				}
			}
			else if (btit.second->get_state() == Button::State::MOUSEOVER)
			{
				btit.second->set_state(Button::State::NORMAL);
			}
		}

		return Cursor::State::LEAF;
	}

	void UIClassConfirm::send_key(std::int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (escape)
				deactivate();
			else if (keycode == KeyAction::Id::RETURN)
				button_pressed(Buttons::OK);
		}
	}

	Button::State UIClassConfirm::button_pressed(std::uint16_t buttonid)
	{
		deactivate();

		if (buttonid == Buttons::OK)
			okhandler();

		return Button::State::NORMAL;
	}

	UIKeySelect::UIKeySelect(std::function<void(bool)> oh, bool l) : okhandler(oh), login(l)
	{
		nl::node KeyType = nl::nx::ui["UIWindow2.img"]["KeyConfig"]["KeyType"];
		nl::node backgrnd = KeyType["backgrnd"];

		sprites.emplace_back(backgrnd);

		buttons[Buttons::TYPEA] = std::make_unique<MapleButton>(KeyType["btTypeA"]);
		buttons[Buttons::TYPEB] = std::make_unique<MapleButton>(KeyType["btTypeB"], Point<int16_t>(1, 1));

		position = Point<int16_t>(181, 135);
		dimension = Texture(backgrnd).get_dimensions();
	}

	void UIKeySelect::draw(float alpha) const
	{
		UIElement::draw(alpha);
	}

	void UIKeySelect::send_key(std::int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && !login)
			if (escape || keycode == KeyAction::Id::RETURN)
				deactivate();
	}

	Button::State UIKeySelect::button_pressed(std::uint16_t buttonid)
	{
		bool alternate = (buttonid == Buttons::TYPEA) ? false : true;

		if (alternate)
			buttons[Buttons::TYPEA]->set_state(Button::State::DISABLED);
		else
			buttons[Buttons::TYPEB]->set_state(Button::State::DISABLED);

		auto onok = [&, alternate]()
		{
			okhandler(alternate);
			deactivate();
		};

		UI::get().emplace<UIKeyConfirm>(alternate, onok, login);

		return Button::State::DISABLED;
	}

	UIKeyConfirm::UIKeyConfirm(bool alternate, std::function<void()> oh, bool l) : okhandler(oh), login(l)
	{
		nl::node alert = nl::nx::ui["UIWindow2.img"]["KeyConfig"]["KeyType"]["alert"];
		nl::node background = alternate ? alert["alternate"] : alert["default"];

		sprites.emplace_back(background);

		buttons[Buttons::OK] = std::make_unique<MapleButton>(alert["btOk"]);

		position = Point<int16_t>(276, 219);
		dimension = Texture(background).get_dimensions();
	}

	void UIKeyConfirm::draw(float alpha) const
	{
		UIElement::draw(alpha);
	}

	void UIKeyConfirm::send_key(std::int32_t keycode, bool pressed, bool escape)
	{
		if (pressed)
		{
			if (keycode == KeyAction::Id::RETURN)
				confirm();
			else if (!login && escape)
				UI::get().remove(UIElement::Type::LOGINNOTICE);
		}
	}

	Button::State UIKeyConfirm::button_pressed(std::uint16_t buttonid)
	{
		confirm();

		return Button::State::NORMAL;
	}

	void UIKeyConfirm::confirm()
	{
		okhandler();
		deactivate();

		UI::get().remove(UIElement::Type::LOGINNOTICE);
	}
}
