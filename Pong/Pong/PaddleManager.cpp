#include "PaddleManager.h"

void PaddleManager::update_players()
{
	paddle[0].setPosition(positions[0].x, positions[0].y);
	paddle[1].setPosition(positions[1].x, positions[1].y);
}

PaddleManager::PaddleManager(sf::Vector2u windowSize)
{
	float x1 = 5;							// Paddle 1
	float x2 = (float)windowSize.x - 5;		// Paddle 2
	float y = (float)windowSize.y / 2;		// Paddle 1 & 2 initial y
	
	paddleSize = sf::Vector2f((float)windowSize.x / 50, (float)windowSize.y / 5);

	// Paddle 1
	positions[0] = sf::Vector2f(x1, y - paddleSize.y / 2);
	paddle[0].setSize(paddleSize);
	paddle[0].setPosition(x1, y - paddleSize.y/2);
	paddle[0].setFillColor(sf::Color::Blue);
	paddle[0].setOutlineColor(sf::Color::White);
	paddle[0].setOutlineThickness(1.f);

	// Paddle 2
	positions[1] = sf::Vector2f(x2 - paddleSize.x, y - paddleSize.y / 2);
	paddle[1].setSize(paddleSize);
	paddle[1].setPosition(x2 - paddleSize.x, y - paddleSize.y/2);
	paddle[1].setFillColor(sf::Color::Red);
	paddle[1].setOutlineColor(sf::Color::White);
	paddle[1].setOutlineThickness(1.f);

	this->windowSize = windowSize;
}

void PaddleManager::movePaddle(int player, int direction)
{
	positions[player].y -= velocity*direction;
	if (positions[player].y + paddleSize.y > windowSize.y - globalConsts::windowBufferSize)
	{
		positions[player].y = windowSize.y - globalConsts::windowBufferSize - paddleSize.y;
	}
	if (positions[player].y < globalConsts::windowBufferSize)
	{
		positions[player].y = globalConsts::windowBufferSize;
	}
}

sf::Packet& operator <<(sf::Packet& packet, const sf::RectangleShape& paddle)
{
	float paddlePosX = paddle.getPosition().x;
	float paddlePosY = paddle.getPosition().y;

	return packet << paddlePosX << paddlePosY;
}

sf::Packet& operator >>(sf::Packet& packet, sf::RectangleShape paddle)
{
	float paddlePosX = paddle.getPosition().x;
	float paddlePosY = paddle.getPosition().y;

	return packet >> paddlePosX >> paddlePosY;
}
