#include "Fpch.h"
#include "PlayerCharacterComponent.h"
#include "Character.h"
#include "Systems/PlayerSystem.h"
#include "Systems/InputSystem.h"

RTTI_IMPLEMENT_TYPE( pacsnake::PlayerCharacterComponent );

void pacsnake::PlayerCharacterComponent::Update()
{
	auto& playerSystem = GetOwner().GetEngineInstance().GetSystemsManager().GetSystem< systems::PlayerSystem >();
	const auto& inputHandler = playerSystem.GetInputHandler();

	pacsnake::Character& character = rtti::CastChecked< pacsnake::Character >( GetOwner() );

	Vector2 dir;
	if ( inputHandler.GetKeyDown( forge::InputHandler::Key::LeftArrow ) )
	{
		dir = Vector2( -1.0f, 0.0f );
	}
	else if ( inputHandler.GetKeyDown( forge::InputHandler::Key::RightArrow ) )
	{
		dir = Vector2( 1.0f, 0.0f );
	}
	else if ( inputHandler.GetKeyDown( forge::InputHandler::Key::UpArrow ) )
	{
		dir = Vector2( 0.0f, 1.0f );
	}
	else if ( inputHandler.GetKeyDown( forge::InputHandler::Key::DownArrow ) )
	{
		dir = Vector2( 0.0f, -1.0f );
	}

	pacsnake::Character::Action action = Character::TranslateDirectionToAction( character.GetDir(), dir );
	if ( action != pacsnake::Character::Action::None )
	{
		character.ScheduleAction( action );
	}
}
