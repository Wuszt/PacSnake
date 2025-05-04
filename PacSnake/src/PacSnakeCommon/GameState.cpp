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
	m_pickupID = m_grid.AddPawn( { -static_cast< Float >( gridWidth / 2 ), 0.0f } );
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

	if ( IsFinished() )
	{
		FORGE_LOG( "Game Over" );
	}
}

forge::CallbackToken pacsnake::GameState::RegisterOnNewTail( std::function< void( pacsnake::GridPawnID ) > func )
{
	return m_onNewTail.AddListener( std::move( func ) );
}

pacsnake::GridPawn& pacsnake::GameState::GetPickupPawn()
{
	return *GetGrid().GetPawn( GetPickupID() );
}

pacsnake::GridPawnID pacsnake::GameState::GetLastTail( pacsnake::GridPawnID owner ) const
{
	auto* pawn = m_grid.GetPawn( owner );
	if ( pawn == nullptr )
	{
		return {};
	}

	if ( pawn->m_nextTailID.IsValid() )
	{
		return GetLastTail( pawn->m_nextTailID );
	}

	return owner;
}

std::vector< pacsnake::GameState::Score > pacsnake::GameState::CalculateScores() const
{
	std::vector< pacsnake::GameState::Score > scores;
	for ( const auto& pawn : m_grid.GetPawns() )
	{
		if ( pawn.m_growsTail )
		{
			scores.push_back( { pawn.m_id, pawn.m_tailLength });
		}
	}

	return scores;
}

void pacsnake::GameState::OnPickupGrabbed( pacsnake::GridPawnID grabberID )
{
	auto* tailAttachment = m_grid.GetPawn( grabberID );
	++tailAttachment->m_tailLength;
	
	while ( tailAttachment->m_nextTailID.IsValid() )
	{
		tailAttachment = m_grid.GetPawn( tailAttachment->m_nextTailID );
		++tailAttachment->m_tailLength;
	}
	auto tailAttachmentID = tailAttachment->m_id;

	auto newTail = m_grid.GetPawn( m_grid.AddPawn( tailAttachment->m_prevPos ) );
	tailAttachment = m_grid.GetPawn( tailAttachmentID );

	newTail->m_nextTailID = tailAttachment->m_nextTailID;
	tailAttachment->m_nextTailID = newTail->m_id;

	if ( m_grid.GetPawns().GetSize() > m_grid.GetWidth() * m_grid.GetHeight() )
	{
		m_isFinished = true;
		FORGE_LOG( "Win!" );
	}
	else
	{
		GetGrid().GetPawn( m_pickupID )->m_pos = GenerateUnoccupiedPos( m_grid );
		FORGE_LOG( "New pickup pos: %s", GetGrid().GetPawn( m_pickupID )->m_pos.ToDebugString().c_str() );
	}

	m_onNewTail.Invoke( newTail->m_id );
}
