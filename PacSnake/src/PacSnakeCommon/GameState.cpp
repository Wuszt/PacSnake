#include "Fpch.h"
#include "GameState.h"

Vector2 GenerateUnoccupiedPos( const pacsnake::Grid& grid )
{
	const Int32 maxX = static_cast< Int32 >( grid.GetWidth() / 2u );
	const Int32 maxY = static_cast< Int32 >( grid.GetHeight() / 2u );

	Vector2 pos;
	Bool collides = false;

	do
	{
		collides = false;
		pos = Vector2( static_cast< Float >( Math::Random::GetRNG().GetInteger( -maxX, maxX ) ), static_cast< Float >( Math::Random::GetRNG().GetInteger( -maxY, maxY ) ) );
		for ( const pacsnake::GridPawn& pawn : grid.GetPawns() )
		{
			if ( pawn.m_pos == pos )
			{
				collides = true;
				break;
			}
		}
	} while( collides );

	return pos;
}

pacsnake::GameState::GameState( Uint32 gridHeight, Uint32 gridWidth )
	: m_grid( gridHeight, gridWidth )
{
	m_pickupID = m_grid.AddPawn( GenerateUnoccupiedPos( m_grid ) );
}

void pacsnake::GameState::Update()
{
	if ( IsFinished() )
	{
		return;
	}

	m_grid.Update();
	auto collisions = m_grid.FindCollisions();
	for ( const auto& collision : collisions )
	{
		if ( collision.m_first == m_pickupID )
		{
			OnPickupGrabbed( collision.m_second );
		}
		else if ( collision.m_second == m_pickupID )
		{
			OnPickupGrabbed( collision.m_first );
		}
		else
		{
			m_isFinished = true;
		}
	}

	for ( const GridPawn& pawn : m_grid.GetPawns() )
	{
		if ( Math::Abs( pawn.m_pos.X ) > m_grid.GetWidth() / 2 || Math::Abs( pawn.m_pos.Y ) > m_grid.GetHeight() / 2 )
		{
			m_isFinished = true;
		}
	}
}

forge::CallbackToken pacsnake::GameState::RegisterOnNewTail( std::function< void( pacsnake::GridPawnID ) > func )
{
	return m_onNewTail.AddListener( std::move( func ) );
}

void pacsnake::GameState::OnPickupGrabbed( pacsnake::GridPawnID grabberID )
{
	auto* tailAttachment = m_grid.GetPawn( grabberID );
	
	while ( tailAttachment->m_nextTailID.IsValid() )
	{
		tailAttachment = m_grid.GetPawn( tailAttachment->m_nextTailID );
	}
	auto tailAttachmentID = tailAttachment->m_id;

	auto newTail = m_grid.GetPawn( m_grid.AddPawn( tailAttachment->m_prevPos ) );
	tailAttachment = m_grid.GetPawn( tailAttachmentID );


	newTail->m_nextTailID = tailAttachment->m_nextTailID;
	tailAttachment->m_nextTailID = newTail->m_id;

	m_onNewTail.Invoke( newTail->m_id );
	GetGrid().GetPawn( m_pickupID )->m_pos = GenerateUnoccupiedPos( m_grid );
}
