#include "Fpch.h"
#include "Character.h"
#include "../PacSnakeCommon/GridSystem.h"
#include "Systems/TransformComponent.h"

RTTI_IMPLEMENT_TYPE( pacsnake::Character );

void pacsnake::Character::TurnLeft()
{
	TurnRight();
	TurnRight();
	TurnRight();
}

void pacsnake::Character::TurnRight()
{
	auto& gridSystem = GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
	GridPawn& pawn = GetPawn();
	if ( pawn.m_dir == Vector2( 1.0f, 0.0f ) )
	{
		pawn.m_dir = Vector2( 0.0f, -1.0f );
	}
	else if ( pawn.m_dir == Vector2( 0.0f, -1.0f ) )
	{
		pawn.m_dir = Vector2( -1.0f, 0.0f );
	}
	else if ( pawn.m_dir == Vector2( -1.0f, 0.0f ) )
	{
		pawn.m_dir = Vector2( 0.0f, 1.0f );
	}
	else if ( pawn.m_dir == Vector2( 0.0f, 1.0f ) )
	{
		pawn.m_dir = Vector2( 1.0f, 0.0f );
	}
}

const Vector2& pacsnake::Character::GetDir() const
{
	return GetPawn().m_dir;
}

void pacsnake::Character::OnBeforeSimUpdated()
{
	Super::OnBeforeSimUpdated();

	switch ( m_scheduledAction )
	{
	case Action::None:
		break;

	case Action::TurnLeft:
		TurnLeft();
		break;

	case Action::TurnRight:
		TurnRight();
		break;
	default:
		FORGE_ASSERT( false );
	}

	m_scheduledAction = Action::None;
}
