#include <SFML/Graphics.hpp>
#include <iostream>
#include <SFML/Network.hpp>
#include <chrono>
#include "PaddleManager.h"
#include "Ball.h"
#include "GlobalConstants.h"


void update_score(sf::Text& score1, sf::Text& score2, int score[2]);
sf::Packet& operator <<(sf::Packet& packet, const sf::RectangleShape& paddle);
sf::Packet& operator >>(sf::Packet& packet, sf::RectangleShape& paddle);
sf::Packet& operator >>(sf::Packet& packet, Ball& ball);
sf::Packet& operator <<(sf::Packet& packet, Ball& ball);


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
    int score[2] = { 0, 0 };
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
    
    // Window
    while (window.isOpen())
    {
        if (playerNum == 0)
        {
            sent_packet << paddleManager.paddle[0] << ball << score[0] << score[1];
            socket.send(sent_packet, addressToSendTo, portToSendTo);
            socket.receive(received_packet, addressToSendTo, portToSendTo);
            received_packet >> paddleManager.paddle[1];
        }
        else if (playerNum == 1)
        {
            sent_packet << paddleManager.paddle[1];
            socket.send(sent_packet, addressToSendTo, portToSendTo);
            socket.receive(received_packet, addressToSendTo, portToSendTo);
            received_packet >> paddleManager.paddle[0] >> ball >> score[0] >> score[1];
        }

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
            paddleManager.movePaddle(playerNum, 1);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            paddleManager.movePaddle(playerNum, -1);
        }

        // Game Logic
        while ((std::chrono::steady_clock::now() - begin).count() >= timePerTick)
        {
            ball.update_ball(isServing);
            // Game Variables
            float paddleWidth = paddleManager.paddle[0].getSize().x;
            float paddleHeight = paddleManager.paddle[0].getSize().y;
            float paddle1PosX = paddleManager.paddle[0].getPosition().x;
            float paddle1PosY = paddleManager.paddle[0].getPosition().y;
            float paddle2PosX = paddleManager.paddle[1].getPosition().x;
            float paddle2PosY = paddleManager.paddle[1].getPosition().y;
            float ballPosX = ball.getPosition().x;
            float ballPosY = ball.getPosition().y;
            float ballVelX = ball.getVelocity().x;
            float ballVelY = ball.getVelocity().y;

            paddleManager.update_players();

            

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
                if (ballPosY > windowHeight - globalConsts::windowBufferSize) 
                {
                    ball.setVelocity(sf::Vector2f(ballVelX / abs(ballVelX), -ballVelY / abs(ballVelY)), ballVelocity);
                }
                if (ballPosY + globalConsts::ballRadius < globalConsts::windowBufferSize)
                {
                    ball.setPosition(sf::Vector2f(ballPosX, globalConsts::windowBufferSize + globalConsts::ballRadius + 5));
                    ball.setVelocity(sf::Vector2f(ballVelX / abs(ballVelX), -ballVelY / abs(ballVelY)), ballVelocity);
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

// paddle packet operators
sf::Packet& operator <<(sf::Packet& packet, const sf::RectangleShape& paddle)
{
    return packet << paddle.getPosition().x << paddle.getPosition().y;
}
sf::Packet& operator >>(sf::Packet& packet, sf::RectangleShape& paddle)
{
    float paddleX, paddleY;
    packet >> paddleX >> paddleY;
    paddle.setPosition(paddleX, paddleY);
    return packet;
}

// Ball packet operators
sf::Packet& operator <<(sf::Packet& packet, Ball& ball)
{
    return packet << ball.getPosition().x << ball.getPosition().y;
}

sf::Packet& operator >>(sf::Packet& packet, Ball& ball)
{
    float ballX, ballY;
    packet >> ballX >> ballY;
    ball.setPosition(sf::Vector2f(ballX, ballY));
    return packet;
}

void update_score(sf::Text& score1, sf::Text& score2, int score[2])
{
    score1.setString(std::to_string(score[0]));
    score2.setString(std::to_string(score[1]));
}