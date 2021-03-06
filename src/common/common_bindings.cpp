///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ***************************************************************************
*** \file    common_bindings.cpp
*** \author  Daniel Steuernol, steu@allacrost.org
*** \brief   Lua bindings for common game code
***
*** All bindings for the common code is contained within this file.
*** Therefore, everything that you see bound within this file will be made
*** available in Lua. This file also binds some of the utility code
*** found in src/utils.h.
***
*** \note To most C++ programmers, the syntax of the binding code found in this
*** file may be very unfamiliar and obtuse. Refer to the Luabind documentation
*** as necessary to gain an understanding of this code style.
*** **************************************************************************/

#include "common.h"
#include "dialogue.h"

#include "common/global/global.h"
#include "common/global/global_actors.h"
#include "common/global/global_effects.h"
#include "common/global/global_objects.h"
#include "common/global/global_skills.h"
#include "common/global/global_utils.h"

namespace hoa_defs {

void BindCommonCode() {
	// ---------- Bind Utils Functions
	{
	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_utils")
	[
		luabind::class_<hoa_utils::ustring>("ustring")
			.def(luabind::constructor<uint16 *>())
			.def(luabind::constructor<>())
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_utils")
	[
		luabind::def("RandomFloat", (float(*)(void)) &hoa_utils::RandomFloat),
		luabind::def("RandomBoundedInteger", &hoa_utils::RandomBoundedInteger),
		luabind::def("MakeUnicodeString", &hoa_utils::MakeUnicodeString),
		luabind::def("MakeStandardString", &hoa_utils::MakeStandardString)
	];
	}

	// ---------- Bind Common Components
	{
	using namespace hoa_common;

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_common")
	[
		luabind::class_<CommonDialogue>("CommonDialogue")
			// TODO: add commented lines back in later. There is a build issue with the editor when these lines are included
// 			.def("AddLine", (void(CommonDialogue::*)(std::string))&CommonDialogue::AddLine)
// 			.def("AddLine", (void(CommonDialogue::*)(std::string, int32))&CommonDialogue::AddLine)
// 			.def("AddLineTimed", (void(CommonDialogue::*)(std::string, uint32))&CommonDialogue::AddLineTimed)
// 			.def("AddLineTimed", (void(CommonDialogue::*)(std::string, int32, uint32))&CommonDialogue::AddLineTimed)
// 			.def("AddOption", (void(CommonDialogue::*)(std::string))&CommonDialogue::AddOption)
// 			.def("AddOption", (void(CommonDialogue::*)(std::string, int32))&CommonDialogue::AddOption)
			.def("HasAlreadySeen", &CommonDialogue::HasAlreadySeen)
			.def("SetTimesSeen", &CommonDialogue::SetTimesSeen)
			.def("SetMaxViews", &CommonDialogue::SetMaxViews)
	];

	} // End using common namespace


	// ---------- Bind Global Components
	{
	using namespace hoa_global;

	luabind::def("GetTargetText", &GetTargetText),
	luabind::def("IsTargetPoint", &IsTargetPoint),
	luabind::def("IsTargetActor", &IsTargetActor),
	luabind::def("IsTargetParty", &IsTargetParty),
	luabind::def("IsTargetSelf", &IsTargetSelf),
	luabind::def("IsTargetAlly", &IsTargetAlly),
	luabind::def("IsTargetFoe", &IsTargetFoe),
	// TODO: Luabind doesn't like these functions. I think its because they take reference arguments.
// 	luabind::def("IncrementIntensity", &IncrementIntensity),
// 	luabind::def("DecrementIntensity", &DecrementIntensity),

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_global")
	[
		luabind::class_<GameGlobal>("GameGlobal")
			.def("AddCharacter", (void(GameGlobal::*)(uint32)) &GameGlobal::AddCharacter)
			.def("RemoveCharacter", (void(GameGlobal::*)(uint32)) &GameGlobal::RemoveCharacter)
			.def("GetCharacter", &GameGlobal::GetCharacter)
			.def("GetActiveParty", &GameGlobal::GetActiveParty)
			.def("GetDrunes", &GameGlobal::GetDrunes)
			.def("SetDrunes", &GameGlobal::SetDrunes)
			.def("AddDrunes", &GameGlobal::AddDrunes)
			.def("SubtractDrunes", &GameGlobal::SubtractDrunes)
			.def("GetMaxExperienceLevel", &GameGlobal::GetMaxExperienceLevel)
			.def("SetMaxExperienceLevel", &GameGlobal::SetMaxExperienceLevel)
			.def("AddToInventory", (void (GameGlobal::*)(uint32, uint32)) &GameGlobal::AddToInventory)
			.def("RemoveFromInventory", (void (GameGlobal::*)(uint32)) &GameGlobal::RemoveFromInventory)
			.def("IsObjectInInventory", &GameGlobal::IsObjectInInventory)
			.def("IncrementObjectCount", &GameGlobal::IncrementObjectCount)
			.def("DecrementObjectCount", &GameGlobal::DecrementObjectCount)
			.def("DoesEventGroupExist", &GameGlobal::DoesEventGroupExist)
			.def("DoesEventExist", &GameGlobal::DoesEventExist)
			.def("AddNewEventGroup", &GameGlobal::AddNewEventGroup)
			.def("GetEventGroup", &GameGlobal::GetEventGroup)
			.def("GetEventValue", &GameGlobal::GetEventValue)
			.def("SetEventValue", &GameGlobal::SetEventValue)
			.def("GetNumberEventGroups", &GameGlobal::GetNumberEventGroups)
			.def("GetNumberEvents", &GameGlobal::GetNumberEvents)
			.def("SetMapFilename", (void(GameGlobal::*)(const std::string&)) &GameGlobal::SetMapFilename)
			.def("GetMapFilename", &GameGlobal::GetMapFilename)
			.def("GetSaveLocationX", &GameGlobal::GetSaveLocationX)
			.def("GetSaveLocationY", &GameGlobal::GetSaveLocationY)
			.def("UnsetSaveLocation", &GameGlobal::UnsetSaveLocation)
			.def("GetPreviousLocation", &GameGlobal::GetPreviousLocation)
			.def("GetBattleSetting", &GameGlobal::GetBattleSetting)
			.def("SetBattleSetting", &GameGlobal::SetBattleSetting)

			// Namespace constants
			.enum_("constants") [
				// Character types
				luabind::value("GLOBAL_CHARACTER_INVALID", GLOBAL_CHARACTER_INVALID),
				luabind::value("GLOBAL_CHARACTER_ALL", GLOBAL_CHARACTER_ALL),
				// Object types
				luabind::value("GLOBAL_OBJECT_INVALID", GLOBAL_OBJECT_INVALID),
				luabind::value("GLOBAL_OBJECT_ITEM", GLOBAL_OBJECT_ITEM),
				luabind::value("GLOBAL_OBJECT_WEAPON", GLOBAL_OBJECT_WEAPON),
				luabind::value("GLOBAL_OBJECT_HEAD_ARMOR", GLOBAL_OBJECT_HEAD_ARMOR),
				luabind::value("GLOBAL_OBJECT_TORSO_ARMOR", GLOBAL_OBJECT_TORSO_ARMOR),
				luabind::value("GLOBAL_OBJECT_ARM_ARMOR", GLOBAL_OBJECT_ARM_ARMOR),
				luabind::value("GLOBAL_OBJECT_LEG_ARMOR", GLOBAL_OBJECT_LEG_ARMOR),
				luabind::value("GLOBAL_OBJECT_SHARD", GLOBAL_OBJECT_SHARD),
				luabind::value("GLOBAL_OBJECT_KEY_ITEM", GLOBAL_OBJECT_KEY_ITEM),
				// Item usage constants
				luabind::value("GLOBAL_USE_INVALID", GLOBAL_USE_INVALID),
				luabind::value("GLOBAL_USE_FIELD", GLOBAL_USE_FIELD),
				luabind::value("GLOBAL_USE_BATTLE", GLOBAL_USE_BATTLE),
				luabind::value("GLOBAL_USE_ALL", GLOBAL_USE_ALL),
				// Item and skill alignment constants
				luabind::value("GLOBAL_POSITION_HEAD", GLOBAL_POSITION_HEAD),
				luabind::value("GLOBAL_POSITION_TORSO", GLOBAL_POSITION_TORSO),
				luabind::value("GLOBAL_POSITION_ARMS", GLOBAL_POSITION_ARMS),
				luabind::value("GLOBAL_POSITION_LEGS", GLOBAL_POSITION_LEGS),
				// Skill types
				luabind::value("GLOBAL_SKILL_INVALID", GLOBAL_SKILL_INVALID),
				luabind::value("GLOBAL_SKILL_ATTACK", GLOBAL_SKILL_ATTACK),
				luabind::value("GLOBAL_SKILL_DEFEND", GLOBAL_SKILL_DEFEND),
				luabind::value("GLOBAL_SKILL_SUPPORT", GLOBAL_SKILL_SUPPORT),
				// Battle settings
				luabind::value("GLOBAL_BATTLE_INVALID", GLOBAL_BATTLE_INVALID),
				luabind::value("GLOBAL_BATTLE_WAIT", GLOBAL_BATTLE_WAIT),
				luabind::value("GLOBAL_BATTLE_ACTIVE", GLOBAL_BATTLE_ACTIVE),
				luabind::value("GLOBAL_BATTLE_TOTAL", GLOBAL_BATTLE_TOTAL),
				// Elemental type constants
				luabind::value("GLOBAL_ELEMENTAL_FIRE", GLOBAL_ELEMENTAL_FIRE),
				luabind::value("GLOBAL_ELEMENTAL_WATER", GLOBAL_ELEMENTAL_WATER),
				luabind::value("GLOBAL_ELEMENTAL_VOLT", GLOBAL_ELEMENTAL_VOLT),
				luabind::value("GLOBAL_ELEMENTAL_EARTH", GLOBAL_ELEMENTAL_EARTH),
				luabind::value("GLOBAL_ELEMENTAL_SLICING", GLOBAL_ELEMENTAL_SLICING),
				luabind::value("GLOBAL_ELEMENTAL_SMASHING", GLOBAL_ELEMENTAL_SMASHING),
				luabind::value("GLOBAL_ELEMENTAL_MAULING", GLOBAL_ELEMENTAL_MAULING),
				luabind::value("GLOBAL_ELEMENTAL_PIERCING", GLOBAL_ELEMENTAL_PIERCING),
				// Status type constants
				luabind::value("GLOBAL_STATUS_INVALID", GLOBAL_STATUS_INVALID),
				luabind::value("GLOBAL_STATUS_STRENGTH_RAISE", GLOBAL_STATUS_STRENGTH_RAISE),
				luabind::value("GLOBAL_STATUS_STRENGTH_LOWER", GLOBAL_STATUS_STRENGTH_LOWER),
				luabind::value("GLOBAL_STATUS_VIGOR_RAISE", GLOBAL_STATUS_VIGOR_RAISE),
				luabind::value("GLOBAL_STATUS_VIGOR_LOWER", GLOBAL_STATUS_VIGOR_LOWER),
				luabind::value("GLOBAL_STATUS_FORTITUDE_RAISE", GLOBAL_STATUS_FORTITUDE_RAISE),
				luabind::value("GLOBAL_STATUS_FORTITUDE_LOWER", GLOBAL_STATUS_FORTITUDE_LOWER),
				luabind::value("GLOBAL_STATUS_PROTECTION_RAISE", GLOBAL_STATUS_PROTECTION_RAISE),
				luabind::value("GLOBAL_STATUS_PROTECTION_LOWER", GLOBAL_STATUS_PROTECTION_LOWER),
				luabind::value("GLOBAL_STATUS_AGILITY_RAISE", GLOBAL_STATUS_AGILITY_RAISE),
				luabind::value("GLOBAL_STATUS_AGILITY_LOWER", GLOBAL_STATUS_AGILITY_LOWER),
				luabind::value("GLOBAL_STATUS_EVADE_RAISE", GLOBAL_STATUS_EVADE_RAISE),
				luabind::value("GLOBAL_STATUS_EVADE_LOWER", GLOBAL_STATUS_EVADE_LOWER),
				luabind::value("GLOBAL_STATUS_HP_REGEN", GLOBAL_STATUS_HP_REGEN),
				luabind::value("GLOBAL_STATUS_HP_DRAIN", GLOBAL_STATUS_HP_DRAIN),
				luabind::value("GLOBAL_STATUS_SP_REGEN", GLOBAL_STATUS_SP_REGEN),
				luabind::value("GLOBAL_STATUS_SP_DRAIN", GLOBAL_STATUS_SP_DRAIN),
				luabind::value("GLOBAL_STATUS_PARALYSIS", GLOBAL_STATUS_PARALYSIS),
				luabind::value("GLOBAL_STATUS_STASIS", GLOBAL_STATUS_STASIS),
				// Intensity type constants
				luabind::value("GLOBAL_INTENSITY_NEG_EXTREME", GLOBAL_INTENSITY_NEG_EXTREME),
				luabind::value("GLOBAL_INTENSITY_NEG_GREATER", GLOBAL_INTENSITY_NEG_GREATER),
				luabind::value("GLOBAL_INTENSITY_NEG_MODERATE", GLOBAL_INTENSITY_NEG_MODERATE),
				luabind::value("GLOBAL_INTENSITY_NEG_LESSER", GLOBAL_INTENSITY_NEG_LESSER),
				luabind::value("GLOBAL_INTENSITY_NEUTRAL", GLOBAL_INTENSITY_NEUTRAL),
				luabind::value("GLOBAL_INTENSITY_POS_LESSER", GLOBAL_INTENSITY_POS_LESSER),
				luabind::value("GLOBAL_INTENSITY_POS_MODERATE", GLOBAL_INTENSITY_POS_MODERATE),
				luabind::value("GLOBAL_INTENSITY_POS_GREATER", GLOBAL_INTENSITY_POS_GREATER),
				luabind::value("GLOBAL_INTENSITY_POS_EXTREME", GLOBAL_INTENSITY_POS_EXTREME),
				// Target constants
				luabind::value("GLOBAL_TARGET_INVALID", GLOBAL_TARGET_INVALID),
				luabind::value("GLOBAL_TARGET_SELF_POINT", GLOBAL_TARGET_SELF_POINT),
				luabind::value("GLOBAL_TARGET_ALLY_POINT", GLOBAL_TARGET_ALLY_POINT),
				luabind::value("GLOBAL_TARGET_FOE_POINT", GLOBAL_TARGET_FOE_POINT),
				luabind::value("GLOBAL_TARGET_SELF", GLOBAL_TARGET_SELF),
				luabind::value("GLOBAL_TARGET_ALLY", GLOBAL_TARGET_ALLY),
				luabind::value("GLOBAL_TARGET_ALLY_EVEN_DEAD", GLOBAL_TARGET_ALLY_EVEN_DEAD),
				luabind::value("GLOBAL_TARGET_FOE", GLOBAL_TARGET_FOE),
				luabind::value("GLOBAL_TARGET_ALL_ALLIES", GLOBAL_TARGET_ALL_ALLIES),
				luabind::value("GLOBAL_TARGET_ALL_FOES", GLOBAL_TARGET_ALL_FOES)
			]
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_global")
	[
		luabind::class_<GlobalEventGroup>("GlobalEventGroup")
			.def("DoesEventExist", &GlobalEventGroup::DoesEventExist)
			.def("AddNewEvent", &GlobalEventGroup::AddNewEvent)
			.def("GetEvent", &GlobalEventGroup::GetEvent)
			.def("SetEvent", &GlobalEventGroup::SetEvent)
			.def("GetNumberEvents", &GlobalEventGroup::GetNumberEvents)
			.def("GetGroupName", &GlobalEventGroup::GetGroupName)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_global")
	[
		luabind::class_<GlobalAttackPoint>("GlobalAttackPoint")
			.def("GetName", &GlobalAttackPoint::GetName)
			.def("GetXPosition", &GlobalAttackPoint::GetXPosition)
			.def("GetYPosition", &GlobalAttackPoint::GetYPosition)
			.def("GetFortitudeModifier", &GlobalAttackPoint::GetFortitudeModifier)
			.def("GetProtectionModifier", &GlobalAttackPoint::GetProtectionModifier)
			.def("GetEvadeModifier", &GlobalAttackPoint::GetEvadeModifier)
			.def("GetActorOwner", &GlobalAttackPoint::GetActorOwner)
			.def("GetTotalPhysicalDefense", &GlobalAttackPoint::GetTotalPhysicalDefense)
			.def("GetTotalMetaphysicalDefense", &GlobalAttackPoint::GetTotalMetaphysicalDefense)
			.def("GetTotalEvadeRating", &GlobalAttackPoint::GetTotalEvadeRating)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_global")
	[
		luabind::class_<GlobalActor>("GlobalActor")
			.def("GetID", &GlobalActor::GetID)
			.def("GetName", &GlobalActor::GetName)

			.def("GetHitPoints", &GlobalActor::GetHitPoints)
			.def("GetMaxHitPoints", &GlobalActor::GetMaxHitPoints)
			.def("GetSkillPoints", &GlobalActor::GetSkillPoints)
			.def("GetMaxSkillPoints", &GlobalActor::GetMaxSkillPoints)
			.def("GetExperienceLevel", &GlobalActor::GetExperienceLevel)
			.def("GetStrength", &GlobalActor::GetStrength)
			.def("GetVigor", &GlobalActor::GetVigor)
			.def("GetFortitude", &GlobalActor::GetFortitude)
			.def("GetProtection", &GlobalActor::GetProtection)
			.def("GetAgility", &GlobalActor::GetAgility)
			.def("GetEvade", &GlobalActor::GetEvade)

			.def("GetTotalPhysicalAttack", &GlobalActor::GetTotalPhysicalAttack)
			.def("GetTotalMetaphysicalAttack", &GlobalActor::GetTotalMetaphysicalAttack)
			.def("GetAttackPoint", &GlobalActor::GetAttackPoint)
// 			.def("GetWeaponEquipped", &GlobalActor::GetWeaponEquipped)
// 			.def("GetArmorEquipped", (GlobalArmor* (GlobalActor::*)(uint32)) &GlobalActor::GetArmorEquipped)
// 			.def("GetAttackPoints", &GlobalActor::GetAttackPoints)
// 			.def("GetElementalAttackBonuses", &GlobalActor::GetElementalAttackBonuses)
// 			.def("GetStatusAttackBonuses", &GlobalActor::GetStatusAttackBonuses)
// 			.def("GetElementalDefenseBonuses", &GlobalActor::GetElementalDefenseBonuses)
// 			.def("GetStatusDefenseBonuses", &GlobalActor::GetStatusDefenseBonuses)

			.def("SetHitPoints", &GlobalActor::SetHitPoints)
			.def("SetSkillPoints", &GlobalActor::SetSkillPoints)
			.def("SetMaxHitPoints", &GlobalActor::SetMaxHitPoints)
			.def("SetMaxSkillPoints", &GlobalActor::SetMaxSkillPoints)
			.def("SetExperienceLevel", &GlobalActor::SetExperienceLevel)
			.def("SetStrength", &GlobalActor::SetStrength)
			.def("SetVigor", &GlobalActor::SetVigor)
			.def("SetFortitude", &GlobalActor::SetFortitude)
			.def("SetProtection", &GlobalActor::SetProtection)
			.def("SetAgility", &GlobalActor::SetAgility)
			.def("SetEvade", &GlobalActor::SetEvade)

			.def("AddHitPoints", &GlobalActor::AddHitPoints)
			.def("SubtractHitPoints", &GlobalActor::SubtractHitPoints)
			.def("AddMaxHitPoints", &GlobalActor::AddMaxHitPoints)
			.def("SubtractMaxHitPoints", &GlobalActor::SubtractMaxHitPoints)
			.def("AddSkillPoints", &GlobalActor::AddSkillPoints)
			.def("SubtractSkillPoints", &GlobalActor::SubtractSkillPoints)
			.def("AddMaxSkillPoints", &GlobalActor::AddMaxSkillPoints)
			.def("SubtractMaxSkillPoints", &GlobalActor::SubtractMaxSkillPoints)
			.def("AddStrength", &GlobalActor::AddStrength)
			.def("SubtractStrength", &GlobalActor::SubtractStrength)
			.def("AddVigor", &GlobalActor::AddVigor)
			.def("SubtractVigor", &GlobalActor::SubtractVigor)
			.def("AddFortitude", &GlobalActor::AddFortitude)
			.def("SubtractFortitude", &GlobalActor::SubtractFortitude)
			.def("AddProtection", &GlobalActor::AddProtection)
			.def("SubtractProtection", &GlobalActor::SubtractProtection)
			.def("AddAgility", &GlobalActor::AddAgility)
			.def("SubtractAgility", &GlobalActor::SubtractAgility)
			.def("AddEvade", &GlobalActor::AddEvade)
			.def("SubtractEvade", &GlobalActor::SubtractEvade)

			.def("IsAlive", &GlobalActor::IsAlive)
// 			.def("EquipWeapon", &GlobalActor::EquipWeapon)
// 			.def("EquipArmor", &GlobalActor::EquipArmor)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_global")
	[
		luabind::class_<GlobalCharacterGrowth>("GlobalCharacterGrowth")
			.def_readwrite("_hit_points_growth", &GlobalCharacterGrowth::_hit_points_growth)
			.def_readwrite("_skill_points_growth", &GlobalCharacterGrowth::_skill_points_growth)
			.def_readwrite("_strength_growth", &GlobalCharacterGrowth::_strength_growth)
			.def_readwrite("_vigor_growth", &GlobalCharacterGrowth::_vigor_growth)
			.def_readwrite("_fortitude_growth", &GlobalCharacterGrowth::_fortitude_growth)
			.def_readwrite("_protection_growth", &GlobalCharacterGrowth::_protection_growth)
			.def_readwrite("_agility_growth", &GlobalCharacterGrowth::_agility_growth)
			.def_readwrite("_evade_growth", &GlobalCharacterGrowth::_evade_growth)
			.def("_AddSkill", &GlobalCharacterGrowth::_AddSkill)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_global")
	[
		luabind::class_<GlobalCharacter, GlobalActor>("GlobalCharacter")
			.def("GetGrowth", &GlobalCharacter::GetGrowth)
			.def("AddSkill", &GlobalCharacter::AddSkill)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_global")
	[
		luabind::class_<GlobalParty>("GlobalParty")
			.def("AddHitPoints", &GlobalParty::AddHitPoints)
			.def("AddSkillPoints", &GlobalParty::AddSkillPoints)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_global")
	[
		luabind::class_<GlobalEnemy, GlobalActor>("GlobalEnemy")
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_global")
	[
		luabind::class_<GlobalObject>("GlobalObject")
			.def("GetID", &GlobalObject::GetID)
			.def("GetName", &GlobalObject::GetName)
			.def("GetType", &GlobalObject::GetObjectType)
			.def("GetCount", &GlobalObject::GetCount)
			.def("IncrementCount", &GlobalObject::IncrementCount)
			.def("DecrementCount", &GlobalObject::DecrementCount)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_global")
	[
		luabind::class_<GlobalItem, GlobalObject>("GlobalItem")
// 			.def(luabind::constructor<>(uint32, uint32))
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_global")
	[
		luabind::class_<GlobalWeapon, GlobalObject>("GlobalWeapon")
			.def("GetUsableBy", &GlobalWeapon::GetUsableBy)
// 			.def(luabind::constructor<>(uint32, uint32))
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_global")
	[
		luabind::class_<GlobalArmor, GlobalObject>("GlobalArmor")
			.def("GetUsableBy", &GlobalArmor::GetUsableBy)
// 			.def(luabind::constructor<>(uint32, uint32))
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_global")
	[
		luabind::class_<GlobalStatusEffect>("GlobalStatusEffect")
			.def("GetType", &GlobalStatusEffect::GetType)
			.def("GetIntensity", &GlobalStatusEffect::GetIntensity)
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_global")
	[
		luabind::class_<GlobalElementalEffect>("GlobalElementalEffect")
	];

	luabind::module(hoa_script::ScriptManager->GetGlobalState(), "hoa_global")
	[
		luabind::class_<GlobalSkill>("GlobalSkill")
			.def("GetID", &GlobalSkill::GetID)
			.def("GetSPRequired", &GlobalSkill::GetSPRequired)
			.def("ExecuteBattleFunction", &GlobalSkill::ExecuteBattleFunction)
	];

	} // End using global namespaces

	// Bind the GlobalManager object to Lua
	luabind::object global_table = luabind::globals(hoa_script::ScriptManager->GetGlobalState());
	global_table["GlobalManager"] = hoa_global::GlobalManager;
} // void BindCommonCode()

} // namespace hoa_defs
