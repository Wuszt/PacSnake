#pragma once
#include "GameEngine/SceneObject.h"
#include "Grid.h"
#include "Core/Callback.h"

namespace pacsnake
{
	struct GridPawn;

	class Actor : public forge::SceneObject
	{
		RTTI_DECLARE_CLASS( Actor, forge::SceneObject );

	public:
		void SetPawn( pacsnake::GridPawnID pawnID )
		{
			m_pawnID = pawnID;
		}

		void EnableSmoothMovement( Bool enable )
		{
			m_enabledSmoothMovement = enable;
		}

		GridPawn& GetPawn() const;

	protected:
		virtual void OnInit( forge::ObjectInitData& initData ) override;
		virtual void OnBeforeSimUpdated() {}
		virtual void OnSimUpdated();
		virtual void OnNewPickup() {}
		virtual void Update();

	private:
		forge::CallbackToken m_gridUpdateToken;
		forge::CallbackToken m_beforeGridUpdateToken;
		forge::CallbackToken m_onNewPickupToken;
		forge::CallbackToken m_updateToken;
		pacsnake::GridPawnID m_pawnID;

		Bool m_enabledSmoothMovement = false;
	};
}
