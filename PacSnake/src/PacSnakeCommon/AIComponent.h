#pragma once
#include "GameEngine/IComponent.h"

namespace pacsnake
{
	class Character;
	struct GridPawn;

	class AIComponent : public forge::IComponent
	{
		RTTI_DECLARE_CLASS( AIComponent, forge::IComponent );

	protected:
		virtual void OnAttached( forge::EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue, forge::ObjectInitData* initData ) override;
		virtual void OnBeforeActionTaken() {}

		Character& GetCharacter();
		GridPawn& GetPawn();

	private:
		forge::CallbackToken m_onBeforeActionTakenToken;
	};
}