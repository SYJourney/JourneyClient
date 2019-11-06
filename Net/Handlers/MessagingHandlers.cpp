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
#include "MessagingHandlers.h"

#include "../../Data/ItemData.h"
#include "../../Gameplay/Stage.h"
#include "../../IO/UI.h"

#include "../../IO/UITypes/UIStatusMessenger.h"
#include "../../IO/UITypes/UIChatbar.h"

namespace ms
{
	// Modes:
	// 0 - Item(0) or Meso(1) 
	// 3 - Exp gain
	// 4 - Fame
	// 5 - Mesos
	// 6 - Guild points
	void ShowStatusInfoHandler::handle(InPacket& recv) const
	{
		std::int8_t mode = recv.read_byte();

		if (mode == 0)
		{
			std::int8_t mode2 = recv.read_byte();

			if (mode2 == 0)
			{
				std::int32_t itemid = recv.read_int();
				std::int32_t qty = recv.read_int();

				const ItemData& idata = ItemData::get(itemid);

				if (!idata.is_valid())
					return;

				std::string name = idata.get_name();
				std::string sign = (qty < 0) ? "-" : "+";

				show_status(Color::Name::WHITE, "Gained an item: " + name + " (" + sign + std::to_string(qty) + ")");
			}
			else if (mode2 == 1)
			{
				recv.skip(1);

				std::int32_t gain = recv.read_int();
				std::string sign = (gain < 0) ? "-" : "+";

				show_status(Color::Name::WHITE, "Received mesos (" + sign + std::to_string(gain) + ")");
			}
		}
		else if (mode == 3)
		{
			bool white = recv.read_bool();
			std::int32_t gain = recv.read_int();
			bool inchat = recv.read_bool();
			std::int32_t bonus1 = recv.read_int();

			recv.read_short();
			recv.read_int();	// bonus 2
			recv.read_bool();	// 'event or party'
			recv.read_int();	// bonus 3
			recv.read_int();	// bonus 4
			recv.read_int();	// bonus 5

			std::string message = "You have gained experience (+" + std::to_string(gain) + ")";

			if (inchat)
			{
				// TODO: Blank
			}
			else
			{
				show_status(white ? Color::Name::WHITE : Color::Name::YELLOW, message);

				if (bonus1 > 0)
					show_status(Color::Name::YELLOW, "+ Bonus EXP (+" + std::to_string(bonus1) + ")");
			}
		}
		else if (mode == 4)
		{
			std::int32_t gain = recv.read_int();
			std::string sign = (gain < 0) ? "-" : "+";

			show_status(Color::Name::WHITE, "Received fame (" + sign + std::to_string(gain) + ")");
		}
		else if (mode == 5)
		{
			// TODO: Blank
		}
	}

	void ShowStatusInfoHandler::show_status(Color::Name color, const std::string& message) const
	{
		if (auto messenger = UI::get().get_element<UIStatusMessenger>())
			messenger->show_status(color, message);
	}

	void ServerMessageHandler::handle(InPacket& recv) const
	{
		std::int8_t type = recv.read_byte();
		bool servermessage = recv.inspect_bool();

		if (servermessage)
			recv.skip(1);

		std::string message = recv.read_string();

		if (type == 3)
		{
			recv.read_byte(); // channel
			recv.read_bool(); // megaphone
		}
		else if (type == 4)
		{
			UI::get().set_scrollnotice(message);
		}
		else if (type == 7)
		{
			recv.read_int(); // npcid
		}
	}

	void WeekEventMessageHandler::handle(InPacket& recv) const
	{
		recv.read_byte(); // TODO: Always 0xFF, Check this!

		std::string message = recv.read_string();

		static const std::string MAPLETIP = "[MapleTip]";

		if (message.substr(0, MAPLETIP.length()).compare("[MapleTip]"))
			message = "[Notice] " + message;

		UI::get().get_element<UIChatbar>()->send_chatline(message, UIChatbar::LineType::YELLOW);
	}

	void ChatReceivedHandler::handle(InPacket& recv) const
	{
		std::int32_t charid = recv.read_int();

		recv.read_bool(); // 'gm'

		std::string message = recv.read_string();
		std::int8_t type = recv.read_byte();

		if (auto character = Stage::get().get_character(charid))
		{
			message = character->get_name() + ": " + message;
			character->speak(message);
		}

		auto linetype = static_cast<UIChatbar::LineType>(type);

		if (auto chatbar = UI::get().get_element<UIChatbar>())
			chatbar->send_chatline(message, linetype);
	}

	void ScrollResultHandler::handle(InPacket& recv) const
	{
		std::int32_t cid = recv.read_int();
		bool success = recv.read_bool();
		bool destroyed = recv.read_bool();

		recv.read_short(); // Legendary spirit if 1

		CharEffect::Id effect;
		Messages::Type message;

		if (success)
		{
			effect = CharEffect::Id::SCROLL_SUCCESS;
			message = Messages::Type::SCROLL_SUCCESS;
		}
		else
		{
			effect = CharEffect::Id::SCROLL_FAILURE;

			if (destroyed)
				message = Messages::Type::SCROLL_DESTROYED;
			else
				message = Messages::Type::SCROLL_FAILURE;
		}

		Stage::get().show_character_effect(cid, effect);

		if (Stage::get().is_player(cid))
		{
			if (auto chatbar = UI::get().get_element<UIChatbar>())
				chatbar->display_message(message, UIChatbar::LineType::RED);

			UI::get().enable();
		}
	}

	void ShowItemGainInChatHandler::handle(InPacket& recv) const
	{
		std::int8_t mode1 = recv.read_byte();

		if (mode1 == 3)
		{
			std::int8_t mode2 = recv.read_byte();

			if (mode2 == 1) // This is actually 'item gain in chat'
			{
				std::int32_t itemid = recv.read_int();
				std::int32_t qty = recv.read_int();

				const ItemData& idata = ItemData::get(itemid);

				if (!idata.is_valid())
					return;

				std::string name = idata.get_name();
				std::string sign = (qty < 0) ? "-" : "+";
				std::string message = "Gained an item: " + name + " (" + sign + std::to_string(qty) + ")";

				if (auto chatbar = UI::get().get_element<UIChatbar>())
					chatbar->send_chatline(message, UIChatbar::LineType::BLUE);
			}
		}
		else if (mode1 == 13) // card effect
		{
			Stage::get().get_player().show_effect_id(CharEffect::Id::MONSTER_CARD);
		}
		else if (mode1 == 18) // intro effect
		{
			recv.read_string(); // path
		}
		else if (mode1 == 23) // info
		{
			recv.read_string();	// path
			recv.read_int();	// some int
		}
		else // Buff effect
		{
			std::int32_t skillid = recv.read_int();

			// More bytes, but we don't need them
			Stage::get().get_combat().show_player_buff(skillid);
		}
	}
}