#pragma once

#include "GlobalConstants.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

struct PaddleManager
{
	// Paddle 
	sf::RectangleShape paddle[2];

	sf::Vector2f positions[2];

	// Functions
	PaddleManager(sf::Vector2u windowSize);
	void update_players();
	void movePaddle(int player, int direction); // UP is direction = 1, DOWN is direction = -1
protected:

private:
	// Constants
	const float velocity = 0.45f;
	sf::Vector2f paddleSize;

	// Window Data
	sf::Vector2u windowSize;

	// Player positions
	

};