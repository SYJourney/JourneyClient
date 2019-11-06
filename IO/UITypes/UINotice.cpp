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
#include "UINotice.h"

#include "../UI.h"

#include "../Components/MapleButton.h"
#include "../../Audio/Audio.h"

#include <nlnx/nx.hpp>

namespace ms
{
	UINotice::UINotice(std::string message, NoticeType t, Text::Alignment a) : UIDragElement<PosNOTICE>(Point<int16_t>()), type(t), alignment(a)
	{
		nl::node src = nl::nx::ui["Basic.img"]["Notice6"];

		top = src["t"];
		center = src["c"];
		centerbox = src["c_box"];
		box = src["box"];
		box2 = src["box2"];
		bottom = src["s"];
		bottombox = src["s_box"];

		if (type == NoticeType::YESNO)
		{
			position.shift_y(-8);
			question = Text(Text::Font::A11M, alignment, Color::Name::WHITE, message, 200);
		}
		else if (type == NoticeType::ENTERNUMBER)
		{
			position.shift_y(-16);
			question = Text(Text::Font::A12M, Text::Alignment::LEFT, Color::Name::WHITE, message, 200);
		}
		else if (type == NoticeType::OK)
		{
			std::uint16_t maxwidth = top.width() - 6;

			position.shift_y(-8);
			question = Text(Text::Font::A11M, Text::Alignment::CENTER, Color::Name::WHITE, message, maxwidth);
		}

		height = question.height();
		dimension = Point<int16_t>(top.width(), top.height() + height + bottom.height());
		position = Point<int16_t>(position.x() - dimension.x() / 2, position.y() - dimension.y() / 2);
		dragarea = Point<int16_t>(dimension.x(), 20);

		if (type != NoticeType::ENTERNUMBER)
			Sound(Sound::Name::DLGNOTICE).play();
	}

	UINotice::UINotice(std::string message, NoticeType t) : UINotice(message, t, Text::Alignment::CENTER) {}

	void UINotice::draw(bool textfield) const
	{
		Point<int16_t> start = position;

		top.draw(start);
		start.shift_y(top.height());

		if (textfield)
		{
			center.draw(start);
			start.shift_y(center.height());
			centerbox.draw(start);
			start.shift_y(centerbox.height() - 1);
			box2.draw(start);
			start.shift_y(box2.height());
			box.draw(DrawArgument(start, Point<int16_t>(0, 29)));
			start.shift_y(29);

			question.draw(position + Point<int16_t>(13, 13));
		}
		else
		{
			std::int16_t pos_y = height >= 32 ? height : 32;

			center.draw(DrawArgument(start, Point<int16_t>(0, pos_y)));
			start.shift_y(pos_y);
			centerbox.draw(start);
			start.shift_y(centerbox.height());
			box.draw(start);
			start.shift_y(box.height());

			if (type == NoticeType::YESNO && alignment == Text::Alignment::LEFT)
				question.draw(position + Point<int16_t>(31, 14));
			else
				question.draw(position + Point<int16_t>(131, 14));
		}

		bottombox.draw(start);
	}

	void UINotice::send_key(std::int32_t keycode, bool pressed, bool escape)
	{
		if (pressed && (keycode == KeyAction::Id::RETURN || escape))
		{
			if (type == NoticeType::OK)
				UI::get().get_element<UIOk>()->send_key(keycode, pressed, escape);
			else if (type == NoticeType::YESNO)
				UI::get().get_element<UIYesNo>()->send_key(keycode, pressed, escape);
			else if (type == NoticeType::ENTERNUMBER)
				UI::get().get_element<UIEnterNumber>()->send_key(keycode, pressed, escape);
		}
	}

	std::int16_t UINotice::box2offset(bool textfield) const
	{
		std::int16_t offset = top.height() + centerbox.height() + box.height() + height - (textfield ? 0 : 16);

		if (type == NoticeType::OK)
			if (height < 34)
				offset += 15;

		return offset;
	}

	UIYesNo::UIYesNo(std::string message, std::function<void(bool yes)> yh, Text::Alignment alignment) : UINotice(message, NoticeType::YESNO, alignment)
	{
		yesnohandler = yh;

		std::int16_t belowtext = box2offset(false);

		nl::node src = nl::nx::ui["Basic.img"];

		buttons[Buttons::YES] = std::make_unique<MapleButton>(src["BtOK4"], Point<int16_t>(156, belowtext));
		buttons[Buttons::NO] = std::make_unique<MapleButton>(src["BtCancel4"], Point<int16_t>(198, belowtext));
	}

	UIYesNo::UIYesNo(std::string message, std::function<void(bool yes)> yesnohandler) : UIYesNo(message, yesnohandler, Text::Alignment::CENTER) {}

	void UIYesNo::draw(float alpha) const
	{
		UINotice::draw(false);
		UIElement::draw(alpha);
	}

	void UIYesNo::send_key(std::int32_t keycode, bool pressed, bool escape)
	{
		if (keycode == KeyAction::Id::RETURN)
		{
			yesnohandler(true);
			active = false;
		}
		else if (escape)
		{
			yesnohandler(false);
			active = false;
		}
	}

