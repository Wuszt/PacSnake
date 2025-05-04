#include "Fpch.h"
#include "NaiveAIComponent.h"
#include "GridSystem.h"
#include "AI/Graph.h"
#include "AI/PathFinding.h"
#include "Character.h"
#include "Systems/DebugSystem.h"

RTTI_IMPLEMENT_TYPE( pacsnake::NaiveAIComponent );

template< class TNodeAccessibilityFunc >
using AStarConfig = forge::ai::AStarConfig< Vector2
	, false
	, forge::Functor< Float, const Vector2&, const Vector2& >::Static< forge::ai::ManhattanHeuristicFormula >
	, forge::Functor< Bool, forge::ai::NodeID, Float >::Dynamic< TNodeAccessibilityFunc >
	, true >;

static void TryToAddConnection( forge::ai::NavigationGraph< Vector2 >& graph, const Vector2& nodePos, const Vector2& offset, Float cost = 1.0f )
{
	Vector2 neighbourPos{ nodePos + offset };
	if ( graph.HasNodeForLocation( neighbourPos ) )
	{
		graph.AddConnection( nodePos, neighbourPos, cost );
	}
}

void pacsnake::NaiveAIComponent::OnAttached( forge::EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue, forge::ObjectInitData* initData )
{
	Super::OnAttached( engineInstance, commandsQueue, initData );

	auto& gridSystem = GetOwner().GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
	auto& pickup = gridSystem.GetGameState().GetPickupPawn();

	const Uint32 cellsAmount = gridSystem.GetGrid().GetWidth() * gridSystem.GetGrid().GetHeight();

	std::vector< forge::ai::NodeID > nodes;
	nodes.reserve( cellsAmount * cellsAmount );

	for ( Int32 y = -static_cast< Int32 >( gridSystem.GetGrid().GetHeight() / 2 ); y <= static_cast< Int32 >( gridSystem.GetGrid().GetHeight() / 2 ); ++y )
	{
		for ( Int32 x = -static_cast< Int32 >( gridSystem.GetGrid().GetWidth() / 2 ); x <= static_cast< Int32 >( gridSystem.GetGrid().GetWidth() / 2 ); ++x )
		{
			const Vector2 pos( static_cast< Float >( x ), static_cast< Float >( y ) );
			nodes.push_back( m_graph.AddNode( pos ) );
		}
	}

	for ( forge::ai::NodeID id : nodes )
	{
		const Vector2 nodePos = m_graph.GetLocationFromID( id );
		TryToAddConnection( m_graph, nodePos, { 1.0f, 0.0f } );
		TryToAddConnection( m_graph, nodePos, { -1.0f, 0.0f } );
		TryToAddConnection( m_graph, nodePos, { 0.0f, 1.0f } );
		TryToAddConnection( m_graph, nodePos, { 0.0f, -1.0f } );
	}

	CalculatePath();
}

void pacsnake::NaiveAIComponent::OnBeforeActionTaken()
{
	Super::OnBeforeActionTaken();

	if ( !m_currentPath.empty() )
	{
		const Vector2 newDir = m_currentPath[ m_indexOnPath + 1 ] - m_currentPath[ m_indexOnPath ];
		GetCharacter().ScheduleAction( Character::TranslateDirectionToAction( GetPawn().m_dir, newDir ) );
		++m_indexOnPath;
	}
}

void pacsnake::NaiveAIComponent::OnSimUpdated()
{
	Super::OnSimUpdated();

	auto& gridSystem = GetOwner().GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();

	if ( gridSystem.GetGameState().IsFinished() )
	{
		return;
	}

	if ( m_currentPath.size() - 1u == m_indexOnPath )
	{
		CalculatePath();
	}
}
static Uint32 fails = 0u;
void pacsnake::NaiveAIComponent::OnNewPickup()
{
	Super::OnNewPickup();

	auto& gridSystem = GetOwner().GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
	if ( !gridSystem.GetGameState().IsFinished() )
	{
		CalculatePath();
	}

	FORGE_LOG( "Fails: %d", fails );
}

