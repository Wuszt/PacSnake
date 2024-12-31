#include "Fpch.h"
#include "GridSystem.h"
#include "GameEngine/UpdateManager.h"
#include "Pickup.h"
#include "Tail.h"

RTTI_IMPLEMENT_TYPE( pacsnake::GridSystem );

void pacsnake::GridSystem::OnInitialize()
{
	m_lastSimUpdateTime = forge::Time::GetTime();
	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Update,
		[ this ]()
		{
			const Float currTime = forge::Time::GetTime();
			if ( currTime > m_lastSimUpdateTime + m_period )
			{
				m_onBeforeSimUpdate.Invoke();
				m_gameState.Update();
				m_onSimUpdated.Invoke();

				m_lastSimUpdateTime = currTime;
				m_prevPeriod = m_period;
				m_period -= 0.001f;
				m_period = Math::Max( 0.1f, m_period );
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

forge::CallbackToken pacsnake::GridSystem::RegisterOnSimUpdate( std::function< void() > func )
{
	return m_onSimUpdated.AddListener( std::move( func) );
}

forge::CallbackToken pacsnake::GridSystem::RegisterOnBeforeSimUpdate( std::function< void() > func )
{
	return m_onBeforeSimUpdate.AddListener( std::move( func ) );
}