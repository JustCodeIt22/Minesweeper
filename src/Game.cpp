#include "Game.hpp"

// void print(int arr[12][12])
// {
//     for (int i = 0; i < 12; i++)
//     {
//         for (int j = 0; j < 12; j++)
//         {
//             std::cout << arr[i][j] << " ";
//         }
//         std::cout << "\n";
//     }
// }

void resize(sf::Sprite &s, sf::Vector2f sprite_size)
{
    s.setScale(sprite_size.x / s.getGlobalBounds().width, sprite_size.y / s.getGlobalBounds().height);
}

Game::Game(int width, int height, std::string title) : mWindow(sf::VideoMode(width, height), title), scoreBoardRect(sf::Vector2f(width, 100))
{
    // setting icon
    sf::Image icon;
    icon.loadFromFile("../assets/imgs/minesweeper_ic.png");
    mWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // srand() -> changes the seed or starting point of rand() algorithm.
    // (here it uses current time as seed for random generator)
    srand(time(0));

    scoreBoardRect.setFillColor(sf::Color(0xc0c0c0ff));

    tile_tex.loadFromFile("../assets/imgs/minesweeper_sprite_sheet.png");
    tile_sprite.setTexture(tile_tex);

    score_board_tex.loadFromFile("../assets/imgs/scoreBoardBg.png");
    score_board_sprite.setTexture(score_board_tex);
    resize(score_board_sprite, sf::Vector2f(width, 100));

    moods_tex.loadFromFile("../assets/imgs/moods_sprite_sheet.png");
    moods_sprite.setTexture(moods_tex);
    moods_sprite.setTextureRect(sf::IntRect(0, 0, 24 * 3, 24 * 3)); // since the tex was of 24 x 24 and scaled by (3, 3);
    moods_sprite.setPosition(width / 2 - moods_sprite.getGlobalBounds().width / 2, 15);

    timerStarted = false;
    elapsedTime = 0; // elapsedTime is in seconds goto line 189 or in update method
    clock_font.loadFromFile("../assets/fonts/digital_clock.otf");
    clock_text.setFont(clock_font);
    clock_text.setCharacterSize(60);
    clock_text.setString("0:00");
    clock_text.setPosition(width - 150, 10);
    clock_text.setFillColor(sf::Color::Red);
    clock_bg_rect.setSize(sf::Vector2f(clock_text.getGlobalBounds().width + 16, clock_text.getGlobalBounds().height + 15));
    clock_bg_rect.setFillColor(sf::Color::Black);
    clock_bg_rect.setPosition(clock_text.getPosition().x - 8, clock_text.getPosition().y + 12);

    mine_cnt_text.setFont(clock_font);
    mine_cnt_text.setCharacterSize(60);
    mine_cnt_text.setString("000");
    mine_cnt_text.setPosition(50, 10);
    mine_cnt_text.setFillColor(sf::Color::Red);
    mine_cnt_bg_rect.setSize(sf::Vector2f(mine_cnt_text.getGlobalBounds().width + 16, mine_cnt_text.getGlobalBounds().height + 15));
    mine_cnt_bg_rect.setFillColor(sf::Color::Black);
    mine_cnt_bg_rect.setPosition(mine_cnt_text.getPosition().x - 8, mine_cnt_text.getPosition().y + 12);

    fillGrid();
}

Game::~Game()
{
    if (mWindow.isOpen())
        mWindow.close();
}

void Game::run()
{
    while (mWindow.isOpen())
    {
        sf::Event event;
        handle_events(event);
        update();
        display();
    }
}

