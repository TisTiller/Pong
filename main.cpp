#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <windows.h>
#include <winuser.h>
#include <chrono>
#include <wtypes.h>
#include <iomanip>
#include <sstream>

int GetDesktopResolution(char xy)
{
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);

    int horizontal = desktop.right;
    int vertical = desktop.bottom;

    if (xy == 'x') {
        return horizontal;
    }
    else if (xy == 'y') {
        return vertical;
    }
    else {
        return 0;
    }
}

std::map<char, int> desktopRes = {{'x', GetDesktopResolution('x')}, {'y', GetDesktopResolution('y')}};

int ballsize = 20;
sf::Vector2f RackSize(20, 100);

// True is Right and Down
// Ball Direction
bool lr = true;
bool ud = true;

// Left Racket
bool lmoving = false;
bool lud = true;

// Right Racket
bool rmoving = false;
bool rud = false;

bool started = false;

// Ball Speed Multiplier - 0.5 is normal.
float spd = 0.5f;
sf::Vector2i ballstart(desktopRes['x']/2, desktopRes['y']/2);

std::map<char, int> score = {{'l', 0}, {'r', 0}};

sf::Text score_text;
sf::CircleShape pongball(10.f);
sf::RectangleShape LRacket;
sf::RectangleShape RRacket;


std::string ConvertToString(float c)
{
    std::stringstream conv;
    conv << std::fixed << std::setprecision(0) << c;
    std::string converted = conv.str();
    return converted;
}

void resetGame()
{
    // Left Racket
    lmoving = false;
    lud = true;
    LRacket.setPosition(desktopRes['x']/12, desktopRes['y']/2);

    // Right Racket
    rmoving = false;
    rud = false;
    RRacket.setPosition(desktopRes['x']/12*11, desktopRes['y']/2);


    score_text.setString(std::to_string(score['l']) + " : " + std::to_string(score['r']));
    pongball.setRadius(ballsize/2);
    pongball.setFillColor(sf::Color::White);
    pongball.setOrigin(pongball.getGlobalBounds().width, pongball.getGlobalBounds().height);
    pongball.setPosition(ballstart.x, ballstart.y+(rand() % (desktopRes['y']/4)));


}

