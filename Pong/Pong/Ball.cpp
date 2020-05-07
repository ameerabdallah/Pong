#include "Ball.h"

Ball::Ball(float ballRadius, sf::Vector2f position)
{
	this->ballRadius = ballRadius;
	this->position = position;
	this->velocity = sf::Vector2f(0, 0);

	ball.setPosition(position);
	ball.setRadius(ballRadius);
	ball.setFillColor(sf::Color::Yellow);
}

Ball::Ball(float ballRadius, float x, float y)
{
	this->ballRadius = ballRadius;
	this->position = sf::Vector2f(x, y);
	this->velocity = sf::Vector2f(0, 0);

	ball.setOrigin(ballRadius, ballRadius);
	ball.setPosition(position);
	ball.setRadius(ballRadius);
	ball.setFillColor(sf::Color::Yellow);
}

void Ball::update_ball(bool beingServed)
{
	if (!beingServed)
	{
		position += velocity;
	}
	ball.setPosition(position);
}

sf::Vector2f Ball::getPosition()
{
	return position;
}
void Ball::setPosition(sf::Vector2f position)
{
	this->position = position;
	ball.setPosition(position);
}

sf::Vector2f Ball::getVelocity()
{
	return velocity;
}

void Ball::setVelocity(sf::Vector2f velocity, float multiplier)
{
	float norm = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
	if (velocity.x == 0) velocity.x = (rand() % 1 == 0) ? -1 : 1;
	if (velocity.y == 0) velocity.y = (rand() % 1 == 0) ? -1 : 1;

	if (norm == 0) this->velocity = sf::Vector2f(0, 0);
	else
	this->velocity = sf::Vector2f((velocity.x/norm) * multiplier,	// x component
								(velocity.y/norm) * multiplier);	// y component
}

sf::CircleShape Ball::getBall()
{
	return ball;
}

sf::Packet& operator <<(sf::Packet& packet, const Ball& ball)
{
	return packet << ball.position.x << ball.position.y;
}

sf::Packet& operator >>(sf::Packet& packet, Ball& ball)
{
	return packet >> ball.position.x >> ball.position.y;
}
