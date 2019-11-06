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

#include "../UIDragElement.h"

#include "../Components/Slider.h"
#include "../../Graphics/Text.h"

namespace ms
{
	// The Item inventory.
	class UIItemInventory : public UIDragElement<PosINV>
	{
	public:
		static constexpr Type TYPE = UIElement::Type::ITEMINVENTORY;
		static constexpr bool FOCUSED = false;
		static constexpr bool TOGGLED = true;

		UIItemInventory(const Inventory& inventory);

		void draw(float inter) const override;
		void update() override;

		void doubleclick(Point<int16_t> position) override;
		bool send_icon(const Icon& icon, Point<int16_t> position) override;
		void toggle_active() override;
		bool remove_cursor(bool clicked, Point<int16_t> cursorpos) override;
		Cursor::State send_cursor(bool pressed, Point<int16_t> position) override;
		void send_key(std::int32_t keycode, bool pressed, bool escape) override;

		void modify(InventoryType::Id type, std::int16_t pos, std::int8_t mode, std::int16_t arg);
		void set_sort(bool enabled);
		void change_tab(InventoryType::Id type);
		void clear_new();

	protected:
		Button::State button_pressed(std::uint16_t buttonid) override;

	private:
		void show_item(std::int16_t slot);
		void clear_tooltip();
		void load_icons();
		void update_slot(std::int16_t slot);
		bool is_visible(std::int16_t slot) const;
		bool is_not_visible(std::int16_t slot) const;
		bool can_wear_equip(std::int16_t slot) const;
		std::int16_t slot_by_position(Point<int16_t> position) const;
		std::uint16_t button_by_tab(InventoryType::Id tab) const;
		Point<int16_t> get_slotpos(std::int16_t slot) const;
		Point<int16_t> get_full_slotpos(std::int16_t slot) const;
		Point<int16_t> get_tabpos(InventoryType::Id tab) const;
		Icon* get_icon(std::int16_t slot);
		void set_full(bool enabled);

		class ItemIcon : public Icon::Type
		{
		public:
			ItemIcon(const UIItemInventory& parent, InventoryType::Id sourcetab, Equipslot::Id eqsource, std::int16_t source, std::int16_t count, bool untradable, bool cashitem);

			void drop_on_stage() const override;
			void drop_on_equips(Equipslot::Id eqslot) const override;
			bool drop_on_items(InventoryType::Id tab, Equipslot::Id, std::int16_t slot, bool) const override;
			void drop_on_bindings(Point<int16_t>, bool) const override {}
			void set_count(std::int16_t count) override;

		private:
			InventoryType::Id sourcetab;
			Equipslot::Id eqsource;
			std::int16_t source;
			std::int16_t count;
			bool untradable;
			bool cashitem;
			const UIItemInventory& parent;
		};

		static constexpr std::uint16_t ROWS = 8;
		static constexpr std::uint16_t COLUMNS = 4;
		static constexpr std::uint16_t MAXSLOTS = ROWS * COLUMNS;
		static constexpr std::uint16_t MAXFULLSLOTS = COLUMNS * MAXSLOTS;
		static constexpr std::uint16_t ICON_WIDTH = 36;
		static constexpr std::uint16_t ICON_HEIGHT = 35;

		enum Buttons
		{
			BT_CLOSE,
			BT_TAB_EQUIP,
			BT_TAB_USE,
			BT_TAB_ETC,
			BT_TAB_SETUP,
			BT_TAB_CASH,
			BT_COIN,
			BT_POINT,
			BT_GATHER,
			BT_SORT,
			BT_FULL,
			BT_SMALL,
			BT_POT,
			BT_UPGRADE,
			BT_APPRAISE,
			BT_EXTRACT,
			BT_DISASSEMBLE,
			BT_TOAD,
			BT_COIN_SM,
			BT_POINT_SM,
			BT_GATHER_SM,
			BT_SORT_SM,
			BT_FULL_SM,
			BT_SMALL_SM,
			BT_POT_SM,
			BT_UPGRADE_SM,
			BT_APPRAISE_SM,
			BT_EXTRACT_SM,
			BT_DISASSEMBLE_SM,
			BT_TOAD_SM,
			BT_CASHSHOP
		};

		const Inventory& inventory;

		Animation newitemslot;
		Animation newitemtab;
		Texture projectile;
		Texture disabled;
		Text mesolabel;
		Text maplepointslabel;
		Slider slider;

		std::map<int16_t, std::unique_ptr<Icon>> icons;
		std::map<InventoryType::Id, std::pair<int16_t, std::int16_t>> slotrange;

		InventoryType::Id tab;
		InventoryType::Id newtab;
		std::int16_t newslot;
		bool ignore_tooltip;

		bool sort_enabled;
		bool full_enabled;
		Texture backgrnd;
		Texture backgrnd2;
		Texture backgrnd3;
		Texture full_backgrnd;
		Texture full_backgrnd2;
		Texture full_backgrnd3;
		Point<int16_t> bg_dimensions;
		Point<int16_t> bg_full_dimensions;
	};
}