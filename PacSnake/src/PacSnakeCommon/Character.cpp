#include "Fpch.h"
#include "Character.h"
#include "../PacSnakeCommon/GridSystem.h"
#include "Systems/TransformComponent.h"

RTTI_IMPLEMENT_TYPE( pacsnake::Character );

void pacsnake::Character::TurnLeft()
{
	auto& gridSystem = GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
	GridPawn& pawn = GetPawn();
	pawn.m_dir = GetDirRotatedLeft( pawn.m_dir );
}

void pacsnake::Character::TurnRight()
{
	auto& gridSystem = GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
	GridPawn& pawn = GetPawn();
	pawn.m_dir = GetDirRotatedRight( pawn.m_dir );
}

const Vector2& pacsnake::Character::GetDir() const
{
	return GetPawn().m_dir;
}

forge::CallbackToken pacsnake::Character::RegisterOnBeforeActionTaken( std::function< void() > func )
{
	return m_onBeforeActionTaken.AddListener( std::move( func ) );
}

pacsnake::Character::Action pacsnake::Character::TranslateDirectionToAction( const Vector2& currentDir, const Vector2& newDir )
{
	const Float crossProduct = Vector3( currentDir, 0.0f ).Cross( Vector3( newDir, 0.0f ) ).Z;

	if ( crossProduct > 0.0f )
	{
		return pacsnake::Character::Action::TurnLeft;
	}
	else if ( crossProduct < 0.0f )
	{
		return pacsnake::Character::Action::TurnRight;
	}

	return pacsnake::Character::Action::None;
}

Vector2 pacsnake::Character::GetDirRotatedRight( const Vector2& dir )
{
	Vector2 newDir;
	if ( dir == Vector2( 1.0f, 0.0f ) )
	{
		newDir = Vector2( 0.0f, -1.0f );
	}
	else if ( dir == Vector2( 0.0f, -1.0f ) )
	{
		newDir = Vector2( -1.0f, 0.0f );
	}
	else if ( dir == Vector2( -1.0f, 0.0f ) )
	{
		newDir = Vector2( 0.0f, 1.0f );
	}
	else if ( dir == Vector2( 0.0f, 1.0f ) )
	{
		newDir = Vector2( 1.0f, 0.0f );
	}
	else
	{
		FORGE_ASSERT( false );
	}

	return newDir;
}

Vector2 pacsnake::Character::GetDirRotatedLeft( const Vector2& dir )
{
	Vector2 res = GetDirRotatedRight( dir );
	res = GetDirRotatedRight( res );
	return GetDirRotatedRight( res );
}

void pacsnake::Character::OnBeforeSimUpdated()
{
	Super::OnBeforeSimUpdated();

	m_onBeforeActionTaken.Invoke();

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