	Button::State UIYesNo::button_pressed(std::uint16_t buttonid)
	{
		switch (buttonid)
		{
		case Buttons::YES:
			yesnohandler(true);
			break;
		case Buttons::NO:
			yesnohandler(false);
			break;
		}

		active = false;

		return Button::State::PRESSED;
	}

	UIEnterNumber::UIEnterNumber(std::string message, std::function<void(std::int32_t)> nh, std::int32_t m, std::int32_t quantity) : UINotice(message, NoticeType::ENTERNUMBER)
	{
		numhandler = nh;
		max = m;

		std::int16_t belowtext = box2offset(true) - 21;
		std::int16_t pos_y = belowtext + 35;

		nl::node src = nl::nx::ui["Basic.img"];

		buttons[Buttons::OK] = std::make_unique<MapleButton>(src["BtOK4"], 156, pos_y);
		buttons[Buttons::CANCEL] = std::make_unique<MapleButton>(src["BtCancel4"], 198, pos_y);

		numfield = Textfield(Text::Font::A11M, Text::Alignment::LEFT, Color::Name::LIGHTGREY, Rectangle<int16_t>(24, 232, belowtext, belowtext + 20), 10);
		numfield.change_text(std::to_string(quantity));

		numfield.set_enter_callback(
			[&](std::string numstr)
			{
				handlestring(numstr);
			}
		);

		numfield.set_key_callback(
			KeyAction::Id::ESCAPE,
			[&]()
			{
				active = false;
			}
		);

		numfield.set_state(Textfield::State::FOCUSED);
	}

	void UIEnterNumber::draw(float alpha) const
	{
		UINotice::draw(true);
		UIElement::draw(alpha);

		numfield.draw(position);
	}

	void UIEnterNumber::update()
	{
		UIElement::update();

		numfield.update(position);
	}

	Cursor::State UIEnterNumber::send_cursor(bool clicked, Point<int16_t> cursorpos)
	{
		if (numfield.get_state() == Textfield::State::NORMAL)
		{
			Cursor::State nstate = numfield.send_cursor(cursorpos, clicked);

			if (nstate != Cursor::State::IDLE)
				return nstate;
		}

		return UIElement::send_cursor(clicked, cursorpos);
	}

	void UIEnterNumber::send_key(std::int32_t keycode, bool pressed, bool escape)
	{
		if (keycode == KeyAction::Id::RETURN)
		{
			handlestring(numfield.get_text());
			active = false;
		}
		else if (escape)
		{
			active = false;
		}
	}

	Button::State UIEnterNumber::button_pressed(std::uint16_t buttonid)
	{
		switch (buttonid)
		{
		case Buttons::OK:
			handlestring(numfield.get_text());
			break;
		case Buttons::CANCEL:
			active = false;
			break;
		}

		return Button::State::NORMAL;
	}

	void UIEnterNumber::handlestring(std::string numstr)
	{
		int num = -1;
		bool has_only_digits = (numstr.find_first_not_of("0123456789") == std::string::npos);

		auto okhandler = [&]()
		{
			numfield.set_state(Textfield::State::FOCUSED);
			buttons[Buttons::OK]->set_state(Button::State::NORMAL);
		};

		if (!has_only_digits)
		{
			numfield.set_state(Textfield::State::DISABLED);
			UI::get().emplace<UIOk>("Only numbers are allowed.", okhandler);
			return;
		}
		else
		{
			num = std::stoi(numstr);
		}

		if (num < 1)
		{
			numfield.set_state(Textfield::State::DISABLED);
			UI::get().emplace<UIOk>("You may only enter a number equal to or higher than 1.", okhandler);
			return;
		}
		else if (num > max)
		{
			numfield.set_state(Textfield::State::DISABLED);
			UI::get().emplace<UIOk>("You may only enter a number equal to or lower than " + std::to_string(max) + ".", okhandler);
			return;
		}
		else
		{
			numhandler(num);
			active = false;
		}

		buttons[Buttons::OK]->set_state(Button::State::NORMAL);
	}

	UIOk::UIOk(std::string message, std::function<void()> oh) : UINotice(message, NoticeType::OK)
	{
		okhandler = oh;

		nl::node src = nl::nx::ui["Basic.img"];

		buttons[Buttons::OK] = std::make_unique<MapleButton>(src["BtOK4"], 197, box2offset(false));
	}

	void UIOk::draw(float alpha) const
	{
		UINotice::draw(false);
		UIElement::draw(alpha);
	}

	void UIOk::send_key(std::int32_t keycode, bool pressed, bool escape)
	{
		if (keycode == KeyAction::Id::RETURN)
		{
			okhandler();
			active = false;
		}
		else if (escape)
		{
			active = false;
		}
	}

	Button::State UIOk::button_pressed(std::uint16_t buttonid)
	{
		switch (buttonid)
		{
		case Buttons::OK:
			okhandler();
			break;
		}

		active = false;

		return Button::State::NORMAL;
	}
}