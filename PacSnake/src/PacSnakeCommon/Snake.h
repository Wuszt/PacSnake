#pragma once
#include "Character.h"

namespace forge
{
	class ObjectInitData;
}

namespace pacsnake
{
	class Tail;

	class Snake : public pacsnake::Character
	{
		RTTI_DECLARE_CLASS( Snake, pacsnake::Character );

	public:
		virtual void OnInit( forge::ObjectInitData& initData ) override;

	private:
		pacsnake::Tail* m_tailStart = nullptr;
		forge::CallbackToken m_gridUpdateToken;
		Bool m_spawnedTail = false;
	};
}