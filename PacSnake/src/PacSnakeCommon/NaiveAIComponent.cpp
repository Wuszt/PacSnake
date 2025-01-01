#include "Fpch.h"
#include "NaiveAIComponent.h"
#include "GridSystem.h"
#include "AI/Graph.h"
#include "AI/PathFinding.h"
#include "Character.h"
#include "Systems/DebugSystem.h"

RTTI_IMPLEMENT_TYPE( pacsnake::NaiveAIComponent );

static void TryToAddConnection( AI::NavigationGraph< pacsnake::PathNode >& graph, const pacsnake::PathNode& node, const Vector2& offset, Float cost = 1.0f )
{
	pacsnake::PathNode neighbour{ node.m_pos + offset, offset, node.m_time + 1u };
	if ( graph.HasNodeForLocation( neighbour ) )
	{
		graph.AddConnection( node, neighbour, cost );
	}
}

void pacsnake::NaiveAIComponent::OnAttached( forge::EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue, forge::ObjectInitData* initData )
{
	Super::OnAttached( engineInstance, commandsQueue, initData );

	auto& gridSystem = GetOwner().GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
	auto& pickup = gridSystem.GetGameState().GetPickupPawn();

	const Uint32 tailLength = gridSystem.GetGameState().CalcTailLength( GetPawn().m_id );
	const Uint32 cellsAmount = gridSystem.GetGrid().GetWidth() * gridSystem.GetGrid().GetWidth();

	std::vector< AI::NodeID > nodes;
	nodes.reserve( cellsAmount * cellsAmount );

	for ( Uint32 t = 0u; t < cellsAmount; ++t )
	{
		for ( Int32 y = -static_cast< Int32 >( gridSystem.GetGrid().GetHeight() / 2 ); y <= static_cast< Int32 >( gridSystem.GetGrid().GetHeight() / 2 ); ++y )
		{
			for ( Int32 x = -static_cast< Int32 >( gridSystem.GetGrid().GetWidth() / 2 ); x <= static_cast< Int32 >( gridSystem.GetGrid().GetWidth() / 2 ); ++x )
			{
				const Vector2 pos( static_cast< Float >( x ), static_cast< Float >( y ) );
				nodes.push_back( m_graph.AddNode( { pos, { 1.0f, 0.0f }, t } ) );
				nodes.push_back( m_graph.AddNode( { pos, { -1.0f, 0.0f }, t } ) );
				nodes.push_back( m_graph.AddNode( { pos, { 0.0f, 1.0f }, t } ) );
				nodes.push_back( m_graph.AddNode( { pos, { 0.0f, -1.0f }, t } ) );
			}
		}
	}

	for ( AI::NodeID id : nodes )
	{
		const PathNode node = m_graph.GetLocationFromID( id );
		TryToAddConnection( m_graph, node, node.m_dir );
		TryToAddConnection( m_graph, node, Character::GetDirRotatedRight( node.m_dir ) );
		TryToAddConnection( m_graph, node, Character::GetDirRotatedLeft( node.m_dir ) );
	}
}

void pacsnake::NaiveAIComponent::OnBeforeActionTaken()
{
	Super::OnBeforeActionTaken();
	//ScheduleNextAction_Simple();
	ScheduleNextAction_Prediction();
}

void pacsnake::NaiveAIComponent::ScheduleNextAction_Simple()
{
	auto& gridSystem = GetOwner().GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
	auto& pickup = gridSystem.GetGameState().GetPickupPawn();

	AI::NavigationGraph< Vector2 > graph;
	std::vector< AI::NodeID > nodes;
	nodes.reserve( gridSystem.GetGrid().GetHeight() * gridSystem.GetGrid().GetWidth() );

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

Float PathNodeManhattanHeuristicFormula( const pacsnake::PathNode& from, const pacsnake::PathNode& to )
{
	return abs( from.m_pos.X - to.m_pos.X ) + abs( from.m_pos.Y - to.m_pos.Y );
}

void pacsnake::NaiveAIComponent::ScheduleNextAction_Prediction()
{
	auto& gridSystem = GetOwner().GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
	auto& pickup = gridSystem.GetGameState().GetPickupPawn();
	const Uint32 cellsAmount = gridSystem.GetGrid().GetWidth() * gridSystem.GetGrid().GetWidth();

	std::vector< AI::NodeID > disabledNodes;
	disabledNodes.reserve( cellsAmount );

	for ( const auto& pawn : gridSystem.GetGrid().GetPawns() )
	{
		if ( pickup.m_id == pawn.m_id )
		{
			continue;
		}

		const Uint32 tailLength = gridSystem.GetGameState().CalcTailLength( pawn.m_id );
		for ( Uint32 t = 0u; t <= tailLength; ++t )
		{
			auto DisableNode = [ & ]( const Vector2& offset )
				{
					AI::NodeID id = m_graph.GetIDFromLocation( { pawn.m_pos, offset, t } );
					m_graph.EnableNode( id, false );
					disabledNodes.push_back( id );
				};

			DisableNode( { 1.0f, 0.0f } );
			DisableNode( { -1.0f, 0.0f } );
			DisableNode( { 0.0f, 1.0f } );
			DisableNode( { 0.0f, -1.0f } );
		}
	}

	m_graph.EnableNode( m_graph.GetIDFromLocation( { GetPawn().m_pos, GetPawn().m_dir, 0u } ), true );
	m_graph.EnableNode( m_graph.GetIDFromLocation( { GetPawn().m_pos, Character::GetDirRotatedRight( GetPawn().m_dir ), 0u } ), true );
	m_graph.EnableNode( m_graph.GetIDFromLocation( { GetPawn().m_pos, Character::GetDirRotatedLeft( GetPawn().m_dir ), 0u } ), true );

	auto endComparator = [ & ]( AI::NodeID node, AI::NodeID end )
		{
			return m_graph.GetLocationFromID( node ).m_pos == m_graph.GetLocationFromID( end ).m_pos;
		};

	AI::PathAsNodes result;
	AI::PerformAStar( m_graph.GetIDFromLocation( { GetPawn().m_pos, GetPawn().m_dir, 0u } ), m_graph.GetIDFromLocation( { pickup.m_pos, {1.0f, 0.0f}, 0}), m_graph, result, &PathNodeManhattanHeuristicFormula, endComparator);
	if ( !result.empty() )
	{
		std::vector< PathNode > nodes = m_graph.TranslatePath( result );

		auto& debugSystem = GetOwner().GetEngineInstance().GetSystemsManager().GetSystem< systems::DebugSystem >();
		for ( const auto& node : nodes )
		{
			debugSystem.DrawSphere( Vector3( node.m_pos.X, node.m_pos.Y, 0.0f ), 1.0f, LinearColor::Red, true, false, gridSystem.GetPeriod() );
		}

		const Vector2 newDir = m_graph.GetLocationFromID( result[ 1 ] ).m_pos - m_graph.GetLocationFromID( result[ 0 ] ).m_pos;
		GetCharacter().ScheduleAction( Character::TranslateDirectionToAction( GetPawn().m_dir, newDir ) );
	}

	for ( AI::NodeID id : disabledNodes )
	{
		m_graph.EnableNode( id, true );
	}
}