void pacsnake::NaiveAIComponent::CalculatePath()
{
	auto& gridSystem = GetOwner().GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
	auto& pickup = gridSystem.GetGameState().GetPickupPawn();
	const Uint32 cellsAmount = gridSystem.GetGrid().GetWidth() * gridSystem.GetGrid().GetHeight();

	std::vector< Uint32 > cellAccesibilities( cellsAmount, 0u );

	auto disableNodeUntil = [ this, &cellAccesibilities ]( const Vector2& pos, Uint32 time )
		{
			forge::ai::NodeID id = m_graph.GetIDFromLocation( pos );
			cellAccesibilities[ id.Get() ] = Math::Max( time, cellAccesibilities[ id.Get() ] );
		};

	auto getTimeLimitInterruptor = []( Float timeLimit )
		{
			return [ sw = forge::StopWatch(), timeLimit ]()
				{
					return sw.GetDuration() > timeLimit;
				};
		};

	for ( const auto& pawn : gridSystem.GetGrid().GetPawns() )
	{
		if ( pickup.m_id == pawn.m_id )
		{
			continue;
		}

		disableNodeUntil( pawn.m_pos, pawn.m_tailLength );
	}

	disableNodeUntil( GetPawn().m_pos - GetPawn().m_dir, 1u );

	forge::ai::PathAsNodes result;
	auto nodeAccesibilityFunc = [ & ]( forge::ai::NodeID node, Float currentCost )
		{
			return cellAccesibilities[ node.Get() ] < currentCost;
		};

	if ( GetPawn().m_tailLength == cellsAmount - 2 )
	{
		forge::ai::FindPath_AStar( { m_graph.GetIDFromLocation( GetPawn().m_pos ) }, m_graph.GetIDFromLocation( pickup.m_pos ), AStarConfig< decltype( nodeAccesibilityFunc ) >( m_graph, {}, nodeAccesibilityFunc ), result);
	}

	if ( result.empty() )
	{
		Uint32 attemptsCounter = 0u;
		for ( auto& path : forge::ai::FindPaths_AStar( { m_graph.GetIDFromLocation( GetPawn().m_pos ) }, m_graph.GetIDFromLocation( pickup.m_pos ), AStarConfig< decltype( nodeAccesibilityFunc ) >( m_graph, {}, nodeAccesibilityFunc, GetPawn().m_tailLength + 1u ), getTimeLimitInterruptor( m_timeBudget * 0.75f ) ) )
		{
			++attemptsCounter;
			if ( CheckPathToTail( path, true ) )
			{
				FORGE_LOG( "Found path to pickup and then to tail!" );
				result = std::move( path );
				break;
			}
			else
			{
				FORGE_LOG( "Found path to pickup, but failed to find path to tail [%d]", attemptsCounter );
			}
		}

		if ( result.empty() )
		{
			++fails;
			const Vector2 tailPos = gridSystem.GetGrid().GetPawn( gridSystem.GetGameState().GetLastTail( GetPawn().m_id ) )->m_pos;

			cellAccesibilities[ m_graph.GetIDFromLocation( pickup.m_pos ).Get() ] = std::numeric_limits< Uint32 >::max();
			for ( auto& path : forge::ai::FindPaths_AStar( { m_graph.GetIDFromLocation( GetPawn().m_pos ) }, m_graph.GetIDFromLocation( tailPos ), AStarConfig< decltype( nodeAccesibilityFunc ) >( m_graph, {}, nodeAccesibilityFunc ), getTimeLimitInterruptor( m_timeBudget * 0.25f ) ) )
			{
				result = { path[ 0 ], path[ 1 ] };
			}

			if ( result.empty() )
			{
				forge::ai::PathAsNodes path;
				forge::ai::FindPath_AStar( { m_graph.GetIDFromLocation( GetPawn().m_pos ) }, m_graph.GetIDFromLocation( tailPos ), AStarConfig< decltype( nodeAccesibilityFunc ) >( m_graph, {}, nodeAccesibilityFunc ), path );
				if ( !path.empty() )
				{
					result = { path[ 0 ], path[ 1 ] };
				}
			}

			cellAccesibilities[ m_graph.GetIDFromLocation( pickup.m_pos ).Get() ] = 0u;

			if ( !result.empty() )
			{
				FORGE_LOG( "Following tail instead..." );
			}
		}
	}

	m_currentPath.clear();
	m_indexOnPath = 0u;

	if ( !result.empty() )
	{
		DrawNodes( result, LinearColor::Red );

		m_currentPath = m_graph.TranslatePath( result );
	}
	else
	{
		FORGE_LOG( "Failed to find a path" );
	}
}

