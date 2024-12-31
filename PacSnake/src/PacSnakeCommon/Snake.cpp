#include "Fpch.h"
#include "Snake.h"
#include "Systems/RenderingComponent.h"
#include "PlayerCharacterComponent.h"
#include "Systems/TransformComponent.h"
#include "GridSystem.h"
#include "Tail.h"
#include "NaiveAIComponent.h"

RTTI_IMPLEMENT_TYPE( pacsnake::Snake );

void pacsnake::Snake::OnInit( forge::ObjectInitData& initData )
{
	Super::OnInit( initData );
	FORGE_ASSURE( AddComponent< forge::RenderingComponent >() );
	GetComponent< forge::RenderingComponent >()->LoadMeshAndMaterial( forge::Path( "PacSnake\\Snake\\Snake.fbx" ) );

	//AddComponent< pacsnake::PlayerCharacterComponent >();
	AddComponent< pacsnake::NaiveAIComponent >();
	GetComponent< forge::TransformComponent >()->SetWorldScale( Vector3( 1.0f, 1.0f, 1.0f ) );

	GetPawn().m_dir = Vector2( 1.0f, 0.0f );
	GetPawn().m_growsTail = true;

	auto& gridSystem = GetEngineInstance().GetSystemsManager().GetSystem< pacsnake::GridSystem >();
	m_gridUpdateToken = gridSystem.RegisterOnSimUpdate( [ this ]()
		{
			GridPawn& pawn = GetPawn();
			if ( pawn.m_nextTailID.IsValid() && !m_spawnedTail )
			{
				m_spawnedTail = true;
				GetEngineInstance().GetObjectsManager().RequestCreatingObject< pacsnake::Tail >( { .m_preInitFunc = [ this, tailID = pawn.m_nextTailID ]( forge::Object& tail, forge::ObjectInitData& )
					{
						m_tailStart = rtti::Cast< pacsnake::Tail >( tail );
						m_tailStart->SetPawn( tailID );
					} } );
			}
		} );

	EnableSmoothMovement( true );
}