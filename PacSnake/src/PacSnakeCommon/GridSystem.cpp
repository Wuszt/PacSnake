#include "Fpch.h"
#include "GridSystem.h"
#include "GameEngine/UpdateManager.h"
#include "Pickup.h"
#include "Tail.h"

#ifdef FORGE_IMGUI_ENABLED
#include "External/imgui/imgui.h"
#endif

RTTI_IMPLEMENT_TYPE( pacsnake::GridSystem );

void pacsnake::GridSystem::OnInitialize()
{
#ifdef FORGE_IMGUI_ENABLED
	InitializeDebuggable< pacsnake::GridSystem >( GetEngineInstance() );
#endif
	m_lastSimUpdateTime = forge::Time::GetTime();
	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Update,
		[ this ]()
		{
			const Float currTime = forge::Time::GetTime();
			if ( currTime > m_lastSimUpdateTime + m_period )
			{
				if ( !m_gameState.IsFinished() )
				{
					for ( Uint32 UpdateIteration = 0u; UpdateIteration < m_simUpdatesAmountPerTick; ++UpdateIteration )
					{
						m_onBeforeSimUpdate.Invoke();
						m_gameState.Update();
						m_onSimUpdated.Invoke();

						if ( m_gameState.IsFinished() )
						{
							break;
						}
					}

					m_lastSimUpdateTime = currTime;
					m_prevPeriod = m_period;
				}
			}
		} );

	GetEngineInstance().GetObjectsManager().RequestCreatingObject< pacsnake::Pickup >( { .m_preInitFunc = [ & ]( forge::Object& pickup, forge::ObjectInitData& )
		{
			rtti::CastChecked< pacsnake::Pickup >( pickup ).SetPawn( m_gameState.GetPickupID() );
		} } );

	m_onNewTailToken = m_gameState.RegisterOnNewTail( [ this ]( pacsnake::GridPawnID tailID )
		{
			GetEngineInstance().GetObjectsManager().RequestCreatingObject< pacsnake::Tail >( { .m_preInitFunc = [ this, tailID ]( forge::Object& tail, forge::ObjectInitData& )
			{
				rtti::CastChecked< pacsnake::Tail >( tail ).SetPawn( tailID );
			} } );
		} );
}

#ifdef FORGE_IMGUI_ENABLED
void pacsnake::GridSystem::OnRenderDebug()
{
	if ( ImGui::Begin( "GridSystem" ) )
	{
		ImGui::SliderFloat( "Period", &m_period, 0.025f, 1.0f );
	}

	ImGui::End();
}
#endif

forge::CallbackToken pacsnake::GridSystem::RegisterOnSimUpdate( std::function< void() > func )
{
	return m_onSimUpdated.AddListener( std::move( func) );
}

forge::CallbackToken pacsnake::GridSystem::RegisterOnBeforeSimUpdate( std::function< void() > func )
{
	return m_onBeforeSimUpdate.AddListener( std::move( func ) );
}