#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <queue>
#include <utility>

// SFML Libraries
#include "SFML/Graphics.hpp"

#define SQ_SIZE 48
#define ROWS 12
#define COLS 12

class Game
{
public:
    Game(int width, int height, std::string title);
    ~Game();
    void run();

private:
    // Window var
    sf::RenderWindow mWindow;
    sf::Vector2i mouse_pos;

    sf::RectangleShape scoreBoardRect;

    // Game var
    int grid[ROWS][COLS];
    int sgrid[ROWS][COLS];
    int mine_cnt;

    // Textures and Sprites
    sf::Texture tile_tex;
    sf::Sprite tile_sprite;

    sf::Texture score_board_tex;
    sf::Sprite score_board_sprite;

    sf::Texture moods_tex;
    sf::Sprite moods_sprite;

    int clicked_row, clicked_col;

    // Fonts
    sf::Font clock_font;
    sf::Text clock_text;
    sf::RectangleShape clock_bg_rect;
    sf::Clock clock;
    bool timerStarted;
    int elapsedTime;

    sf::Text mine_cnt_text;
    sf::RectangleShape mine_cnt_bg_rect;

    // Window fnc
    void handle_events(sf::Event &event);
    void update();
    void display();

    // Game fnc
    void fillGrid();
    void revealMines();
    void revealNeighbours(int curr_row, int curr_col);
    bool isRevealed(int row, int col);
    bool isWinner();
};
