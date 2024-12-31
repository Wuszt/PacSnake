#include "Fpch.h"
#include "AIComponent.h"
#include "Character.h"

RTTI_IMPLEMENT_TYPE( pacsnake::AIComponent );

void pacsnake::AIComponent::OnAttached( forge::EngineInstance& engineInstance, ecs::CommandsQueue& commandsQueue, forge::ObjectInitData* initData )
{
	Super::OnAttached( engineInstance, commandsQueue, initData );
	m_onBeforeActionTakenToken = GetCharacter().RegisterOnBeforeActionTaken( [ this ]() { OnBeforeActionTaken(); } );
}

pacsnake::Character& pacsnake::AIComponent::GetCharacter()
{
	return rtti::CastChecked< pacsnake::Character >( GetOwner() );
}

pacsnake::GridPawn& pacsnake::AIComponent::GetPawn()
{
	return GetCharacter().GetPawn();
}
