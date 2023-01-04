#include "Game.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

	Game::GetInstance()->Run();

	return 0;
}