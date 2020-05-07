#include <SFML/Graphics.hpp>
#include <iostream>
#include <SFML/Network.hpp>
#include <chrono>
#include "PaddleManager.h"
#include "Ball.h"
#include "GlobalConstants.h"
#include <stdio.h>
#include <stdlib.h>

enum GameState
{
    MAIN_MENU,
    HOST_MENU,
    JOIN_MENU,
    GAME
};

bool isWithinButtonBounds(sf::Vector2i mousePos, sf::Vector2f buttonPosition, float buttonW, float buttonH);
void update_score(sf::Text& score1, sf::Text& score2, int score[2]);


int main()
{
    srand(time(NULL));
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // Constants
    const float playerVelocity = 0.5f;
    const float ballVelocity = 5.f;
    const long nanoSecondsPerMillisecond = 1000000;
    const long timePerTick = 5 * nanoSecondsPerMillisecond;
    const float buttonSizeW = 400;
    const float buttonSizeH = 200;
    const float textFieldSizeW = 600;
    const float textFieldSizeH = 300;

    // Other game variables
    float waitingToConnect = true;
    bool isServing = true;
    int playerNum = 0;
    int playerServing = 0;
    int score[2] = { 0, 0 };
    bool dataEntered = false;
    bool addressFieldFocused = false;
    bool portFieldFocused = false;

    // Game font
    sf::Font font;
    font.loadFromFile("font.ttf");

    // Text Fields
    sf::Text score1;
    sf::Text score2;

    // Game states
    GameState game_state = MAIN_MENU;


    // Views and window
    sf::View game_view;
    sf::RenderWindow window(sf::VideoMode(1024, 512), "PONG");


    const unsigned int windowWidth = window.getSize().x;
    const unsigned int windowHeight = window.getSize().y;

    // game objects & variables
    PaddleManager paddleManager(window.getSize());
    float ballXPos = (window.getSize().x / 2);
    float ballYPos = (window.getSize().y / 2);

    Ball ball(globalConsts::ballRadius, ballXPos, ballYPos);

    // main menu objects & variables
    sf::RectangleShape hostRect;
    sf::RectangleShape joinRect;
    sf::RectangleShape addressRect;
    sf::RectangleShape connectRect;
    sf::RectangleShape portRect;
    sf::FloatRect textRect;
    sf::Text hostText("Host a Game", font);
    sf::Text joinText("Join a Game", font);
    sf::Text connectText("Connect", font);
    sf::Text addressTextField("", font);
    sf::Text portTextField("", font);
    sf::String addressInput;
    sf::String portInput;

    // host button
    hostRect.setFillColor(sf::Color::Green);
    hostRect.setSize(sf::Vector2f(buttonSizeW, buttonSizeH));
    hostRect.setOrigin(sf::Vector2f(buttonSizeW / 2, buttonSizeH / 2));
    hostRect.setPosition(sf::Vector2f(window.getSize().x / 4, window.getSize().y / 2));

    hostText.setCharacterSize(30);
    hostText.setStyle(sf::Text::Bold);
    hostText.setFillColor(sf::Color::Red);
    textRect = hostText.getLocalBounds();
    hostText.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
    hostText.setPosition(sf::Vector2f(hostRect.getPosition().x, hostRect.getPosition().y));

    //center text
    // join button
    joinRect.setFillColor(sf::Color::Cyan);
    joinRect.setSize(sf::Vector2f(buttonSizeW, buttonSizeH));
    joinRect.setOrigin(sf::Vector2f(buttonSizeW / 2, buttonSizeH / 2));
    joinRect.setPosition(sf::Vector2f(3 * window.getSize().x / 4, window.getSize().y / 2));

    joinText.setCharacterSize(30);
    joinText.setStyle(sf::Text::Bold);
    joinText.setFillColor(sf::Color::Red);
    textRect = joinText.getLocalBounds();
    joinText.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
    joinText.setPosition(sf::Vector2f(joinRect.getPosition().x, joinRect.getPosition().y));

    // Connect button
    connectRect.setFillColor(sf::Color::Cyan);
    connectRect.setSize(sf::Vector2f(buttonSizeW, buttonSizeH));
    connectRect.setOrigin(sf::Vector2f(buttonSizeH / 2, buttonSizeW / 2));
    connectRect.setPosition(sf::Vector2f( 4 * window.getSize().x / 5, window.getSize().y / 2));

    connectText.setCharacterSize(30);
    connectText.setStyle(sf::Text::Bold);
    connectText.setFillColor(sf::Color::Red);
    textRect = connectText.getLocalBounds();
    connectText.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
    connectText.setPosition(sf::Vector2f(connectRect.getPosition().x, connectRect.getPosition().y));

    // address text field
    addressRect.setSize(sf::Vector2f(textFieldSizeW, textFieldSizeH));
    addressRect.setOrigin(sf::Vector2f(textFieldSizeW / 2, textFieldSizeH / 2));
    addressRect.setPosition(sf::Vector2f(windowWidth / 2, windowHeight / 4 - 20));
    addressRect.setFillColor(sf::Color::Black);
    addressRect.setOutlineColor(sf::Color::White);
    addressRect.setOutlineThickness(10);

    addressTextField.setCharacterSize(30);
    addressTextField.setStyle(sf::Text::Bold);
    addressTextField.setFillColor(sf::Color::White);
    textRect = addressTextField.getLocalBounds();
    addressTextField.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
    addressTextField.setPosition(sf::Vector2f(addressRect.getPosition().x - addressRect.getSize().x/2 + 5, addressRect.getPosition().y));// address text field
   
    // port text field 
    portRect.setSize(sf::Vector2f(textFieldSizeW, textFieldSizeH));
    portRect.setOrigin(sf::Vector2f(textFieldSizeW / 2, textFieldSizeH / 2));
    portRect.setPosition(sf::Vector2f(windowWidth / 2, 3 * windowHeight / 4 - 20));
    portRect.setFillColor(sf::Color::Black);
    portRect.setOutlineColor(sf::Color::White);
    portRect.setOutlineThickness(10);

    portTextField.setCharacterSize(30);
    portTextField.setStyle(sf::Text::Bold);
    portTextField.setFillColor(sf::Color::White);
    textRect = portTextField.getLocalBounds();
    portTextField.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
    portTextField.setPosition(sf::Vector2f(portRect.getPosition().x - portRect.getSize().x/2 + 5, portRect.getPosition().y));


    // Networking Stuff
    sf::UdpSocket socket;
    sf::Packet sent_packet;
    sf::Packet received_packet;
    sf::IpAddress senderAddress, receipentAddress, addressToSendTo;
    unsigned short senderPort, receipentPort, portToSendTo;

    
    // Window
    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {

            if (event.type == sf::Event::Closed)
                window.close();

            switch (game_state)
            {
                // GAME CASE START
            case (GAME): 
                if (event.type == sf::Event::KeyReleased)
                {
                    switch (event.key.code)
                    {
                    case(sf::Keyboard::Space):
                        isServing = false;
                        if (playerServing == 0)
                        {
                            ball.setVelocity(sf::Vector2f(1, 0), ballVelocity);
                        }
                        if (playerServing == 1)
                        {
                            ball.setVelocity(sf::Vector2f(-1, 0), ballVelocity);
                        }
                        break;
                    }
                }
                break;
            case (JOIN_MENU):
                if (event.type == sf::Event::TextEntered)
                {
                    if (event.text.unicode < 128)
                    {
                        if (addressFieldFocused)
                        {
                            addressInput += event.text.unicode;
                            addressTextField.setString(addressInput);
                        }
                        if (portFieldFocused)
                        {
                            portInput += event.text.unicode;
                            portTextField.setString(portInput);
                        }
                    }
                }
            }
            
        }

        // Real-time input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            paddleManager.movePaddle(playerNum, 1);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            paddleManager.movePaddle(playerNum, -1);
        }

        // Game Logic
        while ((std::chrono::steady_clock::now() - begin).count() >= timePerTick)
        {

            // Game Variables
            float paddleWidth = paddleManager.paddle[0].getSize().x;
            float paddleHeight = paddleManager.paddle[0].getSize().y;
            sf::Vector2i localPosition = sf::Mouse::getPosition(window);
            float paddle1PosX;
            float paddle1PosY;
            float paddle2PosX;
            float paddle2PosY;
            float ballPosX;
            float ballPosY;
            float ballVelX;
            float ballVelY;

            // Main Menu Variables
            float hostRectXPos;
            float hostRectYPos;
            float joinRectXPos;
            float joinRectYPos;

            // Host Menu Variables
            float hostMenuXPos;
            float hostMenuYPos;

            // Join Menu Variables
            float joinMenuXPos;
            float joinMenuYPos;

            switch (game_state)
            {

            case (GAME):
                paddleManager.update_players();
                if (playerNum == 0)
                {
					paddle1PosX = paddleManager.paddle[0].getPosition().x;
					paddle1PosY = paddleManager.paddle[0].getPosition().y;
                    ballPosX = ball.getPosition().x;
                    ballPosY = ball.getPosition().y;
                    sent_packet << &paddle1PosX << &paddle1PosY << &ballPosX << &ballPosY;

					if (socket.send(sent_packet, addressToSendTo, portToSendTo) != sf::Socket::Done)
					{
						printf("Unable to send packets");
					}
					if (socket.receive(received_packet, addressToSendTo, portToSendTo) != sf::Socket::Done)
					{
						printf("Unable to receive packets");
					}

                    ball.update_ball(isServing);
					ballVelX = ball.getVelocity().x;
					ballVelY = ball.getVelocity().y;
                    received_packet >> paddle2PosX >> paddle2PosY;
					paddleManager.positions[1] = sf::Vector2f(paddle2PosX, paddle2PosY);
                }
                else if (playerNum == 1)
                {

					paddle2PosX = paddleManager.paddle[1].getPosition().x;
					paddle2PosY = paddleManager.paddle[1].getPosition().y;
                    sent_packet << paddle2PosX << paddle2PosY;

					if (socket.send(sent_packet, addressToSendTo, portToSendTo) != sf::Socket::Done)
					{
						printf("Unable to send packets");
					}
					if (socket.receive(received_packet, addressToSendTo, portToSendTo) != sf::Socket::Done)
					{
						printf("Unable to receive packets");
					}
                    if (received_packet >> paddle1PosX >> paddle1PosY >> ballPosX >> ballPosY)
                    {
                        printf("Data Extracted: Paddle1: (%f %f), Ball: (%f %f)\n", paddle1PosX, paddle1PosY);
                    }

                    paddleManager.positions[0].x = paddle1PosX;
                    paddleManager.positions[0].y = paddle1PosY;
                    ball.setPosition(sf::Vector2f(ballPosX, ballPosY));
					
                }

                if (isServing && playerNum == 0)
                {
                    if (playerServing == 0)
                    {
                        
                        ballPosX = paddle1PosX + paddleWidth + globalConsts::ballRadius;
                        ballPosY = paddle1PosY + (paddleHeight / 2);
                    }
                    else if (playerServing == 1)
                    {
                        ballPosX = paddle2PosX - globalConsts::ballRadius;
                        ballPosY = paddle2PosY + (paddleHeight / 2);
                    }
                    ball.setPosition(sf::Vector2f(ballPosX, ballPosY));
                }

                if (playerNum == 0)
                {
                    if (ballPosX < paddle1PosX + paddleWidth + globalConsts::ballRadius)
                    {
                        if (ballPosY < paddle1PosY + paddleHeight && ballPosY >= paddle1PosY + (2 * (paddleHeight / 3)))
                        {
                            ball.setVelocity(sf::Vector2f(1, 1), ballVelocity);
                        }
                        else if (ballPosY < paddle1PosY + (2 * (paddleHeight / 3)) && ballPosY >= paddle1PosY + (paddleHeight / 3))
                        {
                            ball.setVelocity(sf::Vector2f(1, 0), ballVelocity);
                        }
                        else if (ballPosY < paddle1PosY + (paddleHeight / 3) && ballPosY >= paddle1PosY)
                        {
                            ball.setVelocity(sf::Vector2f(1, -1), ballVelocity);
                        }
                    }
                    if (ballPosX > paddle2PosX - globalConsts::ballRadius)
                    {
                        if (ballPosY < paddle2PosY + paddleHeight && ballPosY >= paddle2PosY + (2 * (paddleHeight / 3)))
                        {
                            ball.setVelocity(sf::Vector2f(-1, 1), ballVelocity);
                        }
                        else if (ballPosY < paddle2PosY + (2 * (paddleHeight / 3)) && ballPosY >= paddle2PosY + (paddleHeight / 3))
                        {
                            ball.setVelocity(sf::Vector2f(-1, 0), ballVelocity);
                        }
                        else if (ballPosY < paddle2PosY + (paddleHeight / 3) && ballPosY >= paddle2PosY)
                        {
                            ball.setVelocity(sf::Vector2f(-1, -1), ballVelocity);
                        }
                    }
                    if (ballPosY > window.getSize().y - globalConsts::windowBufferSize || ballPosY + globalConsts::ballRadius < globalConsts::windowBufferSize)
                    {
                        ball.setVelocity(sf::Vector2f(ballVelX / abs(ballVelX), -ballVelY / abs(ballVelY)), ballVelocity);
                    }
                    if (ballPosX + globalConsts::ballRadius < globalConsts::windowBufferSize)
                    {
                        score[1]++;
                        isServing = true;
                        playerServing = 0;
                    }
                    if (ballPosX > window.getSize().x - globalConsts::windowBufferSize)
                    {
                        score[0]++;
                        isServing = true;
                        playerServing = 1;
                    }
                }


                break;

                // MAIN_MENU CASE START
            case (MAIN_MENU):
                hostRectXPos = hostRect.getPosition().x;
                hostRectYPos = hostRect.getPosition().y;
                joinRectXPos = joinRect.getPosition().x;
                joinRectYPos = joinRect.getPosition().y;

                // if mouse is within host button boundary
                if (isWithinButtonBounds(localPosition, hostRect.getPosition(), buttonSizeW, buttonSizeH))
                {
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                    {
                        if (socket.bind(54000) != sf::Socket::Done)
                        {
                            printf("Unable to bind to port 54000");
                        }
                        game_state = HOST_MENU;
                    }
                    hostRect.setFillColor(sf::Color::Magenta);
                }
                else
                {
                    hostRect.setFillColor(sf::Color::Green);
                }

                // if mouse is within join button boundary
                if (isWithinButtonBounds(localPosition, joinRect.getPosition(), buttonSizeW, buttonSizeH))
                {
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                    {
                        game_state = JOIN_MENU;
                    }
                    joinRect.setFillColor(sf::Color::Magenta);
                }
                else
                {
                    joinRect.setFillColor(sf::Color::Cyan);
                }


                break;
                // MAIN_MENU CASE END
            case (HOST_MENU):
                // Host Menu
                if (waitingToConnect)
                {
                    int join_key;
                    socket.receive(received_packet, senderAddress, senderPort);
                    received_packet >> join_key;

                    if (join_key == 5)
                    {
                        waitingToConnect = false;
                        addressToSendTo = senderAddress;
                        portToSendTo = senderPort;
                    }
                }
                else
                {
                    playerNum = 0;
                    game_state = GAME;
                }

                break;
            case (JOIN_MENU):
                // Join menu
                if (waitingToConnect)
                {
                    if (dataEntered)
                    {
                        std::string portString = portTextField.getString();
                        /*addressToSendTo = sf::IpAddress(addressTextField.getString());
                        portToSendTo = std::stoi(portString);*/
                        addressToSendTo = sf::IpAddress("76.86.109.115");
                        portToSendTo = 54000;
                        sent_packet << 5;
                        if (socket.send(sent_packet, addressToSendTo, portToSendTo) != sf::Socket::Done)
                        {
                            printf("ERROR ESTABLISHING CONNECTION");
                        }
                        waitingToConnect = false;
                    }

                    if (isWithinButtonBounds(localPosition, addressRect.getPosition(), textFieldSizeW, textFieldSizeH))
                    {
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                        {
                            addressFieldFocused = true;
                            portFieldFocused = false;
                            addressRect.setOutlineColor(sf::Color::Green);
                            portRect.setOutlineColor(sf::Color::White);
                        }
                    }

                    if (isWithinButtonBounds(localPosition, portRect.getPosition(), textFieldSizeW, textFieldSizeH))
                    {
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                        {
                            addressFieldFocused = false;
                            portFieldFocused = true;
                            addressRect.setOutlineColor(sf::Color::White);
                            portRect.setOutlineColor(sf::Color::Green);
                        }
                    }

                    if (isWithinButtonBounds(localPosition, connectRect.getPosition(), buttonSizeH, buttonSizeW))
                    {
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                        {
                            dataEntered = true;
                            if (socket.bind(sf::UdpSocket::AnyPort) != sf::Socket::Done)
                            {
                                printf("Unable to bind to port");
                            }
                        }
                        connectRect.setFillColor(sf::Color::Magenta);
                    }
                }
                else
                {
                    playerNum = 1;
                    game_state = GAME;
                }
                break;
            }
            begin = std::chrono::steady_clock::now();
        }

        // Rendering
        switch (game_state)
        {
        case (GAME):
            // Game
            window.clear();
            // TODO: DRAW
            update_score(score1, score2, score);
            window.draw(score1);
            window.draw(score2);
            // window.draw(center_line);
            window.draw(paddleManager.paddle[0]);
            window.draw(paddleManager.paddle[1]);
            window.draw(ball.getBall());

            window.display();

            break;
        case (MAIN_MENU):
            // Main Menu

            window.clear();
            // TODO: DRAW
            window.draw(hostRect);
            window.draw(joinRect);
            window.draw(hostText);
            window.draw(joinText);
            window.display();

            break;
        case (HOST_MENU):
            // Host Menu

            break;
        case (JOIN_MENU):
            // Non-host Menu

            window.clear();
            window.draw(addressRect);
            window.draw(addressTextField);
            window.draw(portRect);
            window.draw(portTextField);
            window.draw(connectRect);
            window.draw(connectText);
            window.display();

            break;
        }

    }
}

bool isWithinButtonBounds(sf::Vector2i mousePos, sf::Vector2f buttonPosition, float buttonW, float buttonH)
{
    float buttonPosX = buttonPosition.x;
    float buttonPosY = buttonPosition.y;

    return (mousePos.x >= buttonPosX - buttonW / 2 &&
        mousePos.x <= buttonPosX + buttonW / 2) &&
        (mousePos.y >= buttonPosY - buttonH / 2 &&
            mousePos.y <= buttonPosY + buttonH / 2);
}

void update_score(sf::Text& score1, sf::Text& score2, int score[2])
{
    score1.setString(std::to_string(score[0]));
    score2.setString(std::to_string(score[1]));
}