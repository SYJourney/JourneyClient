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
#include "Messages.h"

#include "UI.h"

#include "UITypes/UIChatBar.h"

namespace ms
{
	const EnumMap<Messages::Type, const char*> Messages::messages =
	{
		"",
		"You cannot use this skill with this weapon.",
		"You do not have enough hp to use this skill.",
		"You do not have enough mp to use this skill.",
		"You do not have enough arrows to use this attack.",
		"You do not have enough bullets to use this attack.",
		"You do not have enough throwing stars to use this attack.",
		"You cannot use this skill as it is on cooldown.",
		"The scroll lights up and it's mysterious powers have been transferred to the item.",
		"The scroll lights up but the item remains as if nothing happened.",
		"The item has been destroyed due to the overwhelming power of the scroll."
	};

	InChatMessage::InChatMessage(Messages::Type t)
	{
		type = t;
	}

	void InChatMessage::drop() const
	{
		if (type == Messages::Type::NONE)
			return;

		if (auto chatbar = UI::get().get_element<UIChatBar>())
			chatbar->show_message(Messages::messages[type], UIChatBar::RED);
	}

	ForbidSkillMessage::ForbidSkillMessage(SpecialMove::ForbidReason reason, Weapon::Type weapon) : InChatMessage(message_by_reason(reason, weapon)) {}

	Messages::Type ForbidSkillMessage::message_by_reason(SpecialMove::ForbidReason reason, Weapon::Type weapon)
	{
		switch (reason)
		{
		case SpecialMove::ForbidReason::FBR_WEAPONTYPE:
			return Messages::Type::SKILL_WEAPONTYPE;
		case SpecialMove::ForbidReason::FBR_HPCOST:
			return Messages::Type::SKILL_HPCOST;
		case SpecialMove::ForbidReason::FBR_MPCOST:
			return Messages::Type::SKILL_MPCOST;
		case SpecialMove::ForbidReason::FBR_COOLDOWN:
			return Messages::Type::SKILL_COOLDOWN;
		case SpecialMove::ForbidReason::FBR_BULLETCOST:
			return message_by_weapon(weapon);
		default:
			return Messages::Type::NONE;
		}
	}

	Messages::Type ForbidSkillMessage::message_by_weapon(Weapon::Type weapon)
	{
		switch (weapon)
		{
		case Weapon::Type::BOW:
		case Weapon::Type::CROSSBOW:
			return Messages::Type::SKILL_NOARROWS;
		case Weapon::Type::CLAW:
			return Messages::Type::SKILL_NOSTARS;
		default:
			return Messages::Type::SKILL_NOBULLETS;
		}
	}
}