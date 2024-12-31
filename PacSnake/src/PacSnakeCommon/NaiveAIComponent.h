#pragma once
#include "AIComponent.h"

namespace pacsnake
{
	class NaiveAIComponent : public pacsnake::AIComponent
	{
		RTTI_DECLARE_CLASS( NaiveAIComponent, pacsnake::AIComponent );

	protected:
		virtual void OnBeforeActionTaken() override;
	};
}