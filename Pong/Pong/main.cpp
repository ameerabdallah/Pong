#include <SFML/Graphics.hpp>
#include <iostream>
#include <SFML/Network.hpp>
#include <chrono>
#include "PaddleManager.h"
#include "Ball.h"
#include "GlobalConstants.h"


void update_score(sf::Text& score1, sf::Text& score2, sf::Int8 score[2]);

int main()
{
    sf::RenderWindow window(sf::VideoMode(1024, 512), "PONG");

    srand(time(NULL));
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // Constants
    const float playerVelocity = 0.5f;
    const float ballVelocity = 5.f;
    const long nanoSecondsPerMillisecond = 1000000;
    const long timePerTick = 5 * nanoSecondsPerMillisecond;
    const unsigned int windowWidth = window.getSize().x;
    const unsigned int windowHeight = window.getSize().y;


    // Game font
    sf::Font font;
    font.loadFromFile("font.ttf");

    
    // Text Fields
    sf::Text score1;
    sf::Text score2;

    // Networking Stuff
    sf::UdpSocket socket;
    sf::Packet sent_packet;
    sf::Packet received_packet;
    sf::IpAddress addressToSendTo;
    unsigned short portToSendTo = 5400, myPort;

    // Views and window
    sf::View game_view;

    // Game Objects
    Ball ball(globalConsts::ballRadius, windowWidth/2, windowHeight/2);
    PaddleManager paddleManager(window.getSize());

    // Initializations
    float waitingToConnect = true;
    bool isServing = true;
    int playerNum = 0;
    int playerServing = 0;
    sf::Int8 score[2] = { 0, 0 };
    std::string userInput;

    std::cout << "Press (j) to Join a game and press (h) to host one\n";
    std::cin >> userInput;
    if (userInput == "j")
    {
        playerNum = 1;
        sf::Int8 key = 10;
        sent_packet << key;

        if (socket.bind(54001) != sf::Socket::Done)
        {
            std::cout << "Failed to bind port: " << socket.getLocalPort() << "\n";
        }
        myPort = socket.getLocalPort();

        std::cout << "Please enter the IP Address that you would like to connect to\n";
        std::cin >> addressToSendTo;

        std::cout << "Please enter the port number:\n";
        std::cin >> portToSendTo;

        std::cout << "Port to send to: " << portToSendTo << std::endl; 

        socket.send(sent_packet, addressToSendTo, portToSendTo);
    }
    else if (userInput == "h")
    { 
        playerNum = 0;
        sf::Int8 key = -1;
        if (socket.bind(54000) != sf::Socket::Done)
        {
            std::cout << "Failed to bind port: " << socket.getLocalPort() << "\n";
        }
        myPort = socket.getLocalPort();
        std::cout << "Port: " << myPort << "\nWaiting for someone to join...";
        while (key != 10)
        {
            socket.receive(received_packet, addressToSendTo, portToSendTo);
            received_packet >> key;
            std::cout << "Joining!\n";
            std::cout << key << std::endl;
        }

    }

    float paddleWidth = paddleManager.paddle[0].getSize().x;
    float paddleHeight = paddleManager.paddle[0].getSize().y;
    
    // Window
    while (window.isOpen())
    {
        float ballPosX = ball.getPosition().x;
        float ballPosY = ball.getPosition().y;
        float ballVelX = ball.getVelocity().x;
        float ballVelY = ball.getVelocity().y;


        sf::Event event;
        sf::Vector2i localPosition = sf::Mouse::getPosition(window);

        while (window.pollEvent(event))
        {

            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyReleased)
            {
                switch (event.key.code)
                {
                case(sf::Keyboard::Space):
                    isServing = false;
                    if (playerServing == 0 && playerNum == 0)
                    {
                        ball.setVelocity(sf::Vector2f(1, 0), ballVelocity);
                    }
                    if (playerServing == 1 && playerNum == 1)
                    {
                        ball.setVelocity(sf::Vector2f(-1, 0), ballVelocity);
                    }
                    break;
                }
            }
        }

        // Real-time input
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            paddleManager.positions[playerNum].y -= playerVelocity;
            if (paddleManager.positions[playerNum].y < globalConsts::windowBufferSize)
            {
                paddleManager.positions[playerNum].y = globalConsts::windowBufferSize;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            paddleManager.positions[playerNum].y -= playerVelocity * -1;
            if (paddleManager.positions[playerNum].y + paddleHeight > windowHeight - globalConsts::windowBufferSize)
            {
                paddleManager.positions[playerNum].y = windowHeight - globalConsts::windowBufferSize - paddleHeight;
            }
            paddleManager.movePaddle(playerNum, -1);
        }

        // Game Logic
        while ((std::chrono::steady_clock::now() - begin).count() >= timePerTick)
        {

            // Game Variables
            ballPosX = ball.getPosition().x;
            ballPosY = ball.getPosition().y;

            if (isServing && playerNum == 0)
            {
                if (playerServing == 0)
                {

                    ballPosX = paddleManager.positions[0].x + paddleWidth + globalConsts::ballRadius;
                    ballPosY = paddleManager.positions[0].y + (paddleHeight / 2);
                }
                else if (playerServing == 1)
                {
                    ballPosX = paddleManager.positions[1].x - globalConsts::ballRadius;
                    ballPosY = paddleManager.positions[1].y + (paddleHeight / 2);
                }
                ball.setPosition(sf::Vector2f(ballPosX, ballPosY));
            }

            if (playerNum == 0)
            {
                ball.update_ball(isServing);
                if (ballPosX < paddleManager.positions[0].x + paddleWidth + globalConsts::ballRadius)
                {
                    if (ballPosY < paddleManager.positions[0].y + paddleHeight && ballPosY >= paddleManager.positions[0].y + (2 * (paddleHeight / 3)))
                    {
                        ball.setVelocity(sf::Vector2f(1, 1), ballVelocity);
                    }
                    else if (ballPosY < paddleManager.positions[0].y + (2 * (paddleHeight / 3)) && ballPosY >= paddleManager.positions[0].y + (paddleHeight / 3))
                    {
                        ball.setVelocity(sf::Vector2f(1, 0), ballVelocity);
                    }
                    else if (ballPosY < paddleManager.positions[0].y + (paddleHeight / 3) && ballPosY >= paddleManager.positions[0].y)
                    {
                        ball.setVelocity(sf::Vector2f(1, -1), ballVelocity);
                    }
                }
                if (ballPosX > paddleManager.positions[1].x - globalConsts::ballRadius)
                {
                    if (ballPosY < paddleManager.positions[1].y + paddleHeight && ballPosY >= paddleManager.positions[1].y + (2 * (paddleHeight / 3)))
                    {
                        ball.setVelocity(sf::Vector2f(-1, 1), ballVelocity);
                    }
                    else if (ballPosY < paddleManager.positions[1].y + (2 * (paddleHeight / 3)) && ballPosY >= paddleManager.positions[1].y + (paddleHeight / 3))
                    {
                        ball.setVelocity(sf::Vector2f(-1, 0), ballVelocity);
                    }
                    else if (ballPosY < paddleManager.positions[1].y + (paddleHeight / 3) && ballPosY >= paddleManager.positions[1].y)
                    {
                        ball.setVelocity(sf::Vector2f(-1, -1), ballVelocity);
                    }
                }
                if (ballPosY > windowHeight - globalConsts::windowBufferSize) 
                {
                    ball.setVelocity(sf::Vector2f(ball.getVelocity().x / abs(ball.getVelocity().x), -ball.getVelocity().y / abs(ball.getVelocity().y)), ballVelocity);
                }
                if (ballPosY + globalConsts::ballRadius < globalConsts::windowBufferSize)
                {
                    ball.setPosition(sf::Vector2f(ballPosX, globalConsts::windowBufferSize + globalConsts::ballRadius + 5));
                    ball.setVelocity(sf::Vector2f(ball.getVelocity().x / abs(ball.getVelocity().x), -ball.getVelocity().y / abs(ball.getVelocity().y)), ballVelocity);
                }
                if (ballPosX + globalConsts::ballRadius < globalConsts::windowBufferSize)
                {
                    score[1]++;
                    isServing = true;
                    playerServing = 0;
                }
                if (ballPosX > windowWidth - globalConsts::windowBufferSize)
                {
                    score[0]++;
                    isServing = true;
                    playerServing = 1;
                }
            }

            // Send & Receive Packets
            // Send & Receive Packets
            if (playerNum == 0)
            {
                // Send packet
                sent_packet << paddleManager.positions[0].x << paddleManager.positions[0].y << ballPosX << ballPosY << score[0] << score[1];
                socket.send(sent_packet, addressToSendTo, portToSendTo);

                // Receive Packet
                socket.receive(received_packet, addressToSendTo, portToSendTo);
                received_packet >> paddleManager.positions[1].x >> paddleManager.positions[1].y;
            }
            else if (playerNum == 1)
            {
                // Send packet
                sent_packet << paddleManager.positions[1].x << paddleManager.positions[1].y;
                socket.send(sent_packet, addressToSendTo, portToSendTo);

                // Receive Packet
                socket.receive(received_packet, addressToSendTo, portToSendTo);
                received_packet >> paddleManager.positions[1].x >> paddleManager.positions[1].y >> ballPosX >> ballPosY >> score[0] >> score[1];

                ball.setPosition(sf::Vector2f(ballPosX, ballPosY));
            }

            paddleManager.update_players();

            begin = std::chrono::steady_clock::now();
        }

        // Rendering
        window.clear();
        update_score(score1, score2, score);
        window.draw(score1);
        window.draw(score2);
        // window.draw(center_line);
        window.draw(paddleManager.paddle[0]);
        window.draw(paddleManager.paddle[1]);
        window.draw(ball.getBall());
        window.display();


    }
}

void update_score(sf::Text& score1, sf::Text& score2, sf::Int8 score[2])
{
    score1.setString(std::to_string(score[0]));
    score2.setString(std::to_string(score[1]));
}