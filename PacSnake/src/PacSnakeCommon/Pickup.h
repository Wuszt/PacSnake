#pragma once

#include "Grid.h"
#include "Actor.h"

namespace forge
{
	class ObjectInitData;
}

namespace pacsnake
{
	class Pickup : public pacsnake::Actor
	{
		RTTI_DECLARE_CLASS( Pickup, pacsnake::Actor );

	public:
	protected:
		virtual void OnInit( forge::ObjectInitData& initData ) override;
		virtual void Update() override;

	private:
		forge::CallbackToken m_updateToken;
	};
}