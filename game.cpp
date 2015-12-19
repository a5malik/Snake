#include "game.hpp"
#include "stringhelper.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>

const float Game::PlayerSpeed = BLOCK_WIDTH;
const sf::Time Game::TimePerFrame = sf::seconds(1.f / 10.f);

Game::Game()
	: mWindow(sf::VideoMode(WIDTH, WIDTH), "SFML Application", sf::Style::Close)
	, length(3)
	, score(3)
	, mTexture()
	, mFont()
	, mStatisticsText()
	, mStatisticsUpdateTime()
	, mStatisticsNumFrames(0)
	, mIsMovingUp(false)
	, mIsMovingDown(false)
	, mIsMovingRight(false)
	, mIsMovingLeft(false)
	, current_direction(PlayerSpeed, 0)
{
	//if (!mTexture.loadFromFile("Eagle.png"))
	//{
		// Handle loading error
	//}


	//mPlayer.setTexture(mTexture);   
	sf::Vector2f size(BLOCK_WIDTH - BLOCK_WIDTH / 10, BLOCK_WIDTH - BLOCK_WIDTH / 10);
	
	for (int i = 0; i < length; i++)
	{ 
		sf::RectangleShape rect(size);
		rect.setPosition(sf::Vector2f((3 + i)*BLOCK_WIDTH, HEIGHT / 2));
		mPlayer.push_back(rect);
		path.push_front(sf::Vector2f(PlayerSpeed, 0));
	}	
	food = sf::RectangleShape(size);
	food.setFillColor(sf::Color::Red);
	placeFood();
	mFont.loadFromFile("Sansation.ttf");
	mStatisticsText.setFont(mFont);
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(25);
}
		
void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

		 	processEvents();
			if(!update(TimePerFrame))
				return;
		}

		updateStatistics(elapsedTime);
		render();
	}
}

void Game::processEvents()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			handlePlayerInput(event.key.code, true);
			break;

		case sf::Event::KeyReleased:
			handlePlayerInput(event.key.code, false);
			break;

		case sf::Event::Closed:
			mWindow.close();
			exit(0);
			break;
		}
	}
}

bool Game::update(sf::Time elapsedTime)
{
	if (mIsMovingUp)
		current_direction = sf::Vector2f(0, -1*PlayerSpeed);
	else if (mIsMovingDown)
		current_direction = sf::Vector2f(0, PlayerSpeed);
	else if (mIsMovingLeft)
		current_direction = sf::Vector2f(-1*PlayerSpeed, 0);
	else if (mIsMovingRight)
		current_direction = sf::Vector2f(PlayerSpeed, 0);

	path.push_front(current_direction);
	path.pop_back();
	//float time = elapsedTime.asSeconds();
	auto it = path.begin();
    list<sf::RectangleShape>::reverse_iterator rect=mPlayer.rbegin();
	for (; rect != mPlayer.rend();rect++)
	{
		//std::cout << "(" << it->x << ", " << it->y << ")";
		(*rect).move(*it);
		sf::Vector2f pos = (*rect).getPosition();
		if (pos.x >= WIDTH)
			(*rect).setPosition(sf::Vector2f(pos.x - WIDTH, pos.y));
		else if (pos.x < 0)
			(*rect).setPosition(sf::Vector2f(pos.x + WIDTH, pos.y));
		if (pos.y >= HEIGHT)
			(*rect).setPosition(sf::Vector2f(pos.x, pos.y-HEIGHT));
		else if (pos.y < 0)
			(*rect).setPosition(sf::Vector2f(pos.x, pos.y+HEIGHT));
		it++;
	}
	rect = mPlayer.rbegin();
	sf::Vector2f pos = (*rect).getPosition();
	sf::Vector2f food_pos = food.getPosition();
	auto iter = mPlayer.rbegin();
	iter++;
	for (; iter != mPlayer.rend(); iter++)
	{
		sf::Vector2f body_pos = (*iter).getPosition();
		if (pos.x == body_pos.x && pos.y == body_pos.y)
			return false;
	}                     
	if (pos.x == food_pos.x && pos.y == food_pos.y)
	{
		placeFood();
		score++;
		sf::Vector2f size(BLOCK_WIDTH - BLOCK_WIDTH / 10, BLOCK_WIDTH - BLOCK_WIDTH / 10);
		sf::RectangleShape rectangle(size);
		list<sf::RectangleShape>::iterator rec = mPlayer.begin();
		//list<sf::RectangleShape>::iterator rec2 = mPlayer.begin();
		//rec2++;
		sf::Vector2f pos = (*rec).getPosition();       
		auto i = path.rbegin();
		rectangle.setPosition(sf::Vector2f(pos.x - i->x, pos.y - i->y));
		mPlayer.push_front(rectangle);
		path.push_back(sf::Vector2f(i->x, i->y));
	}
	return true;
	//std::cout << std::endl;
}

void Game::render()
{
	mWindow.clear();
	for (const auto &rect :mPlayer)
		mWindow.draw(rect);
	mWindow.draw(food);
	mWindow.draw(mStatisticsText);
	mWindow.display();
}

void Game::updateStatistics(sf::Time elapsedTime)
{
		mStatisticsText.setString(
			 toString(score));	
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
	if (key == sf::Keyboard::W)
		mIsMovingUp = isPressed;
	else if (key == sf::Keyboard::S)
		mIsMovingDown = isPressed;
	else if (key == sf::Keyboard::A)
		mIsMovingLeft = isPressed;
	else if (key == sf::Keyboard::D)
		mIsMovingRight = isPressed;
}

void Game::placeFood()
{
	srand(time(NULL));
	do
	{
		food.setPosition(sf::Vector2f((rand() % 20) * BLOCK_WIDTH, (rand() % 20)* BLOCK_WIDTH));
	} while (!food_snake());
}

bool Game::food_snake()
{
	sf::Vector2f food_pos = food.getPosition();
	for (const auto &rect : mPlayer)
	{
		sf::Vector2f pos = rect.getPosition();
		if (pos.x == food_pos.x && pos.y == food_pos.y)
			return false;
	}
	return true;
}