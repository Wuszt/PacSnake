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
		Vector2 m_dir;
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
		return Math::CombineHashes( Math::CalculateHash( node.m_pos ), Math::CalculateHash( node.m_dir ), Math::CalculateHash( node.m_time ) );
	}
};

namespace pacsnake
{
	class NaiveAIComponent : public pacsnake::AIComponent
	{
		RTTI_DECLARE_CLASS( NaiveAIComponent, pacsnake::AIComponent );

	protected:
		virtual void OnAttached( forge::EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue, forge::ObjectInitData* initData ) override;
		virtual void OnBeforeActionTaken() override;

	private:
		void ScheduleNextAction_Simple();
		void ScheduleNextAction_Prediction();

		AI::NavigationGraph< PathNode > m_graph;
	};
}