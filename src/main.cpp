#include "Game.h"

constexpr int kScreenWidth = 960;
constexpr int kScreenHeight = 540;

int main(int argc, char* argv[])
{
	Game game;
	if (game.Init(kScreenWidth, kScreenHeight, false, "test"))
		game.Run();

	return 0;
}