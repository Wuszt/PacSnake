#pragma once
#include "GameEngine/ISystem.h"
#include "../PacSnakeCommon/Grid.h"
#include "GameState.h"
#include "Systems/IDebuggable.h"

namespace pacsnake
{
	class GridSystem : public systems::ISystem, public forge::IDebuggable
	{
		RTTI_DECLARE_CLASS( GridSystem, systems::ISystem );

	public:
		GridSystem()
			: m_gameState( GameState( 21u, 21u ) )
		{}

		forge::CallbackToken RegisterOnSimUpdate( std::function< void() > func );
		forge::CallbackToken RegisterOnBeforeSimUpdate( std::function< void() > func );

		void SetPeriod( float period )
		{
			FORGE_ASSERT( period >= 0.0f );
			m_period = period;
		}

		void SetSimUpdatesAmountPerTick( Uint32 amount )
		{
			m_simUpdatesAmountPerTick = amount;
		}

		Grid& GetGrid()
		{
			return m_gameState.GetGrid();
		}

		GameState& GetGameState()
		{
			return m_gameState;
		}

		Float GetPeriod() const
		{
			return m_period;
		}

		Float GetPrevPeriod() const
		{
			return m_prevPeriod;
		}

		Float GetLastSimUpdateTime() const
		{
			return m_lastSimUpdateTime;
		}

	private:
		virtual void OnInitialize() override;

		GameState m_gameState;
		forge::CallbackToken m_onNewTailToken;
		forge::CallbackToken m_updateToken;
		forge::Callback<> m_onSimUpdated;
		forge::Callback<> m_onBeforeSimUpdate;

		Uint32 m_simUpdatesAmountPerTick = 1u;

		Float m_lastSimUpdateTime = 0.0f;
		Float m_period = 0.1f;
		Float m_prevPeriod = m_period;

#ifdef FORGE_IMGUI_ENABLED
		void OnRenderDebug() override;
#endif

	};
}