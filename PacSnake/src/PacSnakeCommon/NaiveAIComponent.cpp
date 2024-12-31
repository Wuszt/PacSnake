#include "Fpch.h"
#include "NaiveAIComponent.h"
#include "GridSystem.h"
#include "AI/Graph.h"
#include "AI/PathFinding.h"
#include "Character.h"

RTTI_IMPLEMENT_TYPE( pacsnake::NaiveAIComponent );

void pacsnake::NaiveAIComponent::OnBeforeActionTaken()
{
	Super::OnBeforeActionTaken();
	auto& gridSystem = GetOwner().GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
	auto& pickup = gridSystem.GetGameState().GetPickupPawn();

	AI::NavigationGraph< Vector2 > graph;
	std::vector< AI::NodeID > nodes;
	for ( Int32 y = -static_cast< Int32 >( gridSystem.GetGrid().GetHeight() / 2 ); y <= static_cast< Int32 >( gridSystem.GetGrid().GetHeight() / 2 ); ++y )
	{
		for ( Int32 x = -static_cast< Int32 >( gridSystem.GetGrid().GetWidth() / 2 ); x <= static_cast< Int32 >( gridSystem.GetGrid().GetWidth() / 2 ); ++x )
		{
			const Vector2 pos( static_cast< Float >( x ), static_cast< Float >( y ) );
			if ( gridSystem.GetGrid().GetPawnAtPos( pos ) == nullptr )
			{
				nodes.push_back( graph.AddNode( pos ) );
			}
		}
	}

	nodes.push_back( graph.AddNode( pickup.m_pos ) );

	graph.AddNode( GetPawn().m_pos );

	auto TryToAddConnection = [ & ]( const Vector2& nodePos, const Vector2& offset )
		{
			const Vector2 neighbourPos = nodePos + offset;
			if ( graph.HasNodeForLocation( neighbourPos ) )
			{
				graph.AddConnection( nodePos, neighbourPos );
			}
		};

	for ( AI::NodeID id : nodes )
	{
		const Vector2 nodePos = graph.GetLocationFromID( id );

		TryToAddConnection( nodePos, Vector2( 1.0f, 0.0f ) );
		TryToAddConnection( nodePos, Vector2( -1.0f, 0.0f ) );
		TryToAddConnection( nodePos, Vector2( 0.0f, 1.0f ) );
		TryToAddConnection( nodePos, Vector2( 0.0f, -1.0f ) );
	}

	TryToAddConnection( GetPawn().m_pos, GetPawn().m_dir );
	TryToAddConnection( GetPawn().m_pos, Character::GetDirRotatedRight( GetPawn().m_dir ) );
	TryToAddConnection( GetPawn().m_pos, Character::GetDirRotatedLeft( GetPawn().m_dir ) );

	AI::PathAsNodes result;
	AI::PerformAStar( graph.GetIDFromLocation( GetPawn().m_pos ), graph.GetIDFromLocation( pickup.m_pos ), graph, result, &AI::ManhattanHeuristicFormula );
	if ( !result.empty() )
	{
		const Vector2 newDir = graph.GetLocationFromID( result[ 1 ] ) - graph.GetLocationFromID( result[ 0 ] );
		GetCharacter().ScheduleAction( Character::TranslateDirectionToAction( GetPawn().m_dir, newDir ) );
	}
}