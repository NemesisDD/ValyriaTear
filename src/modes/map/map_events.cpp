///////////////////////////////////////////////////////////////////////////////
//            Copyright (C) 2004-2010 by The Allacrost Project
//                         All Rights Reserved
//
// This code is licensed under the GNU GPL version 2. It is free software
// and you may modify it and/or redistribute it under the terms of this license.
// See http://www.gnu.org/copyleft/gpl.html for details.
///////////////////////////////////////////////////////////////////////////////

/** ****************************************************************************
*** \file    map_events.cpp
*** \author  Tyler Olsen, roots@allacrost.org
*** \brief   Source file for map mode events and event processing.
*** ***************************************************************************/

// Allacrost engines
#include "engine/audio/audio.h"
#include "engine/mode_manager.h"
#include "engine/script/script.h"
#include "engine/system.h"
#include "engine/video/video.h"

// Local map mode headers
#include "modes/map/map.h"
#include "modes/map/map_events.h"
#include "modes/map/map_objects.h"
#include "modes/map/map_sprites.h"

// Other mode headers
#include "modes/shop/shop.h"
#include "modes/battle/battle.h"

using namespace std;

using namespace hoa_audio;
using namespace hoa_mode_manager;
using namespace hoa_script;
using namespace hoa_system;
using namespace hoa_video;

using namespace hoa_battle;
using namespace hoa_shop;

