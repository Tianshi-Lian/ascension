/**
 * File: ascension.hpp
 * Project: ascension
 * File Created: 2023-02-24 19:27:40
 * Author: Rob Graham (robgrahamdev@gmail.com)
 * Last Modified: 2023-04-09 16:03:11
 * ------------------
 * Copyright 2023 Rob Graham
 * ==================
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * ==================
 */

#pragma once

namespace ascension {

// The seven planes of existence to ascend through
enum class plane {
    MORTAL,
    MARTIAL,
    IMMORTAL,
    DIVINE,
    HEAVENLY,
    ANCESTRAL,
    CELESTIAL
};

// The six paths which can be followed, studied and mastered within the planes
// The martial path is the practice of cultivation and fighting
// The path of alchemy is the practice of creating pills, medicine & poisons
// The path of smithing is the practice of creating weapons, armour and jewellery
// The path of formation is the practice of creating powerful arrays and magical formations
// The divine path is a special path followed by divine entities for control over the worlds
// The celestial path is a special path followed by celestial entities for writing the laws of the universe
enum class path {
    MARTIAL,
    ALCHEMY,
    SMITHING,
    FORMATION,
    DIVINE,
    CELESTIAL
};

// The different ranks (1-9) that a practitioner of a path can be
// This only applies to the martial path and the paths of alchemy, smithing and formations
// When referring to one's rank it is always assumed to be the martial path.
//  - "mortal novice" will be an existence of the mortal plane, at rank 1
//  - "martial noble" will be an existence of the martial plane, at rank 4
//  - "alchemist novice" will be an rank 1 alchemist of the plane which matches
//     their martial path cultivation
//  - "formation emperor" will be a rank 6 formation expert of the plane which matches
//     their martial path cultivation
enum class rank {
    NOVICE,
    WARRIOR,
    MASTER,
    NOBLE,
    MONARCH,
    EMPEROR,
    SAINT,
    DEMIGOD,
    GOD
};

// Fire > Ice > Wind > Earth > Lightning > Water > Fire...
// Aether is a mysterious, unseen power from which the primal aspects emerge and return
// Light < > Dark
// Time and space have no inherent strength or weakness, though are infinitely more powerful than the other elements
enum class aspect {
    FIRE,
    ICE,
    WIND,
    EARTH,
    LIGHTNING,
    WATER,
    AETHER,
    LIGHT,
    DARK,
    TIME,
    SPACE
};

// Beasts can be broken down into many sub-races and include half-beasts and beasts which become humanoid
// Devils aren't inherently evil, but often fall that way through practice of Devilish arts
// Angels aren't inherently good, and often become arrogant due to their strong innate abilities
// Monsters are the only race which are inherently evil, they are malformed, grotesque creatures who only fight
// Spirits are mysterious, often invisible, creatures which guide the aspects of the world, though can make pacts with others
enum class creature {
    HUMAN,
    BEAST,
    DEVIL,
    ANGEL,
    MONSTER,
    SPIRIT
};

class Ascension {
  public:
    Ascension();
    ~Ascension();

    bool on_initialize();
    void on_update(f32 delta_time);
    void on_render(f32 delta_time);
};

} // namespace ascension
