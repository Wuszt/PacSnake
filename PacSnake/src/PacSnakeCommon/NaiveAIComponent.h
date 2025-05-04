#pragma once
#include "AIComponent.h"
#include "AI/Graph.h"

namespace forge
{
	class EngineInstance;
	class ObjectInitData;
}

namespace ecs
{
	class CommandsQueue;
}

namespace pacsnake
{
	struct PathNode
	{
		Vector2 m_pos;
		Uint32 m_time = 0u;

		Bool operator==( const PathNode& node ) const = default;

		Float DistTo( const PathNode& node ) const
		{
			return m_pos.DistTo( node.m_pos );
		}
	};
}

template<>
struct std::hash< pacsnake::PathNode >
{
	std::size_t operator()( const pacsnake::PathNode& node ) const noexcept
	{
		return Math::CombineHashes( Math::CalculateHash( node.m_pos ), Math::CalculateHash( node.m_time ) );
	}
};

namespace pacsnake
{
	class NaiveAIComponent : public pacsnake::AIComponent
	{
		RTTI_DECLARE_CLASS( NaiveAIComponent, pacsnake::AIComponent );

	public:
		void SetTimeBudget( Float budget )
		{
			FORGE_ASSERT( budget > 0.0f );
			m_timeBudget = budget;
		}

		void EnableDebugs( Bool enabled )
		{
			m_debugEnabled = enabled;
		}

	protected:
		virtual void OnAttached( forge::EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue, forge::ObjectInitData* initData ) override;
		virtual void OnBeforeActionTaken() override;
		virtual void OnSimUpdated() override;
		virtual void OnNewPickup() override;
		
	private:
		void CalculatePath();
		Bool CheckPathToTail( const forge::ai::PathAsNodes& executedPath, Bool withPickup, forge::ai::PathAsNodes* outPath = nullptr );

		void DrawNodes( forge::ArraySpan< const forge::ai::NodeID > nodes, LinearColor color, Float zOffset = 1.0f );

		forge::ai::NavigationGraph< Vector2 > m_graph;
		std::vector< Vector2 > m_currentPath;
		Uint32 m_indexOnPath = 0u;

		Float m_timeBudget = 1.0f / 60.0f;

		Bool m_debugEnabled = false;
	};
}