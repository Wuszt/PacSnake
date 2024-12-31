#pragma once
#include "Grid.h"
#include "Actor.h"

namespace forge
{
	class ObjectInitData;
}

namespace pacsnake
{
	class Tail : public pacsnake::Actor
	{
		RTTI_DECLARE_CLASS( Tail, pacsnake::Actor );

	public:
		virtual void OnInit( forge::ObjectInitData& initData ) override;
		
	private:
		pacsnake::Tail* m_nextTail = nullptr;
	};
}