#pragma once
#include "GameEngine/ISystem.h"
#include "../PacSnakeCommon/Grid.h"
#include "GameState.h"

namespace pacsnake
{
	class GridSystem : public systems::ISystem
	{
		RTTI_DECLARE_CLASS( GridSystem, systems::ISystem );

	public:
		GridSystem()
			: m_gameState( GameState( 21u, 21u ) )
		{}

		forge::CallbackToken RegisterOnSimUpdate( std::function< void() > func );
		forge::CallbackToken RegisterOnBeforeSimUpdate( std::function< void() > func );

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

		Float m_lastSimUpdateTime = 0.0f;
		Float m_period = 0.5f;
		Float m_prevPeriod = m_period;
	};
}