void Game::handle_events(sf::Event &event)
{
    while (mWindow.pollEvent(event))
    {

        mouse_pos = sf::Mouse::getPosition(mWindow);

        clicked_col = (mouse_pos.x / SQ_SIZE);
        clicked_row = ((mouse_pos.y - 100) / SQ_SIZE);

        // Window Events
        if (event.type == sf::Event::Closed)
        {
            mWindow.close();
        }

        // Mouse Events
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                if (moods_sprite.getGlobalBounds().contains(mouse_pos.x, mouse_pos.y))
                {
                    moods_sprite.setTextureRect(sf::IntRect(24 * 3, 0, 24 * 3, 24 * 3));
                    // restart
                    fillGrid();
                    clock.restart();
                    timerStarted = true;
                }
                else
                {
                    // start the timer only if timer is not started
                    if (!timerStarted)
                    {
                        clock.restart();
                        timerStarted = true;
                    }

                    sgrid[clicked_row][clicked_col] = grid[clicked_row][clicked_col];
                    // if the clicked box have mine
                    if (grid[clicked_row][clicked_col] == 9)
                    {
                        revealMines();

                        moods_sprite.setTextureRect(sf::IntRect(2 * 24 * 3, 0, 24 * 3, 24 * 3));
                        std::cout << "Game Over\n";
                        timerStarted = false;
                    }

                    if (grid[clicked_row][clicked_col] == 0)
                    {
                        revealNeighbours(clicked_row, clicked_col);
                    }
                }
            }
            else if (event.mouseButton.button == sf::Mouse::Right)
            {
                if (mine_cnt > 0)
                {
                    // start the timer
                    if (!timerStarted)
                    {
                        clock.restart();
                        timerStarted = true;
                    }
                    sgrid[clicked_row][clicked_col] = 11;
                    mine_cnt--;
                }
            }

            // check if the player is winnner
            if (isWinner())
            {
                timerStarted = false;
                moods_sprite.setTextureRect(sf::IntRect(4 * 24 * 3, 0, 24 * 3, 24 * 3));
            }
        }
        if (event.type == sf::Event::MouseButtonReleased)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                if (moods_sprite.getGlobalBounds().contains(mouse_pos.x, mouse_pos.y))
                {
                    moods_sprite.setTextureRect(sf::IntRect(0, 0, 24 * 3, 24 * 3));
                }
            }
        }
    }
}

void Game::update()
{
    for (int row = 0; row < ROWS; row++)
    {
        for (int col = 0; col < COLS; col++)
        {
            tile_sprite.setTextureRect(sf::IntRect(sgrid[row][col] * SQ_SIZE, 0, SQ_SIZE, SQ_SIZE));
            tile_sprite.setPosition(col * SQ_SIZE, (row * SQ_SIZE) + 100);
            mWindow.draw(tile_sprite);
        }
    }

    // Clock Update
    // If the timer hasn't started, display "00:00"
    elapsedTime = timerStarted ? static_cast<int>(clock.getElapsedTime().asSeconds()) : elapsedTime;

    // Convert elapsed time to minutes and seconds
    int minutes = elapsedTime / 60;
    int seconds = elapsedTime % 60;

    // Format time as "minute:second"
    std::ostringstream timeStream;
    timeStream << std::setw(1) << std::setfill('0') << minutes << ":"
               << std::setw(2) << std::setfill('0') << seconds;

    // Set the timer text
    clock_text.setString(timeStream.str());

    // Mine Counter
    std::ostringstream mineCntStream;
    mineCntStream << std::setw(3) << std::setfill('0') << mine_cnt;

    mine_cnt_text.setString(mineCntStream.str());
}

void Game::display()
{
    mWindow.clear(sf::Color::Black);
    update();
    mWindow.draw(scoreBoardRect);
    mWindow.draw(score_board_sprite);
    mWindow.draw(moods_sprite);

    // drawing text
    mWindow.draw(mine_cnt_bg_rect);
    mWindow.draw(mine_cnt_text);
    mWindow.draw(clock_bg_rect);
    mWindow.draw(clock_text);
    mWindow.display();
}

