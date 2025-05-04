#include "Fpch.h"
#include "Grid.h"

pacsnake::GridPawnID pacsnake::Grid::AddPawn( const Vector2& pos )
{
	return m_pawns.emplace_back( pos ).m_id;
}

pacsnake::GridPawn* pacsnake::Grid::GetPawn( GridPawnID id )
{
	auto it = std::find_if( m_pawns.begin(), m_pawns.end(), [ id ]( const GridPawn& pawn ) { return pawn.m_id == id; } );
	if ( it != m_pawns.end() )
	{
		return &*it;
	}

	return nullptr;
}

const pacsnake::GridPawn* pacsnake::Grid::GetPawnAtPos( const Vector2& pos ) const
{
	for ( const auto& pawn : m_pawns )
	{
		if ( pawn.m_pos == pos )
		{
			return &pawn;
		}
	}

	return nullptr;
}

forge::co::Generator< Vector2 > pacsnake::Grid::FindUnocuppiedCells()
{
	for ( Int32 y = -static_cast< Int32 >( GetHeight() / 2 ); y <= static_cast< Int32 >( GetHeight() / 2 ); ++y )
	{
		for ( Int32 x = -static_cast< Int32 >( GetWidth() / 2 ); x <= static_cast< Int32 >( GetWidth() / 2 ); ++x )
		{
			Vector2 pos( static_cast< Float >( x ), static_cast< Float >( y ) );
			if ( GetPawnAtPos( pos ) == nullptr )
			{
				co_yield pos;
			}
		}
	}
}

const pacsnake::GridPawn* pacsnake::Grid::GetPawn( GridPawnID id ) const
{
	auto it = std::find_if( m_pawns.begin(), m_pawns.end(), [ id ]( const GridPawn& pawn ) { return pawn.m_id == id; } );
	if ( it != m_pawns.end() )
	{
		return &*it;
	}

	return nullptr;
}

void pacsnake::Grid::UpdatePawnTail( pacsnake::GridPawn& pawn )
{
	if ( auto* tail = GetPawn( pawn.m_nextTailID ) )
	{
		UpdatePawnTail( *tail );
		tail->m_pos = pawn.m_pos;
	}
}

void pacsnake::Grid::Update()
{
	for ( GridPawn& pawn : m_pawns )
	{
		pawn.m_prevPos = pawn.m_pos;
	}

	for ( GridPawn& pawn : m_pawns )
	{
		if ( pawn.m_growsTail )
		{
			UpdatePawnTail( pawn );
		}
		pawn.m_pos = pawn.m_pos + pawn.m_dir;
	}
}

std::vector< pacsnake::Grid::Collision > pacsnake::Grid::FindCollisions()
{
	std::vector< pacsnake::Grid::Collision > collisions;

	for ( Uint32 i = 0u; i < m_pawns.size(); ++i )
	{
		for ( Uint32 j = i + 1; j < m_pawns.size(); ++j )
		{
			if ( m_pawns[ i ].m_pos == m_pawns[ j ].m_pos )
			{
				collisions.push_back( { m_pawns[ i ].m_id, m_pawns[ j ].m_id } );
			}
		}
	}

	return collisions;
}