int main()
{
    bool fullscreen = true;
    bool paused = false;
    char mode = 'm';

    sf::RenderWindow window(sf::VideoMode(desktopRes['x'], desktopRes['y']), "PONG", sf::Style::Fullscreen);

    sf::ContextSettings settings;
    settings.antialiasingLevel = 2;

// Paused Screen Tint
    sf::Texture rtint;
    if (!rtint.loadFromFile("PausedRed.png")) {
        std::cout << "Pause Texture Not Loaded!" << std::endl;
    }

// Roboto Font
    sf::Font Roboto;
    if (!Roboto.loadFromFile("Roboto/Roboto-Regular.ttf")) {
        std::cout << "Roboto Not Loaded!\n" << std::flush;
    }

// Pause Cover
    sf::RectangleShape paused_cover;
    sf::Vector2f rectSize(desktopRes['x'], desktopRes['y']);
    paused_cover.setSize(rectSize);
    paused_cover.setTexture(&rtint);


    //
    // Real Sprites
    //

// Pong Ball
    pongball.setRadius(ballsize/2);
    pongball.setFillColor(sf::Color::White);
    pongball.setOrigin(pongball.getGlobalBounds().width, pongball.getGlobalBounds().height);
    pongball.setPosition(ballstart.x, ballstart.y);

// Left Racket
    LRacket.setFillColor(sf::Color::White);
    LRacket.setSize(RackSize);
    LRacket.setOrigin(LRacket.getGlobalBounds().width/2, LRacket.getGlobalBounds().height/2);
    LRacket.setPosition(desktopRes['x']/12, desktopRes['y']/2);

// Right Racket
    RRacket.setFillColor(sf::Color::White);
    RRacket.setSize(RackSize);
    RRacket.setOrigin(RRacket.getGlobalBounds().width/2, RRacket.getGlobalBounds().height/2);
    RRacket.setPosition(desktopRes['x']/12*11, desktopRes['y']/2);

// Speed Text
    sf::Text speed_text;
    speed_text.setFont(Roboto);
    speed_text.setCharacterSize(40);
    speed_text.setString("Speed: " + std::to_string(spd));
    speed_text.setFillColor(sf::Color::White);

// Paused Text
    sf::Text pause_text;
    pause_text.setFont(Roboto);
    pause_text.setCharacterSize(40);
    pause_text.setString("Paused");
    pause_text.setFillColor(sf::Color::White);
    pause_text.setOrigin(pause_text.getGlobalBounds().width/2, pause_text.getGlobalBounds().height/2);
    pause_text.setPosition(desktopRes['x']/2, floor(desktopRes['y']/2));

// Score Text
    score_text.setFont(Roboto);
    score_text.setCharacterSize(60);
    score_text.setString("0 : 0");
    score_text.setFillColor(sf::Color::White);
    score_text.setOrigin(score_text.getGlobalBounds().width/2, score_text.getGlobalBounds().height/2);
    score_text.setPosition(desktopRes['x']/2, floor(desktopRes['y']/3));

// Title Text (& Font) (Lexend Peta)
    sf::Font Lexend_Peta;
    Lexend_Peta.loadFromFile("Lexend_Peta/LexendPeta-Regular.ttf");

    sf::Text title;
    title.setFont(Lexend_Peta);
    title.setCharacterSize(80);
    title.setFillColor(sf::Color::White);
    title.setString("PONG!");
    title.setStyle(sf::Text::Bold);
    title.setOrigin(title.getGlobalBounds().width/2, title.getGlobalBounds().height/2);
    title.setPosition(desktopRes['x']/2, floor(desktopRes['y']/3));

// Enter To Start
    sf::Text start;
    start.setFont(Lexend_Peta);
    start.setCharacterSize(40);
    start.setFillColor(sf::Color::White);
    start.setString("Press Enter To Start.");
    start.setStyle(sf::Text::Bold);
    start.setOrigin(start.getGlobalBounds().width/2, start.getGlobalBounds().height/2);
    start.setPosition(desktopRes['x']/2, floor(desktopRes['y']/3+80));

// Bounding Boxes
    sf::FloatRect pongBound = pongball.getGlobalBounds();
    sf::FloatRect LRacketBound = LRacket.getGlobalBounds();
    sf::FloatRect RRacketBound = RRacket.getGlobalBounds();

// Check For Input
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type) {

            // Closing The Window
                case sf::Event::Closed:
                    window.close();
                    break;

            // Lose Focus (Pause the game)
                case sf::Event::LostFocus:
                    paused = true;
                    break;

            // Gain Focus (Unpause the game)
                case sf::Event::GainedFocus:
                    paused = false;
                    break;

            // Keys Being Pressed
                case sf::Event::KeyPressed:
                    switch (event.key.code) {

                    // Escape Key (Close the game)
                        case sf::Keyboard::Escape:
                            window.close();
                            break;

                    // Left Control (Windowed/Fullscreen)
                        case sf::Keyboard::LControl:
                            fullscreen = !fullscreen;
                            window.create( \
                                    sf::VideoMode(desktopRes['x']*0.75, desktopRes['y']*0.75),
                                    "PONG", \
                                    (fullscreen ? sf::Style::Fullscreen : sf::Style::Close));

                            if (!fullscreen) {
                                desktopRes['x'] *= 0.75;
                                desktopRes['y'] *= 0.75;

                                LRacket.setPosition(LRacket.getPosition().x*0.75, LRacket.getPosition().y*0.75);
                                RRacket.setPosition(RRacket.getPosition().x*0.75, RRacket.getPosition().y*0.75);
                                pongball.setPosition(pongball.getPosition().x*0.75, pongball.getPosition().y*0.75);


                            } else {
                                desktopRes['x'] = GetDesktopResolution('x');
                                desktopRes['y'] = GetDesktopResolution('y');
                                LRacket.setPosition(desktopRes['x']/12, desktopRes['y']/2);
                                RRacket.setPosition(desktopRes['x']/12*11, desktopRes['y']/2);
                                pongball.setPosition(ceil(pongball.getPosition().x*1.33), ceil(pongball.getPosition().y*1.33));
                            }
                            score_text.setPosition(desktopRes['x']/2, floor(desktopRes['y']/3));
                            pause_text.setPosition(desktopRes['x']/2, floor(desktopRes['y']/2));
                            title.setPosition(desktopRes['x']/2, floor(desktopRes['y']/3));
                            start.setPosition(desktopRes['x']/2, floor(desktopRes['y']/3+80));

                            break;

                    // Right Bracket (Speed Up The Game)
                        case sf::Keyboard::RBracket:
                            spd += 0.5f;
                            break;

                    // Left Bracket (Slow Down The Game)
                        case sf::Keyboard::LBracket:
                            if (spd > 0.5) {
                                spd -= 0.5f;
                            }
                            break;

                    // Pause The Game
                        case sf::Keyboard::P:
                            paused = !paused;
                            break;

                    // Start Game
                        case sf::Keyboard::Enter:
                            started = true;
                            break;

                    // Game Modes
                        case sf::Keyboard::M:
                            resetGame();
                            mode = 'm';
                            break;

                        case sf::Keyboard::B:
                            resetGame();
                            mode = 'b';
                            break;

                        case sf::Keyboard::D:
                            resetGame();
                            mode = 'd';
                            break;

                    // Paddle Movement (Press)
                        case sf::Keyboard::W:
                            lmoving = true;
                            lud = false;
                            break;

                        case sf::Keyboard::S:
                            lmoving = true;
                            lud = true;
                            break;

                        case sf::Keyboard::Up:
                            rmoving = true;
                            rud = false;
                            break;

                        case sf::Keyboard::Down:
                            rmoving = true;
                            rud = true;
                            break;
                    }
                    break;

                // Paddle Movement (Release)
                case sf::Event::KeyReleased:
                    switch (event.key.code) {

                        case sf::Keyboard::W:
                            lmoving = false;
                            break;

                        case sf::Keyboard::S:
                            lmoving = false;
                            break;

                        case sf::Keyboard::Up:
                            rmoving = false;
                            break;

                        case sf::Keyboard::Down:
                            rmoving = false;
                            break;
                    }
            }
        }

    // Run the Logic
        pongBound = pongball.getGlobalBounds();
        LRacketBound = LRacket.getGlobalBounds();
        RRacketBound = RRacket.getGlobalBounds();

        if (!paused) {
            if (started) {
                pongball.move((lr ? spd : -spd), (ud ? spd : -spd));
            }

            // If it is in demo mode.
            if (mode == 'd') {
                lmoving = true;
                LRacket.setPosition(LRacket.getPosition().x, LRacket.getPosition().y + (lud ? -spd : spd));
                if ((LRacket.getPosition().y > desktopRes['y'] - RackSize.y/2) || (LRacket.getPosition().y < RackSize.y/2)) {
                    lud = !lud;
                }
            }

            // If it is in bot(/demo) mode.
            if ((mode == 'd') || (mode == 'b')) {
                rmoving = true;
                RRacket.setPosition(RRacket.getPosition().x, RRacket.getPosition().y + (rud ? -spd : spd));
                if ((RRacket.getPosition().y > desktopRes['y'] - RackSize.y/2) || (RRacket.getPosition().y < RackSize.y/2)) {
                    rud = !rud;
                }
            }

            // If it is in multiplayer(/bot) mode.
            if ((mode == 'm') || (mode == 'b')) {
                if (lmoving) {
                    if (lud) {
                        LRacket.move(0, spd);
                    } else {
                        LRacket.move(0, -spd);
                    }
                }
            }

            // If it is in multiplayer mode.
            if (mode == 'm') {
                if (rmoving) {
                    if (rud) {
                        RRacket.move(0, spd);
                    } else {
                        RRacket.move(0, -spd);
                    }
                }
            }

            // If the ball hits a paddle.
            if ((pongBound.intersects(LRacketBound)) || (pongBound.intersects(RRacketBound))) {
                pongball.move((lr ? -1 : 1), 0);
                lr = !lr;
            }

            // If the Ball hits a roof or floor.
            if ((pongball.getPosition().y < pongball.getRadius()) || (pongball.getPosition().y > desktopRes['y'] - pongball.getRadius())) {
                ud = !ud;
            }

            // If the Ball hits a Wall (Point Given)
            if ((pongball.getPosition().x < pongball.getRadius()) || (pongball.getPosition().x > desktopRes['x'] - pongball.getRadius())) {
                if (pongball.getPosition().x < pongball.getRadius()) {
                    score['r']++;
                } else if (pongball.getPosition().x > desktopRes['x'] - pongball.getRadius()) {
                    score['l']++;
                }

                resetGame();

            }
        }

        //
        // Create The Frame
        //

        // Debug Info
        speed_text.setString("Speed: " + ConvertToString(spd*2) + "\n (" + ConvertToString(pongball.getPosition().x) + ", " + ConvertToString(pongball.getPosition().y) + ")");

        // Clear The Frame
        window.clear();

        // Draw The Entities
        window.draw(LRacket);
        window.draw(RRacket);

        if (started) {
            window.draw(speed_text);
            window.draw(score_text);
            window.draw(pongball);
        } else {
            window.draw(title);
            window.draw(start);
        }

        // Draw Paused Entities
        if (paused) {
            window.draw(paused_cover);
            window.draw(pause_text);
        }

        // Display Frame
        window.display();
    }

    return 0;
}