void Game::fillGrid()
{
    mine_cnt = 0;

    // filling grid
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            sgrid[i][j] = 10;
            if (rand() % 20 == 0)
            {
                grid[i][j] = 9;
                mine_cnt++;
            }
            else
            {
                grid[i][j] = 0;
            }
        }
    }

    // Placing numbers
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            int n = 0;
            if (grid[i][j] == 9)
            {
                continue;
            }
            if (grid[i + 1][j] == 9)
            {
                n++;
            }
            if (grid[i][j + 1] == 9)
            {
                n++;
            }
            if (grid[i - 1][j] == 9)
            {
                n++;
            }
            if (grid[i][j - 1] == 9)
            {
                n++;
            }
            if (grid[i + 1][j + 1] == 9)
            {
                n++;
            }
            if (grid[i - 1][j - 1] == 9)
            {
                n++;
            }
            if (grid[i + 1][j - 1] == 9)
            {
                n++;
            }
            if (grid[i - 1][j + 1] == 9)
            {
                n++;
            }
            grid[i][j] = n;
        }
    }
}

void Game::revealMines()
{
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            if (grid[i][j] == 9)
            {
                sgrid[i][j] = grid[i][j];
            }
        }
    }
}

void Game::revealNeighbours(int curr_row, int curr_col)
{
    std::queue<std::pair<int, int>> q;
    std::vector<int> dx = {0, 1, 0, -1};
    std::vector<int> dy = {1, 0, -1, 0};

    q.push({curr_row, curr_col});

    while (!q.empty())
    {

        int row = q.front().first;
        int col = q.front().second;
        q.pop();

        for (int i = 0; i < 4; i++)
        {
            int nRow = row + dx[i];
            int nCol = col + dy[i];

            if ((nRow >= 0 && nRow < 12) && (nCol >= 0 && nCol < 12) && grid[nRow][nCol] == 0 && sgrid[nRow][nCol] == 10)
            {
                sgrid[nRow][nCol] = grid[nRow][nCol];
                q.push({nRow, nCol});

                if (grid[nRow + 1][nCol] || grid[nRow - 1][nCol] || grid[nRow][nCol + 1] || grid[nRow][nCol - 1])
                {
                    if (grid[nRow + 1][nCol] && nRow + 1 < 12)
                    {
                        sgrid[nRow + 1][nCol] = grid[nRow + 1][nCol];
                    }
                    if (grid[nRow - 1][nCol] && nRow - 1 >= 0)
                    {
                        sgrid[nRow - 1][nCol] = grid[nRow - 1][nCol];
                    }
                    if (grid[nRow][nCol + 1] && nCol + 1 < 12)
                    {
                        sgrid[nRow][nCol + 1] = grid[nRow][nCol + 1];
                    }
                    if (grid[nRow][nCol - 1] && nCol - 1 >= 0)
                    {
                        sgrid[nRow][nCol - 1] = grid[nRow][nCol - 1];
                    }
                }
            }
        }
    }

    // This reveals the clicked box neighbours
    if (grid[curr_row + 1][curr_col] || grid[curr_row - 1][curr_col] || grid[curr_row][curr_col + 1] || grid[curr_row][curr_col - 1])
    {
        if (grid[curr_row + 1][curr_col] && curr_row + 1 < 12)
        {
            sgrid[curr_row + 1][curr_col] = grid[curr_row + 1][curr_col];
        }
        if (grid[curr_row - 1][curr_col] && curr_row - 1 >= 0)
        {
            sgrid[curr_row - 1][curr_col] = grid[curr_row - 1][curr_col];
        }
        if (grid[curr_row][curr_col + 1] && curr_col + 1 < 12)
        {
            sgrid[curr_row][curr_col + 1] = grid[curr_row][curr_col + 1];
        }
        if (grid[curr_row][curr_col - 1] && curr_col - 1 >= 0)
        {
            sgrid[curr_row][curr_col - 1] = grid[curr_row][curr_col - 1];
        }
    }
}

bool Game::isRevealed(int row, int col)
{
    return sgrid[row][col] != 10;
}

bool Game::isWinner()
{
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            if (!isRevealed(i, j) && grid[i][j] != 9)
            {
                // std::cout << i << " " << j << "\n"; // don't uncomment this line !!
                return false;
            }
        }
    }

    return true;
}
