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

#include "ActiveBuffs.h"
#include "MonsterBook.h"
#include "PassiveBuffs.h"
#include "QuestLog.h"
#include "SkillBook.h"
#include "TeleportRock.h"

#include "Inventory/Inventory.h"

#include "../Gameplay/Playable.h"

#include "../Gameplay/Combat/Skill.h"
#include "../Gameplay/MapleMap/Layer.h"
#include "../Gameplay/MapleMap/MapInfo.h"

namespace ms
{
	class Player : public Playable, public Char
	{
	public:
		// Construct a player object from the given character entry
		Player(const CharEntry& entry);
		Player();

		// Draw the player
		void draw(Layer::Id layer, double viewx, double viewy, float alpha) const;
		// Update the player's animation, physics and states.
		int8_t update(const Physics& physics) override;
		// Set flipped ignore if attacking
		void set_direction(bool flipped) override;
		// Set state ignore if attacking
		void set_state(State stance) override;

		// Respawn the player at the given position
		void respawn(Point<int16_t> position, bool underwater);
		// Sends a Keyaction to the player's state, to apply forces, change the state and other behavior.
		void send_action(KeyAction::Id action, bool pressed);
		// Recalculates the total stats from base stats, inventories and skills.
		void recalc_stats(bool equipchanged);
		// Change the equipment at the specified slot and recalculate stats
		void change_equip(int16_t slot);
		// Use the item from the player's inventory with the given id
		void use_item(int32_t itemid);

		// Return if the player is attacking
		bool is_attacking() const;
		// Return whether the player can attack or not
		bool can_attack() const;
		// Return whether the player can use a skill or not
		SpecialMove::ForbidReason can_use(const SpecialMove& move) const;
		// Create an attack struct using the player's stats
		Attack prepare_attack(bool skill) const;

		// Execute a rush movement
		void rush(double targetx);

		// Check whether the player is invincible
		bool is_invincible() const override;
		// Handle an attack to the player
		MobAttackResult damage(const MobAttack& attack);

		// Apply a buff to the player
		void give_buff(Buff buff);
		// Cancel a buff
		void cancel_buff(Buffstat::Id stat);
		// Return whether the buff is active
		bool has_buff(Buffstat::Id stat) const;

		// Change a skill
		void change_skill(int32_t skill_id, int32_t level, int32_t masterlevel, int64_t expiration);
		// Put a skill on cooldown
		void add_cooldown(int32_t skill_id, int32_t time);
		// Check if a skill is on cooldown
		bool has_cooldown(int32_t skill_id) const;

		// Change the player's level, display the "level up" effect.
		void change_level(uint16_t level);
		// Change the player's job, display the job change effect.
		void change_job(uint16_t jobid);

		// Return the character's level
		uint16_t get_level() const override;
		// Return the character's level of a skill
		int32_t get_skilllevel(int32_t skillid) const override;
		// Return the character's attacking speed
		int8_t get_integer_attackspeed() const override;

		// Returns the current walking force, calculated from the total ES_SPEED stat.
		float get_walkforce() const;
		// Returns the current jumping force, calculated from the total ES_JUMP stat.
		float get_jumpforce() const;
		// Returns the climbing force, calculated from the total ES_SPEED stat.
		float get_climbforce() const;
		// Returns the flying force
		float get_flyforce() const;

		// Return whether the player is underwater
		bool is_underwater() const;
		// Returns if a Keyaction is currently active 
		bool is_key_down(KeyAction::Id action) const;
		// Return a pointer to the ladder the player is on
		Optional<const Ladder> get_ladder() const;

		// Change players position to the seat's position and stance to Char::State::SIT
		void set_seat(Optional<const Seat> seat);
		// Change players x-position to the ladder x and change stance to Char::State::LADDER or Char::State::ROPE
		void set_ladder(Optional<const Ladder> ladder);
		// Sets a quick cooldown on climbing so when jumping off a ladder or rope, it doesn't start climb again.
		void set_climb_cooldown();
		// Checks if the player can climb
		bool can_climb();

		// Obtain a reference to the player's stats
		CharStats& get_stats();
		// Obtain a reference to the player's stats
		const CharStats& get_stats() const;
		// Obtain a reference to the player's inventory
		Inventory& get_inventory();
		// Obtain a reference to the player's inventory
		const Inventory& get_inventory() const;
		// Obtain a reference to the player's skills
		SkillBook& get_skills();
		// Obtain a reference to the player's QuestLog
		QuestLog& get_quests();
		// Obtain a reference to the player's TeleportRock locations
		TeleportRock& get_teleportrock();
		// Obtain a reference to the player's MonsterBook
		MonsterBook& get_monsterbook();

	private:
		CharStats stats;
		Inventory inventory;
		SkillBook skillbook;
		QuestLog questlog;
		TeleportRock teleportrock;
		MonsterBook monsterbook;

		EnumMap<Buffstat::Id, Buff> buffs;
		ActiveBuffs active_buffs;
		PassiveBuffs passive_buffs;

		std::unordered_map<int32_t, int32_t> cooldowns;

		std::map<KeyAction::Id, bool> keysdown;

		Movement lastmove;

		Randomizer randomizer;

		Optional<const Ladder> ladder;
		TimedBool climb_cooldown;

		bool underwater;
	};
}