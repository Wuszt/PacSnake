#pragma once
#include "Grid.h"

namespace pacsnake
{
	class GameState
	{
	public:
		GameState( Uint32 gridHeight, Uint32 gridWidth );

		void Update();

		Grid& GetGrid()
		{
			return m_grid;
		}

		[[nodiscard]] forge::CallbackToken RegisterOnNewTail( std::function< void( pacsnake::GridPawnID ) > func );

		GridPawnID GetPickupID() const
		{
			return m_pickupID;
		}

		Bool IsFinished() const
		{
			return m_isFinished;
		}

		struct Score
		{
			pacsnake::GridPawnID m_id;
			Uint32 m_score = 0u;
		};

		std::vector< Score > CalculateScores() const;

	private:
		void OnPickupGrabbed( pacsnake::GridPawnID grabber );

		forge::Callback< pacsnake::GridPawnID > m_onNewTail;
		Grid m_grid;
		GridPawnID m_pickupID;
		Bool m_isFinished = false;
	};
}