namespace hoa_map {

namespace private_map {


// -----------------------------------------------------------------------------
// ---------- DialogueEvent Class Methods
// -----------------------------------------------------------------------------
void SpriteEvent::_Start() {
    EventSupervisor *event_supervisor = MapMode::CurrentInstance()->GetEventSupervisor();
    // Terminate the previous event whenever it is another sprite event.
    if (dynamic_cast<SpriteEvent*>(_sprite->control_event) && event_supervisor) {
        event_supervisor->TerminateEvents(_sprite->control_event, false);
    }

    _sprite->AcquireControl(this);
}

// -----------------------------------------------------------------------------
// ---------- DialogueEvent Class Methods
// -----------------------------------------------------------------------------

DialogueEvent::DialogueEvent(const std::string& event_id, uint32 dialogue_id) :
	MapEvent(event_id, DIALOGUE_EVENT),
	_dialogue_id(dialogue_id),
	_stop_camera_movement(false)
{}

DialogueEvent::DialogueEvent(const std::string& event_id, SpriteDialogue *dialogue) :
	MapEvent(event_id, DIALOGUE_EVENT),
	_dialogue_id(dialogue->GetDialogueID()),
	_stop_camera_movement(false)
{}


void DialogueEvent::_Start() {
	if (_stop_camera_movement == true) {
		MapMode::CurrentInstance()->GetCamera()->moving = false;
		MapMode::CurrentInstance()->GetCamera()->is_running = false;
	}

	MapMode::CurrentInstance()->GetDialogueSupervisor()->BeginDialogue(_dialogue_id);
}



bool DialogueEvent::_Update() {
	SpriteDialogue* active_dialogue = MapMode::CurrentInstance()->GetDialogueSupervisor()->GetCurrentDialogue();
	if ((active_dialogue != NULL) && (active_dialogue->GetDialogueID() == _dialogue_id))
		return false;
	else
		return true;
}

// -----------------------------------------------------------------------------
// ---------- ShopEvent Class Methods
// -----------------------------------------------------------------------------

ShopEvent::ShopEvent(const std::string& event_id) :
	MapEvent(event_id, SHOP_EVENT)
{}



ShopEvent::~ShopEvent()
{}


void ShopEvent::AddWare(uint32 object_id, uint32 stock) {
	_wares.insert(make_pair(object_id, stock));
}



void ShopEvent::_Start() {
	ShopMode* shop = new ShopMode();
	for (set<pair<uint32, uint32> >::iterator i = _wares.begin(); i != _wares.end(); i++) {
		shop->AddObject((*i).first, (*i).second);
	}
	ModeManager->Push(shop);
}



bool ShopEvent::_Update() {
	return true;
}

// -----------------------------------------------------------------------------
// ---------- SoundEvent Class Methods
// -----------------------------------------------------------------------------

SoundEvent::SoundEvent(const std::string& event_id, const std::string& sound_filename) :
	MapEvent(event_id, SOUND_EVENT)
{
	if (_sound.LoadAudio(sound_filename) == false) {
		IF_PRINT_WARNING(MAP_DEBUG) << "failed to load sound event: " << sound_filename << endl;
	}
}



SoundEvent::~SoundEvent() {
	_sound.Stop();
}



void SoundEvent::_Start() {
	_sound.Play();
}



bool SoundEvent::_Update() {
	if (_sound.GetState() == AUDIO_STATE_STOPPED) {
		// TODO: is it necessary to reset the loop counter and other properties here before returning?
		return true;
	}

	return false;
}

// -----------------------------------------------------------------------------
// ---------- MapTransitionEvent Class Methods
// -----------------------------------------------------------------------------

MapTransitionEvent::MapTransitionEvent(const std::string& event_id,
									   const std::string& filename,
									   const std::string& coming_from) :
	MapEvent(event_id, MAP_TRANSITION_EVENT),
	_transition_map_filename(filename),
	_transition_origin(coming_from),
	_done(false)
{}


void MapTransitionEvent::_Start() {
	MapMode::CurrentInstance()->PushState(STATE_SCENE);

	VideoManager->FadeScreen(Color::black, MAP_FADE_OUT_TIME);
	_done = false;

	// TODO: fade out the map music
}


bool MapTransitionEvent::_Update() {
	if (VideoManager->IsFading())
		return false;

	// Only load the map once the fade out is done, since the load time can
	// break the fade smoothness and visible duration.
	if (!_done) {
		hoa_global::GlobalManager->SetPreviousLocation(_transition_origin);
		MapMode *MM = new MapMode(_transition_map_filename);
		ModeManager->Pop();
		ModeManager->Push(MM, false, true);
		_done = true;
	}
	return true;
}

// -----------------------------------------------------------------------------
// ---------- JoinPartyEvent Class Methods
// -----------------------------------------------------------------------------

JoinPartyEvent::JoinPartyEvent(const std::string& event_id) :
	MapEvent(event_id, JOIN_PARTY_EVENT)
{
	// TODO
}



JoinPartyEvent::~JoinPartyEvent() {
	// TODO
}



void JoinPartyEvent::_Start() {
	// TODO
}



bool JoinPartyEvent::_Update() {
	// TODO
	return true;
}

// -----------------------------------------------------------------------------
// ---------- BattleEncounterEvent Class Methods
// -----------------------------------------------------------------------------

BattleEncounterEvent::BattleEncounterEvent(const std::string& event_id, uint32 enemy_id) :
	MapEvent(event_id, BATTLE_ENCOUNTER_EVENT),
	_battle_music("mus/Confrontation.ogg"),
	_battle_background("img/backdrops/battle/desert.png")
{
	_enemy_ids.push_back(enemy_id);
}


void BattleEncounterEvent::_Start() {
	MapMode::CurrentInstance()->PushState(STATE_SCENE);

	try {
		BattleMode* BM = new BattleMode();
		for (uint32 i = 0; i < _enemy_ids.size(); ++i)
			BM->AddEnemy(_enemy_ids.at(i));

		BM->GetMedia().SetBackgroundImage(_battle_background);
		BM->GetMedia().SetBattleMusic(_battle_music);
		for (uint32 i = 0; i < _battle_scripts.size(); ++i)
			BM->AddBattleScript(_battle_scripts[i]);

		TransitionToBattleMode *TM = new TransitionToBattleMode(BM);

		ModeManager->Push(TM);
	} catch (luabind::error e) {
		PRINT_ERROR << "Error while loading battle encounter event!" << endl;
		ScriptManager->HandleLuaError(e);
	}
}

// -----------------------------------------------------------------------------
// ---------- ScriptedEvent Class Methods
// -----------------------------------------------------------------------------

ScriptedEvent::ScriptedEvent(const std::string& event_id,
							 const std::string& start_function,
							 const std::string& update_function) :
	MapEvent(event_id, SCRIPTED_EVENT),
	_start_function(NULL),
	_update_function(NULL)
{
	ReadScriptDescriptor& map_script = MapMode::CurrentInstance()->GetMapScript();
	MapMode::CurrentInstance()->OpenMapTablespace(true);
	map_script.OpenTable("map_functions");
	if (!start_function.empty()) {
		_start_function = new ScriptObject();
		*_start_function = map_script.ReadFunctionPointer(start_function);
	}
	if (!update_function.empty()) {
		_update_function = new ScriptObject();
		*_update_function = map_script.ReadFunctionPointer(update_function);
	}
	map_script.CloseTable();
	map_script.CloseTable();
}



ScriptedEvent::~ScriptedEvent() {
	if (_start_function != NULL) {
		delete _start_function;
		_start_function = NULL;
	}
	if (_update_function != NULL) {
		delete _update_function;
		_update_function = NULL;
	}
}



ScriptedEvent::ScriptedEvent(const ScriptedEvent& copy) :
	MapEvent(copy)
{
	if (copy._start_function == NULL)
		_start_function = NULL;
	else
		_start_function = new ScriptObject(*copy._start_function);

	if (copy._update_function == NULL)
		_update_function = NULL;
	else
		_update_function = new ScriptObject(*copy._update_function);
}



ScriptedEvent& ScriptedEvent::operator=(const ScriptedEvent& copy) {
	if (this == &copy) // Handle self-assignment case
		return *this;

	MapEvent::operator=(copy);

	if (copy._start_function == NULL)
		_start_function = NULL;
	else
		_start_function = new ScriptObject(*copy._start_function);

	if (copy._update_function == NULL)
		_update_function = NULL;
	else
		_update_function = new ScriptObject(*copy._update_function);

	return *this;
}



void ScriptedEvent::_Start() {
	if (_start_function != NULL)
		ScriptCallFunction<void>(*_start_function);
}



bool ScriptedEvent::_Update() {
	if (_update_function != NULL)
		return ScriptCallFunction<bool>(*_update_function);
	else
		return true;
}

// -----------------------------------------------------------------------------
// ---------- SpriteEvent Class Methods
// -----------------------------------------------------------------------------

SpriteEvent::SpriteEvent(const std::string& event_id, EVENT_TYPE event_type, uint16 sprite_id) :
	MapEvent(event_id, event_type),
	_sprite(NULL)
{
	_sprite = MapMode::CurrentInstance()->GetObjectSupervisor()->GetSprite(sprite_id);
	if (_sprite == NULL)
		IF_PRINT_WARNING(MAP_DEBUG) << "sprite_id argument did not correspond to a known sprite object: " << event_id << endl;
}


SpriteEvent::SpriteEvent(const std::string& event_id, EVENT_TYPE event_type, VirtualSprite* sprite) :
	MapEvent(event_id, event_type),
	_sprite(sprite)
{
	if (sprite == NULL)
		IF_PRINT_WARNING(MAP_DEBUG) << "NULL sprite object passed into constructor: " << event_id << endl;
}

// -----------------------------------------------------------------------------
// ---------- ScriptedSpriteEvent Class Methods
// -----------------------------------------------------------------------------

ScriptedSpriteEvent::ScriptedSpriteEvent(const std::string& event_id, uint16 sprite_id,
										 const std::string& start_function,
										 const std::string& update_function) :
	SpriteEvent(event_id, SCRIPTED_SPRITE_EVENT, sprite_id),
	_start_function(NULL),
	_update_function(NULL)
{
	ReadScriptDescriptor& map_script = MapMode::CurrentInstance()->GetMapScript();
	MapMode::CurrentInstance()->OpenMapTablespace(true);
	map_script.OpenTable("map_functions");
	if (!start_function.empty()) {
		_start_function = new ScriptObject();
		*_start_function = map_script.ReadFunctionPointer(start_function);
	}
	if (!update_function.empty()) {
		_update_function = new ScriptObject();
		*_update_function = map_script.ReadFunctionPointer(update_function);
	}
	map_script.CloseTable();
	map_script.CloseTable();
}



ScriptedSpriteEvent::ScriptedSpriteEvent(const std::string& event_id, VirtualSprite* sprite,
										 const std::string& start_function,
										 const std::string& update_function) :
	SpriteEvent(event_id, SCRIPTED_SPRITE_EVENT, sprite),
	_start_function(NULL),
	_update_function(NULL)
{
	ReadScriptDescriptor& map_script = MapMode::CurrentInstance()->GetMapScript();
	MapMode::CurrentInstance()->OpenMapTablespace(true);
	map_script.OpenTable("map_functions");
	if (!start_function.empty()) {
		_start_function = new ScriptObject();
		*_start_function = map_script.ReadFunctionPointer(start_function);
	}
	if (!update_function.empty()) {
		_update_function = new ScriptObject();
		*_update_function = map_script.ReadFunctionPointer(update_function);
	}
	map_script.CloseTable();
	map_script.CloseTable();
}



ScriptedSpriteEvent::~ScriptedSpriteEvent() {
	if (_start_function != NULL) {
		delete _start_function;
		_start_function = NULL;
	}
	if (_update_function != NULL) {
		delete _update_function;
		_update_function = NULL;
	}
}



ScriptedSpriteEvent::ScriptedSpriteEvent(const ScriptedSpriteEvent& copy) :
	SpriteEvent(copy)
{
	if (copy._start_function == NULL)
		_start_function = NULL;
	else
		_start_function = new ScriptObject(*copy._start_function);

	if (copy._update_function == NULL)
		_update_function = NULL;
	else
		_update_function = new ScriptObject(*copy._update_function);
}



ScriptedSpriteEvent& ScriptedSpriteEvent::operator=(const ScriptedSpriteEvent& copy) {
	if (this == &copy) // Handle self-assignment case
		return *this;

	SpriteEvent::operator=(copy);

	if (copy._start_function == NULL)
		_start_function = NULL;
	else
		_start_function = new ScriptObject(*copy._start_function);

	if (copy._update_function == NULL)
		_update_function = NULL;
	else
		_update_function = new ScriptObject(*copy._update_function);

	return *this;
}



void ScriptedSpriteEvent::_Start() {
	if (_start_function != NULL) {
		SpriteEvent::_Start();
		ScriptCallFunction<void>(*_start_function, _sprite);
	}
}



bool ScriptedSpriteEvent::_Update() {
	bool finished = false;
	if (_update_function != NULL) {
		finished = ScriptCallFunction<bool>(*_update_function, _sprite);
	}
	else {
		finished = true;
	}

	if (finished == true) {
		_sprite->ReleaseControl(this);
	}
	return finished;
}

// -----------------------------------------------------------------------------
// ---------- ChangeDirectionSpriteEvent Class Methods
// -----------------------------------------------------------------------------

ChangeDirectionSpriteEvent::ChangeDirectionSpriteEvent(const std::string& event_id, uint16 sprite_id, uint16 direction) :
	SpriteEvent(event_id, CHANGE_DIRECTION_SPRITE_EVENT, sprite_id),
	_direction(direction)
{
	if ((_direction != NORTH) && (_direction != SOUTH) && (_direction != EAST) && (_direction != WEST))
		IF_PRINT_WARNING(MAP_DEBUG) << "non-standard direction specified: " << event_id << endl;
}



ChangeDirectionSpriteEvent::ChangeDirectionSpriteEvent(const std::string& event_id, VirtualSprite* sprite, uint16 direction) :
	SpriteEvent(event_id, CHANGE_DIRECTION_SPRITE_EVENT, sprite),
	_direction(direction)
{
	if ((_direction != NORTH) && (_direction != SOUTH) && (_direction != EAST) && (_direction != WEST))
		IF_PRINT_WARNING(MAP_DEBUG) << "non-standard direction specified: " << event_id << endl;
}



void ChangeDirectionSpriteEvent::_Start() {
	SpriteEvent::_Start();
	_sprite->SetDirection(_direction);
}



bool ChangeDirectionSpriteEvent::_Update() {
	_sprite->ReleaseControl(this);
	return true;
}

// -----------------------------------------------------------------------------
// ---------- PathMoveSpriteEvent Class Methods
// -----------------------------------------------------------------------------

PathMoveSpriteEvent::PathMoveSpriteEvent(const std::string& event_id, uint16 sprite_id,
										 float x_coord, float y_coord, bool run) :
	SpriteEvent(event_id, PATH_MOVE_SPRITE_EVENT, sprite_id),
	_relative_destination(false),
	_destination_x(x_coord),
	_destination_y(y_coord),
	_last_x_position(0.0f),
	_last_y_position(0.0f),
	_current_node_x(0.0f),
	_current_node_y(0.0f),
	_current_node(0),
	_run(run)
{}


PathMoveSpriteEvent::PathMoveSpriteEvent(const std::string& event_id, VirtualSprite* sprite,
										 float x_coord, float y_coord, bool run) :
	SpriteEvent(event_id, PATH_MOVE_SPRITE_EVENT, sprite),
	_relative_destination(false),
	_destination_x(x_coord),
	_destination_y(y_coord),
	_last_x_position(0.0f),
	_last_y_position(0.0f),
	_current_node_x(0.0f),
	_current_node_y(0.0f),
	_current_node(0),
	_run(run)
{}


void PathMoveSpriteEvent::SetRelativeDestination(bool relative) {
	if (MapMode::CurrentInstance()->GetEventSupervisor()->IsEventActive(GetEventID()) == true) {
		IF_PRINT_WARNING(MAP_DEBUG) << "attempted illegal operation while event was active: " << GetEventID() << endl;
		return;
	}

	_relative_destination = relative;
	_path.clear();
}


void PathMoveSpriteEvent::SetDestination(float x_coord, float y_coord, bool run) {
	if (MapMode::CurrentInstance()->GetEventSupervisor()->IsEventActive(GetEventID()) == true) {
		IF_PRINT_WARNING(MAP_DEBUG) << "attempted illegal operation while event was active: " << GetEventID() << endl;
		return;
	}

	_destination_x = x_coord;
	_destination_y = y_coord;
	_path.clear();
	_run = run;
}


void PathMoveSpriteEvent::_Start() {
	SpriteEvent::_Start();

	_current_node = 0;
	_last_x_position = _sprite->GetXPosition();
	_last_y_position = _sprite->GetYPosition();
	_sprite->is_running = _run;

	// Set and check the destination position
	if (_relative_destination) {
		// Add The integer part of the source node, but keep the destination offset untouched.
		_destination_x = hoa_utils::GetFloatInteger(_destination_x)
						+ hoa_utils::GetFloatInteger(_last_x_position) + hoa_utils::GetFloatFraction(_destination_x);
		_destination_y = hoa_utils::GetFloatInteger(_destination_y)
						+ hoa_utils::GetFloatInteger(_last_y_position) + hoa_utils::GetFloatFraction(_destination_y);
	}
	MapPosition dest(_destination_x, _destination_y);

	_path = MapMode::CurrentInstance()->GetObjectSupervisor()->FindPath(_sprite, dest);
	if (_path.empty()) {
		PRINT_ERROR << "No path to destination (" << _destination_x
					<< ", " << _destination_y << ") for sprite: "
					<< _sprite->GetObjectID() << endl;
		return;
	}

	_current_node_x = _path[_current_node].x;
	_current_node_y = _path[_current_node].y;

	_sprite->moving = true;
}



bool PathMoveSpriteEvent::_Update() {
	if (_path.empty()) {
		// No path
		_sprite->moving = false;
		_sprite->ReleaseControl(this);
		return true;
	}

	float sprite_position_x = _sprite->GetXPosition();
	float sprite_position_y = _sprite->GetYPosition();
	float distance_moved = _sprite->CalculateDistanceMoved();

	// Check whether the sprite has arrived at the position of the current node
	if (hoa_utils::IsFloatEqual(sprite_position_x, _current_node_x, distance_moved)
			&& hoa_utils::IsFloatEqual(sprite_position_y, _current_node_y, distance_moved)) {
		++_current_node;

		if (_current_node < _path.size()) {
			_current_node_x = _path[_current_node].x;
			_current_node_y = _path[_current_node].y;
		}
	}
	// If the sprite has moved to a new position other than the next node, adjust its direction so it is trying to move to the next node
	else if ((_sprite->position.x != _last_x_position) || (_sprite->position.y != _last_y_position)) {
		_last_x_position = _sprite->position.x;
		_last_y_position = _sprite->position.y;
	}

	_SetSpriteDirection();

	// End the path event
	if (hoa_utils::IsFloatEqual(sprite_position_x, _destination_x, distance_moved)
		&& hoa_utils::IsFloatEqual(sprite_position_y, _destination_y, distance_moved)) {
		_sprite->moving = false;
		_sprite->ReleaseControl(this);
		return true;
	}

	return false;
}



void PathMoveSpriteEvent::_SetSpriteDirection() {
	uint16 direction = 0;

	float sprite_position_x = _sprite->GetXPosition();
	float sprite_position_y = _sprite->GetYPosition();
	float distance_moved = _sprite->CalculateDistanceMoved();

	if (sprite_position_y - _current_node_y > distance_moved) {
		direction |= NORTH;
	}
	else if (sprite_position_y - _current_node_y < -distance_moved) {
		direction |= SOUTH;
	}

	if (sprite_position_x - _current_node_x > distance_moved) {
		direction |= WEST;
	}
	else if (sprite_position_x - _current_node_x < -distance_moved) {
		direction |= EAST;
	}

	// Determine if the sprite should move diagonally to the next node
	if ((direction & (NORTH | SOUTH)) && (direction & (WEST | EAST))) {
		switch (direction) {
			case (NORTH | WEST):
				direction = MOVING_NORTHWEST;
				break;
			case (NORTH | EAST):
				direction = MOVING_NORTHEAST;
				break;
			case (SOUTH | WEST):
				direction = MOVING_SOUTHWEST;
				break;
			case (SOUTH | EAST):
				direction = MOVING_SOUTHEAST;
				break;
		}
	}

	_sprite->SetDirection(direction);
}



void PathMoveSpriteEvent::_ResolveCollision(COLLISION_TYPE coll_type, MapObject* coll_obj) {
	// Boundary and grid collisions should not occur on a pre-calculated path. If these conditions do occur,
	// we terminate the path event immediately. The conditions may occur if, for some reason, the map's boundaries
	// or collision grid are modified after the path is calculated
	if (coll_type == WALL_COLLISION) {
		if (MapMode::CurrentInstance()->GetObjectSupervisor()->AdjustSpriteAroundCollision(_sprite, coll_type, coll_obj) == false) {
			IF_PRINT_WARNING(MAP_DEBUG) << "boundary or grid collision occurred on a pre-calculated path movement" << endl;
		}
		// Wait
// 		_path.clear(); // This path is obviously not a correct one so we should trash it
// 		_sprite->ReleaseControl(this);
// 		MapMode::CurrentInstance()->GetEventSupervisor()->TerminateEvent(GetEventID());
		return;
	}

	// If the code has reached this point, then we are dealing with an object collision

	// Determine if the obstructing object is blocking the destination of this path
	bool destination_blocked = MapMode::CurrentInstance()->GetObjectSupervisor()->IsPositionOccupiedByObject(_destination_x, _destination_y, coll_obj);

	switch (coll_obj->GetObjectType()) {
		case PHYSICAL_TYPE:
		case TREASURE_TYPE:
			// If the object is a static map object and blocking the destination, give up and terminate the event
			if (destination_blocked == true) {
				IF_PRINT_WARNING(MAP_DEBUG) << "path destination was blocked by a non-sprite map object" << endl;
				_path.clear(); // This path is obviously not a correct one so we should trash it
			}
			// Otherwise, try to find an alternative path around the object
			else {
				// TEMP: try a movement adjustment to get around the object
				MapMode::CurrentInstance()->GetObjectSupervisor()->AdjustSpriteAroundCollision(_sprite, coll_type, coll_obj);
				// TODO: recalculate and find an alternative path around the object
			}
			break;

		case VIRTUAL_TYPE:
		case SPRITE_TYPE:
		case ENEMY_TYPE:
			if (destination_blocked == true) {
				// Do nothing but wait for the obstructing sprite to move out of the way
				return;

				// TODO: maybe we should use a timer here to determine if a certain number of seconds have passed while waiting for the obstructiong
				// sprite to move. If that timer expires and the destination is still blocked by the sprite, we could give up on reaching the
				// destination and terminate the path event
			}

			else {
				// TEMP: try a movement adjustment to get around the object
				MapMode::CurrentInstance()->GetObjectSupervisor()->AdjustSpriteAroundCollision(_sprite, coll_type, coll_obj);
			}
			break;

		default:
			IF_PRINT_WARNING(MAP_DEBUG) << "collision object was of an unknown object type: " << coll_obj->GetObjectType() << endl;
	}
} // void PathMoveSpriteEvent::_ResolveCollision(COLLISION_TYPE coll_type, MapObject* coll_obj)

// -----------------------------------------------------------------------------
// ---------- RandomMoveSpriteEvent Class Methods
// -----------------------------------------------------------------------------

RandomMoveSpriteEvent::RandomMoveSpriteEvent(const std::string& event_id, VirtualSprite* sprite,
											 uint32 move_time, uint32 direction_time) :
	SpriteEvent(event_id, RANDOM_MOVE_SPRITE_EVENT, sprite),
	_total_movement_time(move_time),
	_total_direction_time(direction_time),
	_movement_timer(0),
	_direction_timer(0)
{}



RandomMoveSpriteEvent::~RandomMoveSpriteEvent()
{}



void RandomMoveSpriteEvent::_Start() {
	SpriteEvent::_Start();
	_sprite->SetRandomDirection();
	_sprite->moving = true;
}



bool RandomMoveSpriteEvent::_Update() {
	_direction_timer += SystemManager->GetUpdateTime();
	_movement_timer += SystemManager->GetUpdateTime();

	// Check if we should change the sprite's direction
	if (_direction_timer >= _total_direction_time) {
		_direction_timer -= _total_direction_time;
		_sprite->SetRandomDirection();
	}

	if (_movement_timer >= _total_movement_time) {
		_movement_timer = 0;
		_sprite->moving = false;
		_sprite->ReleaseControl(this);
		return true;
	}

	return false;
}



void RandomMoveSpriteEvent::_ResolveCollision(COLLISION_TYPE coll_type, MapObject* coll_obj) {
	// Try to adjust the sprite's position around the collision. If that fails, change the sprite's direction
	if (MapMode::CurrentInstance()->GetObjectSupervisor()->AdjustSpriteAroundCollision(_sprite, coll_type, coll_obj) == false) {
		_sprite->SetRandomDirection();
	}
}

// -----------------------------------------------------------------------------
// ---------- AnimateSpriteEvent Class Methods
// -----------------------------------------------------------------------------

AnimateSpriteEvent::AnimateSpriteEvent(const std::string& event_id, VirtualSprite* sprite) :
	SpriteEvent(event_id, ANIMATE_SPRITE_EVENT, sprite),
	_current_frame(0),
	_display_timer(0),
	_loop_count(0),
	_number_loops(0)
{}



AnimateSpriteEvent::~AnimateSpriteEvent()
{}



void AnimateSpriteEvent::_Start() {
	SpriteEvent::_Start();
	_current_frame = 0;
	_display_timer = 0;
	_loop_count = 0;
	dynamic_cast<MapSprite*>(_sprite)->SetCustomAnimation(true);
	dynamic_cast<MapSprite*>(_sprite)->SetCurrentAnimation(static_cast<uint8>(_frames[_current_frame]));
}



bool AnimateSpriteEvent::_Update() {
	_display_timer += SystemManager->GetUpdateTime();

	if (_display_timer > _frame_times[_current_frame]) {
		_display_timer = 0;
		_current_frame++;

		// Check if we are past the final frame to display in the loop
		if (_current_frame >= _frames.size()) {
			_current_frame = 0;

			// If this animation is not infinitely looped, increment the loop counter
			if (_number_loops >= 0) {
				_loop_count++;
				if (_loop_count > _number_loops) {
					_loop_count = 0;
					dynamic_cast<MapSprite*>(_sprite)->SetCustomAnimation(false);
					_sprite->ReleaseControl(this);
					return true;
				 }
			}
		}

		dynamic_cast<MapSprite*>(_sprite)->SetCurrentAnimation(static_cast<uint8>(_frames[_current_frame]));
	}

	return false;
}


// -----------------------------------------------------------------------------
// ---------- EventSupervisor Class Methods
// -----------------------------------------------------------------------------

EventSupervisor::~EventSupervisor() {
	_active_events.clear();
	_paused_events.clear();
	_active_delayed_events.clear();
	_paused_delayed_events.clear();

	for (std::map<std::string, MapEvent*>::iterator it = _all_events.begin(); it != _all_events.end(); ++it) {
		delete it->second;
	}
	_all_events.clear();
}



void EventSupervisor::RegisterEvent(MapEvent* new_event) {
	if (new_event == NULL) {
		IF_PRINT_WARNING(MAP_DEBUG) << "function argument was NULL" << endl;
		return;
	}

	if (GetEvent(new_event->_event_id) != NULL) {
		IF_PRINT_WARNING(MAP_DEBUG) << "event with this ID already existed: " << new_event->_event_id << endl;
		return;
	}

	_all_events.insert(make_pair(new_event->_event_id, new_event));
}



void EventSupervisor::StartEvent(const std::string& event_id) {
	MapEvent* event = GetEvent(event_id);
	if (event == NULL) {
		IF_PRINT_WARNING(MAP_DEBUG) << "no event with this ID existed: " << event_id << endl;
		return;
	}

	StartEvent(event);
}


void EventSupervisor::StartEvent(const std::string& event_id, uint32 launch_time) {
	MapEvent* event = GetEvent(event_id);
	if (event == NULL) {
		IF_PRINT_WARNING(MAP_DEBUG) << "no event with this ID existed: " << event_id << endl;
		return;
	}

	if (launch_time == 0)
		StartEvent(event);
	else
		_active_delayed_events.push_back(make_pair(static_cast<int32>(launch_time), event));
}


void EventSupervisor::StartEvent(MapEvent* event) {
	if (event == NULL) {
		IF_PRINT_WARNING(MAP_DEBUG) << "NULL argument passed to function" << endl;
		return;
	}

	_active_events.push_back(event);
	event->_Start();
	_ExamineEventLinks(event, true);
}


void EventSupervisor::StartEvent(MapEvent* event, uint32 launch_time) {
	if (event == NULL) {
		IF_PRINT_WARNING(MAP_DEBUG) << "NULL argument passed to function" << endl;
		return;
	}

	if (launch_time == 0)
		StartEvent(event);
	else
		_active_delayed_events.push_back(make_pair(static_cast<int32>(launch_time), event));
}


void EventSupervisor::PauseEvents(const std::string& event_id) {
	// Search for active ones
	for (list<MapEvent*>::iterator it = _active_events.begin(); it != _active_events.end(); ++it) {
		if ((*it)->_event_id == event_id) {
			_paused_events.push_back(*it);
			it = _active_events.erase(it);
		}
	}

	// and for the delayed ones
	for (std::list<std::pair<int32, MapEvent*> >::iterator it = _active_delayed_events.begin();
			it != _active_delayed_events.end(); ++it) {
		if ((*it).second->_event_id == event_id) {
			_paused_delayed_events.push_back(*it);
			it = _active_delayed_events.erase(it);
		}
	}
}


void EventSupervisor::PauseAllEvents(VirtualSprite *sprite) {
	if (!sprite)
		return;
	// Examine all potential active (now or later) events

	// Starting by active ones.
	for (std::list<MapEvent*>::iterator it = _active_events.begin(); it != _active_events.end(); ++it) {
		SpriteEvent *event = dynamic_cast<SpriteEvent*>(*it);
		if (event && event->GetSprite() == sprite) {
			_paused_events.push_back(*it);
			it = _active_events.erase(it);
		}
	}

	// Looking at incoming ones.
	for (std::list<std::pair<int32, MapEvent*> >::iterator it = _active_delayed_events.begin();
			it != _active_delayed_events.end(); ++it)
	{
		SpriteEvent *event = dynamic_cast<SpriteEvent*>((*it).second);
		if (event && event->GetSprite() == sprite) {
			_paused_delayed_events.push_back(*it);
			it = _active_delayed_events.erase(it);
		}
	}
}


void EventSupervisor::ResumeEvents(const std::string& event_id) {
	for (std::list<MapEvent*>::iterator it = _paused_events.begin();
			it != _paused_events.end(); ++it) {
		if ((*it)->_event_id == event_id) {
			_active_events.push_back(*it);
			it = _paused_events.erase(it);
		}
	}

	// and the delayed ones
	for (std::list<std::pair<int32, MapEvent*> >::iterator it = _paused_delayed_events.begin();
			it != _paused_delayed_events.end(); ++it) {
		if ((*it).second->_event_id == event_id) {
			_active_delayed_events.push_back(*it);
			it = _paused_delayed_events.erase(it);
		}
	}
}


void EventSupervisor::ResumeAllEvents(VirtualSprite *sprite) {
	if (!sprite)
		return;
	// Examine all potential active (now or later) events

	// Starting by active ones.
	for (std::list<MapEvent*>::iterator it = _paused_events.begin(); it != _paused_events.end(); ++it) {
		SpriteEvent *event = dynamic_cast<SpriteEvent*>(*it);
		if (event && event->GetSprite() == sprite) {
			_active_events.push_back(*it);
			it = _paused_events.erase(it);
		}
	}

	// Looking at incoming ones.
	for (std::list<std::pair<int32, MapEvent*> >::iterator it = _paused_delayed_events.begin();
			it != _paused_delayed_events.end(); ++it)
	{
		SpriteEvent *event = dynamic_cast<SpriteEvent*>((*it).second);
		if (event && event->GetSprite() == sprite) {
			_active_delayed_events.push_back(*it);
			it = _paused_delayed_events.erase(it);
		}
	}
}


void EventSupervisor::TerminateEvents(const std::string& event_id, bool trigger_event_links) {
	// Examine all potential active (now or later) events

	// Starting by active ones.
	for (std::list<MapEvent*>::iterator it = _active_events.begin(); it != _active_events.end(); ++it) {
		if ((*it)->_event_id == event_id) {
			MapEvent* terminated_event = *it;
			it = _active_events.erase(it);
			// We examine the event links only after the event has been removed from the active list
			if (trigger_event_links)
			    _ExamineEventLinks(terminated_event, false);
		}
	}

	// Looking at incoming ones.
	for (std::list<std::pair<int32, MapEvent*> >::iterator it = _active_delayed_events.begin();
			it != _active_delayed_events.end(); ++it)
	{
		if ((*it).second->_event_id == event_id) {
			MapEvent* terminated_event = (*it).second;
			it = _active_delayed_events.erase(it);

			// We examine the event links only after the event has been removed from the active list
			if (trigger_event_links)
				_ExamineEventLinks(terminated_event, false);
		}
	}

	// And paused ones
	for (std::list<MapEvent*>::iterator it = _paused_events.begin(); it != _paused_events.end(); ++it) {
		if ((*it)->_event_id == event_id) {
			MapEvent* terminated_event = *it;
			it = _paused_events.erase(it);
			// We examine the event links only after the event has been removed from the list
			if (trigger_event_links)
				_ExamineEventLinks(terminated_event, false);
		}
	}

	for (std::list<std::pair<int32, MapEvent*> >::iterator it = _paused_delayed_events.begin();
			it != _paused_delayed_events.end(); ++it)
	{
		if ((*it).second->_event_id == event_id) {
			MapEvent* terminated_event = (*it).second;
			it = _paused_delayed_events.erase(it);

			// We examine the event links only after the event has been removed from the list
			if (trigger_event_links)
				_ExamineEventLinks(terminated_event, false);
		}
	}
}


void EventSupervisor::TerminateEvents(MapEvent *event, bool trigger_event_links) {
	if (!event) {
		PRINT_ERROR << "Couldn't terminate NULL event" << endl;
		return;
	}

	TerminateEvents(event->GetEventID(), trigger_event_links);
}


void EventSupervisor::TerminateAllEvents(VirtualSprite *sprite) {
	if (!sprite)
		return;
	// Examine all potential active (now or later) events

	// Starting by active ones.
	for (std::list<MapEvent*>::iterator it = _active_events.begin(); it != _active_events.end(); ++it) {
		SpriteEvent *event = dynamic_cast<SpriteEvent*>(*it);
		if (event && event->GetSprite() == sprite)
			it = _active_events.erase(it);
	}

	// Looking at incoming ones.
	for (std::list<std::pair<int32, MapEvent*> >::iterator it = _active_delayed_events.begin();
			it != _active_delayed_events.end(); ++it)
	{
		SpriteEvent *event = dynamic_cast<SpriteEvent*>((*it).second);
		if (event && event->GetSprite() == sprite)
			it = _active_delayed_events.erase(it);
	}

	// And paused ones
	for (std::list<MapEvent*>::iterator it = _paused_events.begin(); it != _paused_events.end(); ++it) {
		SpriteEvent *event = dynamic_cast<SpriteEvent*>(*it);
		if (event && event->GetSprite() == sprite)
			it = _paused_events.erase(it);
	}

	for (std::list<std::pair<int32, MapEvent*> >::iterator it = _paused_delayed_events.begin();
			it != _paused_delayed_events.end(); ++it)
	{
		SpriteEvent *event = dynamic_cast<SpriteEvent*>((*it).second);
		if (event && event->GetSprite() == sprite)
			it = _paused_delayed_events.erase(it);
	}
}


void EventSupervisor::Update() {
	// Update all launch event timers and start all events whose timers have finished
	for (list<pair<int32, MapEvent*> >::iterator it = _active_delayed_events.begin();
			it != _active_delayed_events.end();) {
		it->first -= SystemManager->GetUpdateTime();

		if (it->first <= 0) { // Timer has expired
			MapEvent* start_event = it->second;
			it = _active_delayed_events.erase(it);
			// We begin the event only after it has been removed from the launch list
			StartEvent(start_event);
		}
		else
			++it;
	}

	// Check for active events which have finished
	for (list<MapEvent*>::iterator it = _active_events.begin(); it != _active_events.end();) {
		if ((*it)->_Update() == true) {
			MapEvent* finished_event = *it;
			it = _active_events.erase(it);
			// We examine the event links only after the event has been removed from the active list
			_ExamineEventLinks(finished_event, false);
		}
		else
			++it;
	}
}



bool EventSupervisor::IsEventActive(const std::string& event_id) const {
	for (list<MapEvent*>::const_iterator i = _active_events.begin(); i != _active_events.end(); i++) {
		if ((*i)->_event_id == event_id) {
			return true;
		}
	}
	return false;
}



MapEvent* EventSupervisor::GetEvent(const std::string& event_id) const {
	std::map<std::string, MapEvent*>::const_iterator it = _all_events.find(event_id);

	if (it == _all_events.end())
		return NULL;
	else
		return it->second;
}



void EventSupervisor::_ExamineEventLinks(MapEvent* parent_event, bool event_start) {
	for (uint32 i = 0; i < parent_event->_event_links.size(); i++) {
		EventLink& link = parent_event->_event_links[i];

		// Case 1: Start/finish launch member is not equal to the start/finish status of the parent event, so ignore this link
		if (link.launch_at_start != event_start) {
			continue;
		}
		// Case 2: The child event is to be launched immediately
		else if (link.launch_timer == 0) {
			StartEvent(link.child_event_id);
		}
		// Case 3: The child event has a timer associated with it and needs to be placed in the event launch container
		else {
			MapEvent* child = GetEvent(link.child_event_id);
			if (child == NULL) {
				IF_PRINT_WARNING(MAP_DEBUG) << "can not launch child event, no event with this ID existed: " << link.child_event_id << endl;
				continue;
			}
			else {
				_active_delayed_events.push_back(make_pair(static_cast<int32>(link.launch_timer), child));
			}
		}
	}
}

} // namespace private_map

} // namespace hoa_map
