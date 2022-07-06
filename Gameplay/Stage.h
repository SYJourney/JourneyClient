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

#include "Combat/Combat.h"
#include "MapleMap/MapBackgrounds.h"
#include "MapleMap/MapDrops.h"
#include "MapleMap/MapEffect.h"
#include "MapleMap/MapNpcs.h"
#include "MapleMap/MapPortals.h"
#include "MapleMap/MapTilesObjs.h"

#include "../Timer.h"

#include "../IO/KeyType.h"

namespace ms
{
	class Stage : public Singleton<Stage>
	{
	public:
		Stage();

		void init();

		// Loads the map to display
		void load(int32_t mapid, int8_t portalid);
		// Remove all map objects and graphics
		void clear();

		// Construct the player from a character entry
		void loadplayer(const CharEntry& entry);

		// Call 'draw()' of all objects on stage
		void draw(float alpha) const;
		// Calls 'update()' of all objects on stage
		void update();

		// Show a character effect
		void show_character_effect(int32_t cid, CharEffect::Id effect);

		// Send key input to the stage
		void send_key(KeyType::Id keytype, int32_t keycode, bool pressed);
		// Send mouse input to the stage
		Cursor::State send_cursor(bool clicked, Point<int16_t> cursor_position);

		// Check if the specified id is the player's id
		bool is_player(int32_t cid) const;

		// Returns a reference to the NPCs on the current map
		MapNpcs& get_npcs();
		// Returns a reference to the other characters on the current map
		MapChars& get_chars();
		// Returns a reference to the mobs on the current map
		MapMobs& get_mobs();
		// Returns a reference to the reactors on the current map
		MapReactors& get_reactors();
		// Returns a reference to the drops on the current map
		MapDrops& get_drops();
		// Returns a reference to the Player
		Player& get_player();
		// Return a reference to the attack and buff component
		Combat& get_combat();
		// Returns the current map ID
		int32_t get_mapid();

		// Return a pointer to a character, possibly the player.
		Optional<Char> get_character(int32_t cid);

		// Set a map effect
		void add_effect(std::string path);

		// Get the total up time of the client
		int64_t get_uptime();
		// Get the level at the start of the client
		uint16_t get_uplevel();
		// Get the EXP at the start of the client
		int64_t get_upexp();

		// Notify the server that the player has moved maps
		void transfer_player();

	private:
		void load_map(int32_t mapid);
		void respawn(int8_t portalid);
		void check_portals();
		void check_seats();
		void check_ladders(bool up);
		void check_drops();

		enum State
		{
			INACTIVE,
			TRANSITION,
			ACTIVE
		};

		Camera camera;
		Physics physics;
		Player player;

		Optional<Playable> playable;
		State state;
		int32_t mapid;

		MapInfo mapinfo;
		MapTilesObjs tilesobjs;
		MapBackgrounds backgrounds;
		MapPortals portals;
		MapReactors reactors;
		MapNpcs npcs;
		MapChars chars;
		MapMobs mobs;
		MapDrops drops;
		MapEffect effect;

		Combat combat;

		std::chrono::time_point<std::chrono::steady_clock> start;
		uint16_t levelBefore;
		int64_t expBefore;
	};
}