#pragma once

#include "GlobalConstants.h"
#include "PaddleManager.h"
#include <random>

struct Ball
{
	sf::Vector2f position;
	Ball(float ballRadius, sf::Vector2f position);
	Ball(float ballRadius, float x, float y);
	void update_ball(bool beingServed);
	
	// Getters & Setters
	sf::Vector2f getPosition();
	void setPosition(sf::Vector2f position);

	sf::Vector2f getVelocity();
	void setVelocity(sf::Vector2f velocity, float multiplier = 1);

	sf::CircleShape getBall();


private:
	sf::CircleShape ball;
	sf::Vector2f velocity;
	float ballRadius;
};