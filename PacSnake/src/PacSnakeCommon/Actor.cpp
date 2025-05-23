#include "Fpch.h"
#include "Actor.h"
#include "GridSystem.h"
#include "Systems/TransformComponent.h"

RTTI_IMPLEMENT_TYPE( pacsnake::Actor );

void pacsnake::Actor::OnInit( forge::ObjectInitData& initData )
{
	Super::OnInit( initData );
	auto& gridSystem = GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();

	if ( !m_pawnID.IsValid() )
	{
		m_pawnID = gridSystem.GetGrid().AddPawn( Vector2( 0.0f, 0.0f ) );
	}

	m_beforeGridUpdateToken = gridSystem.RegisterOnBeforeSimUpdate( [ this ]()
		{
			OnBeforeSimUpdated();
		} );

	m_gridUpdateToken = gridSystem.RegisterOnSimUpdate( [ this ]()
		{
			OnSimUpdated();
		} );

	m_onNewPickupToken = gridSystem.GetGameState().RegisterOnNewTail( [ this ]( pacsnake::GridPawnID )
		{
			OnNewPickup();
		} );

	m_updateToken = GetEngineInstance().GetUpdateManager().RegisterUpdateFunction( forge::UpdateManager::BucketType::Update,
		[ this ]()
		{
			Update();
		} );
}

void pacsnake::Actor::OnSimUpdated()
{
	GridPawn& pawn = GetPawn();

	if ( pawn.m_dir.SquareMag() > 0.0f )
	{
		auto* transformComp = GetComponent< forge::TransformComponent >();
		transformComp->SetWorldOrientation( Quaternion::CreateFromDirection( Vector3( pawn.m_dir, 0.0f ) ) );
	}
}

void pacsnake::Actor::Update()
{
	auto& gridSystem = GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
	if ( gridSystem.GetGameState().IsFinished() )
	{
		return;
	}

	const GridPawn& pawn = GetPawn();

	if ( m_enabledSmoothMovement )
	{
		const Float currentTime = forge::Time::GetTime();
		const Float t = ( currentTime - gridSystem.GetLastSimUpdateTime() ) / gridSystem.GetPrevPeriod();

		GetComponent< forge::TransformComponent >()->SetWorldPosition( Vector3( Math::Lerp( pawn.m_prevPos, pawn.m_pos, t ), 0.5f ) );
	}
	else
	{
		GetComponent< forge::TransformComponent >()->SetWorldPosition( Vector3( pawn.m_pos, 0.5f ) );
	}
}

pacsnake::GridPawn& pacsnake::Actor::GetPawn() const
{
	auto& gridSystem = GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
	GridPawn* pawn = gridSystem.GetGrid().GetPawn( m_pawnID );
	FORGE_ASSERT( pawn );
	return *pawn;
}