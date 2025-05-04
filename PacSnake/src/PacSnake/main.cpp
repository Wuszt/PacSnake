#include "Core/PublicDefaults.h"
#include "GameEngine/EngineInstance.h"
#include "../PacSnakeCommon/PublicDefaults.h"
#include "GameInstance.h"

Int32 main()
{
	pacsnake::GameInstance gameInstance( "PacSnake" );

	forge::EngineInstance engineInstance( gameInstance );
	engineInstance.Run();
}