Bool pacsnake::NaiveAIComponent::CheckPathToTail( const forge::ai::PathAsNodes& executedPath, Bool withPickup, forge::ai::PathAsNodes* outPath )
{
	PC_SCOPE_FUNC();

	auto& gridSystem = GetOwner().GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
	auto& pickup = gridSystem.GetGameState().GetPickupPawn();
	const Uint32 cellsAmount = gridSystem.GetGrid().GetWidth() * gridSystem.GetGrid().GetHeight();

	std::vector< Uint32 > cellAccesibilities( cellsAmount, 0u );

	auto disableNodeUntil = [ this, &cellAccesibilities ]( const Vector2& pos, Uint32 time )
		{
			forge::ai::NodeID id = m_graph.GetIDFromLocation( pos );
			cellAccesibilities[ id.Get() ] = Math::Max( time, cellAccesibilities[ id.Get() ] );
		};

	const Uint32 bodyLengthOffset = withPickup ? 1u : 0u;

	// Disable nodes that will be still disabled after the movement
	{
		for ( const auto& pawn : gridSystem.GetGrid().GetPawns() )
		{
			if ( pickup.m_id == pawn.m_id )
			{
				continue;
			}

			const Uint32 bodyLength = pawn.m_tailLength + 1u + bodyLengthOffset;
			GridPawnID tailID = pawn.m_id;
			disableNodeUntil( pawn.m_pos, static_cast< Uint32 >( bodyLength - executedPath.size() ) );
		}
	}

	const Uint32 bodyLength = GetPawn().m_tailLength + 1u + bodyLengthOffset;
	for ( Uint32 p = 0u; p < Math::Min( bodyLength, static_cast< Uint32 >( executedPath.size() ) ); ++p )
	{
		const forge::ai::NodeID pathNodeID = executedPath[ executedPath.size() - p - 1 ];
		const auto& pos = m_graph.GetLocationFromID( pathNodeID );
		disableNodeUntil( pos, bodyLength - p );
	}

	Vector2 tailPos;
	{
		if ( bodyLength >= executedPath.size() )
		{
			pacsnake::GridPawnID tailID = GetPawn().m_id;
			for ( Uint32 i = 0u; i < bodyLength - executedPath.size(); ++i )
			{
				tailID = gridSystem.GetGrid().GetPawn( tailID )->m_nextTailID;
			}
			tailPos = gridSystem.GetGrid().GetPawn( tailID )->m_pos;
		}
		else
		{
			tailPos = m_graph.GetLocationFromID( executedPath[ executedPath.size() - bodyLength ] );
		}
	}

	auto nodeAccesibilityFunc = [ & ]( forge::ai::NodeID node, Float currentCost )
		{
			return cellAccesibilities[ node.Get() ] < currentCost;
		};

	forge::ai::PathAsNodes path;

	if ( outPath == nullptr )
	{
		outPath = &path;
	}

	forge::ai::FindPath_AStar( { executedPath.back() }, m_graph.GetIDFromLocation( { tailPos } ), AStarConfig< decltype( nodeAccesibilityFunc ) >( m_graph, {}, nodeAccesibilityFunc ), *outPath );
	if ( outPath->empty() )
	{
		return false;
	}

	DrawNodes( path, LinearColor::Blue );

	return true;
}

void pacsnake::NaiveAIComponent::DrawNodes( forge::ArraySpan< const forge::ai::NodeID > nodes, LinearColor color, Float zOffset )
{
#ifdef FORGE_DEBUGGING
	if ( !m_debugEnabled )
	{
		return;
	}

	auto& gridSystem = GetOwner().GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
	auto& debugSystem = GetOwner().GetEngineInstance().GetSystemsManager().GetSystem< systems::DebugSystem >();
	const auto poses = m_graph.TranslatePath( nodes );
	for ( const auto& nodePos : poses )
	{
		debugSystem.DrawSphere( Vector3( nodePos.X, nodePos.Y, zOffset ), 0.9f, color, true, false, 0.1f );
	}
#endif
}