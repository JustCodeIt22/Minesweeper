#include "iostream"
#include "SFML/Graphics.hpp"

#include "Game.hpp"

using namespace std;

int main(int argc, char **argv)
{
    Game game(576, 676, "Minesweeper"); // width = 64 * 12 = 576
    game.run();

    return 0;
}