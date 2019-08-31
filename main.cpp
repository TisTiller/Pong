#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <windows.h>
#include <winuser.h>
#include <chrono>
#include <wtypes.h>
#include <iomanip>
#include <sstream>
#include <math.h>

int GetDesktopResolution(char xy);

std::string ConvertToString(float c);

void resetGame();

//
// START GAME
//

std::map<char, int> desktopRes = {{'x', GetDesktopResolution('x')}, {'y', GetDesktopResolution('y')}};

// True is Right and Down
// Ball Direction
bool lr = false;
bool ud = true;

// Left Racket
bool lmoving = false;
bool lud = true;

// Right Racket
bool rmoving = false;
bool rud = false;

// Ball Speed Multiplier - 0.5 is normal.
float spd = 0.5f;

// Size of the ball (Radius)
int ballsize = 20;

// Size of Paddles
sf::Vector2f RackSize(15, 100);

// Location of the ball start.
sf::Vector2i ballstart(desktopRes['x']/2, desktopRes['y']/2);

// Score Dictionary
std::map<char, int> score = {{'l', 0}, {'r', 0}};

// Objects
sf::Text score_text;
sf::CircleShape pongball(10.f);
sf::RectangleShape LRacket;
sf::RectangleShape RRacket;

int main()
{
    // Whether The Game is Started Yet.
    bool started = false;
    bool fullscreen = true;
    bool paused = false;
    bool musicplayed = false;
    char mode = 'm';

    // Background Fun.
    bool fun = true;
    sf::Color bgcol(0, 0, 0);
    int frame = 0;


    sf::RenderWindow window(sf::VideoMode(desktopRes['x'], desktopRes['y']), "PONG", (fullscreen ? sf::Style::Fullscreen : sf::Style::Resize));

    sf::ContextSettings settings;
    settings.antialiasingLevel = 2;

// Hooray Sound Effect
    sf::SoundBuffer Hooray;
    if (!Hooray.loadFromFile("Assets/Cheer.wav")) {
        std::cout << "Hooray Sound Effect Not Loaded" << std::endl;
    }

// Start Music
    sf::Music introMusic;
    if (!introMusic.openFromFile("Assets/ES_Detective_Work.wav")) {
        std::cout << "Intro Music Not Loaded!" << std::endl;
    }

// Paused Screen Tint
    sf::Texture rtint;
    if (!rtint.loadFromFile("Assets/PausedRed.png")) {
        std::cout << "Pause Texture Not Loaded!" << std::endl;
    }

// Roboto Font
    sf::Font Roboto;
    if (!Roboto.loadFromFile("Assets/Roboto/Roboto-Regular.ttf")) {
        std::cout << "Roboto Not Loaded!" << std::endl;
    }

// Lexend Peta Font
    sf::Font Lexend_Peta;
    if (!Lexend_Peta.loadFromFile("Assets/Lexend_Peta/LexendPeta-Regular.ttf")) {
        std::cout << "Lexend Peta Not Loaded!" << std::endl;
    }

// Pause Cover
    sf::RectangleShape paused_cover;
    sf::Vector2f coverSize(desktopRes['x'], desktopRes['y']);
    paused_cover.setSize(coverSize);
    paused_cover.setTexture(&rtint);

// Text Overlay
    sf::RectangleShape overlay;
    sf::Vector2f overlaySize(0, 0);
    overlay.setSize(overlaySize);
    overlay.setTexture(&rtint);

    //
    // Real Sprites
    //

// Pong Ball
    pongball.setRadius(ballsize/2);
    pongball.setFillColor(sf::Color::White);
    pongball.setOrigin(pongball.getGlobalBounds().width/2, pongball.getGlobalBounds().height/2);
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

// Title Text
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

                    // Plus (Technically Equals) - Increase Volume
                        case sf::Keyboard::Equal:
                            if (sf::Listener::getGlobalVolume()+10 > 100) {
                                sf::Listener::setGlobalVolume(100);
                            } else {
                                sf::Listener::setGlobalVolume(sf::Listener::getGlobalVolume()+10);
                            }
                            break;

                    // Minus - Decrease Volume
                        case sf::Keyboard::Hyphen:
                            if (sf::Listener::getGlobalVolume()-10 < 0) {
                                sf::Listener::setGlobalVolume(0);
                            } else {
                                sf::Listener::setGlobalVolume(sf::Listener::getGlobalVolume()-10);
                            }
                            break;


                    // Pause The Game
                        case sf::Keyboard::P:
                            paused = !paused;
                            break;

                    // Start Game
                        case sf::Keyboard::Enter:
                            started = true;

                            // Background Colors (Set To Black When Started!)
                            fun = false;

                            bgcol.r = 0;
                            bgcol.g = 0;
                            bgcol.b = 0;

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

            if (rmoving) {
                RRacket.move(0, (rud ? spd : -spd));
            }
            if (lmoving) {
                LRacket.move(0, (lud ? spd : -spd));
            }

            // If it is in demo mode.
            if (mode == 'd') {
                lmoving = true;
                if ((LRacket.getPosition().y > desktopRes['y'] - RackSize.y/2) || (LRacket.getPosition().y < RackSize.y/2)) {
                    lud = !lud;
                }
            }

            // If it is in bot(/demo) mode.
            if ((mode == 'd') || (mode == 'b')) {
                rmoving = true;
                if ((RRacket.getPosition().y > desktopRes['y'] - RackSize.y/2) || (RRacket.getPosition().y < RackSize.y/2)) {
                    rud = !rud;
                }
            }

            if (!(mode == 'd')) {
                if (LRacket.getPosition().y > desktopRes['y'] - RackSize.y/2) {
                    LRacket.move(0, -spd);
                } else if (LRacket.getPosition().y < RackSize.y/2) {
                    LRacket.move(0, spd);
                }
            }

            if (mode == 'm') {
                if (RRacket.getPosition().y > desktopRes['y'] - RackSize.y/2) {
                    RRacket.move(0, -spd);
                } else if (RRacket.getPosition().y < RackSize.y/2) {
                    RRacket.move(0, spd);
                }
            }

            // If the ball hits a paddle.
            if ((pongBound.intersects(LRacketBound)) || (pongBound.intersects(RRacketBound))) {
                while (true) {
                    if ((pongBound.intersects(LRacketBound)) || (pongBound.intersects(RRacketBound))) {
                        pongball.move((lr ? -10 : 10), 0);
                    } else {
                        break;
                    }
                    break;
                }
                lr = !lr;
            }

            // If the Ball hits a roof or floor.
            if ((pongball.getPosition().y < pongball.getRadius()) || (pongball.getPosition().y > desktopRes['y'] - pongball.getRadius())) {
                ud = !ud;
            }

            // If the Ball hits a Wall (Point Given)
            if ((pongball.getPosition().x < pongball.getRadius()) || (pongball.getPosition().x > desktopRes['x'])) {
                if (pongball.getPosition().x < pongball.getRadius()) {
                    score['r']++;
                } else if (pongball.getPosition().x > desktopRes['x'] - pongball.getRadius()) {
                    score['l']++;
                }

            // Play Hoorsy Sound
                sf::Sound hooray;
                hooray.setBuffer(Hooray);
                hooray.play();

                while (hooray.getStatus() == 2) {

                }

                lr = false;
                resetGame();

            }
        }

        //
        // Create The Frame
        //

    // Debug Info
        speed_text.setString("Speed: " + ConvertToString(spd*2) + "\nVolume: " + ConvertToString(sf::Listener::getGlobalVolume()) + "\n(" + ConvertToString(pongball.getPosition().x) + ", " + ConvertToString(pongball.getPosition().y) + ")");

    // Overlay Cover
        overlaySize.x = speed_text.getGlobalBounds().width*1.1;
        overlaySize.y = speed_text.getGlobalBounds().height*1.1;
        overlay.setSize(overlaySize);

    // Random Color Gen
        if (!paused) {
            if (fun) {
                frame++;
                if (frame == 100) {
                    bgcol.r = (rand() % 255);
                    bgcol.g = (rand() % 255);
                    bgcol.b = (rand() % 255);
                    frame = 0;
                }
            }
        }

        // Clear The Frame
        window.clear(bgcol);

        // Draw The Entities
        window.draw(LRacket);
        window.draw(RRacket);

        if (started) {
            window.draw(overlay);
            window.draw(speed_text);
            window.draw(score_text);
            window.draw(pongball);
        } else {
            if (!musicplayed) {
                introMusic.play();
                musicplayed = true;
            }
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

std::string ConvertToString(float c)
{
    std::stringstream conv;
    conv << std::fixed << std::setprecision(0) << c;
    std::string converted = conv.str();
    return converted;
}

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
