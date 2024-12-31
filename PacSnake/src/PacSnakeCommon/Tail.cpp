#include "Fpch.h"
#include "Tail.h"
#include "Systems/RenderingComponent.h"
#include "Systems/TransformComponent.h"
#include "Renderer/Material.h"
#include "GridSystem.h"

RTTI_IMPLEMENT_TYPE( pacsnake::Tail );

void pacsnake::Tail::OnInit( forge::ObjectInitData& initData )
{
	Super::OnInit( initData );
	FORGE_ASSURE( AddComponent< forge::RenderingComponent >() );
	GetComponent< forge::RenderingComponent >()->LoadMeshAndMaterial( forge::Path( "Engine\\Models\\sphere.obj" ) );
	auto& material = *GetComponent< forge::RenderingComponent >()->GetDirtyData()->m_renderable.GetMaterials()[ 0 ];
	material.GetConstantBuffer()->SetData( "diffuseColor", LinearColor( 0.38f, 0.86f, 0.4f ) );
	material.GetConstantBuffer()->UpdateBuffer();
	GetComponent< forge::TransformComponent >()->SetWorldScale( { 1.0f, 1.0f, 1.0f } );
	EnableSmoothMovement( true );
}
