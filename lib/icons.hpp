/*
 * Copyright 2011-2016 "Silver Squirrel Software Handelsbolag"
 * Copyright 2023-2024 "John Högberg"
 *
 * This file is part of tibiarc.
 *
 * tibiarc is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * tibiarc is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with tibiarc. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __TRC_ICONS_HPP__
#define __TRC_ICONS_HPP__

#include "versions_decl.hpp"

#include "pictures.hpp"
#include "creature.hpp"

#include <initializer_list>
#include <type_traits>

namespace trc {

class Icons {
    const trc::Canvas &Canvas;

public:
    Sprite ClientBackground;
    Sprite IconBarBackground;
    Sprite IconBarWar;
    Sprite InventoryBackground;
    Sprite RiskyIcon;
    Sprite SecondaryStatBackground;

    // Minimap area
    Sprite Compass;
    Sprite ZoomIn;
    Sprite ZoomOut;
    Sprite LevelUp;
    Sprite LevelDown;

    // Statusbars
    Sprite HealthIcon;
    Sprite ManaIcon;
    Sprite EmptyStatusBar;
    Sprite HealthBar;
    Sprite ManaBar;

    // Buttons
    Sprite Button34px;
    Sprite Button34pxPressed;
    Sprite Button43px;
    Sprite Button43pxPressed;
    Sprite Button86px;
    Sprite Button86pxPressed;
    Sprite Minimize;
    Sprite MinimizePressed;
    Sprite Close;
    Sprite ClosePressed;
    Sprite SkillsIcon;
    Sprite BattleIcon;
    Sprite FightingOffensive;
    Sprite FightingBalanced;
    Sprite FightingDefensive;
    Sprite AttackStanding;
    Sprite AttackChasing;
    Sprite AttackUnmarked;  // Note: before 7.4 this sprite and button was attack mode "keep distance"

    // Window parts
    Sprite WindowHeaderMiddle;
    Sprite WindowHeaderLeft;
    Sprite WindowHeaderRight;
    Sprite WindowLeft;
    Sprite WindowRight;
    Sprite WindowBottom;
    Sprite WindowBottomLeft;
    Sprite WindowBottomRight;

    const Sprite &GetCharacterSkull(CharacterSkull skull) const;
    const Sprite &GetCreatureType(CreatureType type) const;
    const Sprite &GetIconBarSkull(CharacterSkull skull) const;
    const Sprite &GetInventorySlot(InventorySlot slot) const;
    const Sprite &GetPartyShield(PartyShield shield) const;
    const Sprite &GetStatusIcon(StatusIcon status) const;
    const Sprite &GetWarIcon(WarIcon war) const;

    Icons(const Version &version);

    Icons(const Icons &other) = delete;

private:
    std::unordered_map<CharacterSkull, Sprite> CharacterSkullSprites;
    std::unordered_map<CharacterSkull, Sprite> IconBarSkullSprites;
    std::unordered_map<CreatureType, Sprite> CreatureTypeSprites;
    std::unordered_map<InventorySlot, Sprite> InventorySlotSprites;
    std::unordered_map<PartyShield, Sprite> PartyShieldSprites;
    std::unordered_map<StatusIcon, Sprite> StatusIconSprites;
    std::unordered_map<WarIcon, Sprite> WarIconSprites;
};

} // namespace trc
#endif /* __TRC_ICONS_HPP__ */
