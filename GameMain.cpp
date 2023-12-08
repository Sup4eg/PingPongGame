#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <math.h>

#define PI 3.14159265 

using namespace std;


const std::string RESOURCES_PATH = "Resources/";

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGTH = 900;

const float MARGIN = 30.f;

const float PLATFORM_SPEED = 700.f;
const float PLATFORM_WIDTH = 5.f;
const float PLATFORM_HEIGHT = 150.f;

const float BALL_SIZE = 20.f;
const float BALL_INITIAL_SPEED = 600.f; //r - in polar coordinates

const int AI_HARD_COEFFICIENT = 1;
const int FINAL_SCORE = 5;

const std::string GAME_NAME = "Ping pong game!";


inline void initPlatform(sf::RectangleShape& platform, const float& platformX, const float& platformY) {
  platform.setSize(sf::Vector2f(PLATFORM_WIDTH, PLATFORM_HEIGHT));
  platform.setFillColor(sf::Color::White);
  platform.setOrigin(PLATFORM_WIDTH / 2.f, PLATFORM_HEIGHT / 2.f);
  platform.setPosition(platformX, platformY);
}

inline void initBall(sf::CircleShape& ball, const float& ballX, const float& ballY) {
  ball.setRadius(BALL_SIZE / 2.f);
  ball.setFillColor(sf::Color::White);
  ball.setOrigin(BALL_SIZE / 2.f, BALL_SIZE / 2.f);
  ball.setPosition(ballX, ballY);
}

void definePlayerDirection(float& playerY, float& deltaTime) {
  float newPlayerY = 0;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
	newPlayerY = playerY - PLATFORM_SPEED * deltaTime;
	//Check borders with SCREEN_HEIGHT
	if (newPlayerY > (PLATFORM_HEIGHT / 2.f + MARGIN)) {
	  playerY = newPlayerY;
	}
  }
  else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
	newPlayerY = playerY + PLATFORM_SPEED * deltaTime;
	//Check borders with SCREEN_HEIGHT
	if (newPlayerY < (SCREEN_HEIGTH - PLATFORM_HEIGHT / 2.f - MARGIN)) {
	  playerY = newPlayerY;
	}
  }
}

void defineAiDirection(float& aiY, float& ballX, float& ballY, float& deltaTime) {
  float newAiY = 0;
  if (ballX > SCREEN_WIDTH / 2.f) {
	if (aiY < ballY) {
	  newAiY = aiY + (PLATFORM_SPEED / AI_HARD_COEFFICIENT) * deltaTime;
	  if (newAiY < (SCREEN_HEIGTH - PLATFORM_HEIGHT / 2.f - MARGIN)) {
		aiY = newAiY;
	  }
	}
	else if (aiY > ballY) {
	  newAiY = aiY - (PLATFORM_SPEED / AI_HARD_COEFFICIENT) * deltaTime;
	  if (newAiY > (PLATFORM_HEIGHT / 2.f + MARGIN)) {
		aiY = newAiY;
	  }
	}
  }
}

void checkHorizontalWallCollision(float& ballX, float& ballY, int& yv) {
  //collisions with Down window
  if (ballY >= SCREEN_HEIGTH) {
	yv = 1;
  }
  //collision with up window
  else if (ballY <= 0) {
	yv = -1;
  }
}

inline float getRandomAngle() {
  float angleInDegrees = rand() / (float)(RAND_MAX) * 360; //angle 0 - 360
  //tests
  angleInDegrees = 110;
  //return angle in radians
  return angleInDegrees * (PI / 180) * (-1);
}

void checkVerticalWallCollision(float& ballX, float& ballY, float& angle, int& playerNum, int& aiNum, float& ballSpeedUp) {
  //check collision with left vertical window
  if (ballX < MARGIN) {
	++aiNum;
	ballX = SCREEN_WIDTH / 2.f;
	ballY = rand() / (float)(RAND_MAX)*SCREEN_HEIGTH;
	angle = getRandomAngle();
	ballSpeedUp = 0.f;
  }
  //right
  else if (ballX > (SCREEN_WIDTH - MARGIN)) {
	++playerNum;
	ballX = SCREEN_WIDTH / 2.f;
	ballY = rand() / (float)(RAND_MAX)*SCREEN_HEIGTH;
	angle = getRandomAngle();
	ballSpeedUp = 0.f;
  }
}

void checkPlatformCollision(float& platformX, float& platformY, float& ballX, float& ballY, int& xv, float& ballSpeedUp, bool isUserPlatform = true) {
  //collisions with platform
  float platformMinX = platformX - PLATFORM_WIDTH / 2.f;
  float platformMaxX = platformX + PLATFORM_WIDTH / 2.f;


  float platformMinY = platformY + PLATFORM_HEIGHT / 2.f;
  float platformMaxY = platformY - PLATFORM_HEIGHT / 2.f;

  const float x = max(platformMinX, min(ballX, platformMaxX));
  const float y = max(platformMaxY, min(ballY, platformMinY));

  const float pow2Distance = pow((x - ballX), 2) + pow(y - ballY, 2);
  const float radius = BALL_SIZE / 2.f;

  if (pow2Distance < pow(radius, 2)) {
	if (isUserPlatform) {
	  xv = -1;
	}
	else {
	  xv = 1;
	}
	ballSpeedUp += 100.f;
  }
}


int main()
{
  sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGTH), GAME_NAME);

  //Init seed
  int seed = (int)time(nullptr);
  srand(seed);


  //Init player state
  sf::RectangleShape playerPlatform;
  float playerX = MARGIN;
  float playerY = SCREEN_HEIGTH / 2.f;
  initPlatform(playerPlatform, playerX, playerY);

  //Init AI state
  sf::RectangleShape aiPlatform;
  float aiX = SCREEN_WIDTH - MARGIN;
  float aiY = SCREEN_HEIGTH / 2.f;
  initPlatform(aiPlatform, aiX, aiY);


  //Init ball
  sf::CircleShape ballShape;
  float ballX = SCREEN_WIDTH / 2.f;
  float ballY = SCREEN_HEIGTH / 2.f;
  float angle = getRandomAngle();
  float ballSpeedUp = 0.f;
  int xv = 1;
  int yv = 1;
  initBall(ballShape, ballX, ballY);


  //Init table line
  sf::VertexArray line(sf::LinesStrip, 2);
  line[0].position = sf::Vector2f(SCREEN_WIDTH / 2.f, 0);
  line[0].color = sf::Color::White;
  line[1].position = sf::Vector2f(SCREEN_WIDTH / 2.f, SCREEN_HEIGTH);
  line[1].color = sf::Color::White;

  //Init numbers

  sf::Font font;
  if (!font.loadFromFile(RESOURCES_PATH + "Fonts/Roboto-Regular.ttf"))
  {
	return EXIT_FAILURE;
  }

  int playerNum = 0;
  int aiNum = 0;

  sf::Text playerNumText;
  playerNumText.setFont(font);
  playerNumText.setCharacterSize(130);
  playerNumText.setStyle(sf::Text::Bold);
  playerNumText.setFillColor(sf::Color::White);
  playerNumText.setPosition(SCREEN_WIDTH / 4.f, MARGIN);

  sf::Text aiNumText;
  aiNumText.setFont(font);
  aiNumText.setCharacterSize(130);
  aiNumText.setStyle(sf::Text::Bold);
  aiNumText.setFillColor(sf::Color::White);
  aiNumText.setPosition(SCREEN_WIDTH * 3 / 4.f, MARGIN);

  //Init game clock
  sf::Clock gameClock;
  float lastTime = gameClock.getElapsedTime().asSeconds();

  //Waiting when finishing game
  bool isWaiting = false;

  while (window.isOpen())
  {
	//Calculate time delta
	float currentTime = gameClock.getElapsedTime().asSeconds();
	float deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	sf::Event event;
	while (window.pollEvent(event))
	{
	  if (event.type == sf::Event::Closed)
		window.close();
	}

	playerPlatform.setPosition(playerX, playerY);
	aiPlatform.setPosition(aiX, aiY);
	ballShape.setPosition(ballX, ballY);
	playerNumText.setString(to_string(playerNum));
	aiNumText.setString(to_string(aiNum));

	definePlayerDirection(playerY, deltaTime);
	defineAiDirection(aiY, ballX, ballY, deltaTime);

	checkHorizontalWallCollision(ballX, ballY, yv);
	checkVerticalWallCollision(ballX, ballY, angle, playerNum, aiNum, ballSpeedUp);
	checkPlatformCollision(playerX, playerY, ballX, ballY, xv, ballSpeedUp);
	checkPlatformCollision(aiX, aiY, ballX, ballY, xv, ballSpeedUp, false);

	ballX += xv * ((BALL_INITIAL_SPEED + ballSpeedUp) * cos(angle)) * deltaTime;
	ballY += yv * ((BALL_INITIAL_SPEED + ballSpeedUp) * sin(angle)) * deltaTime;

	//finish the game
	if (playerNum == FINAL_SCORE || aiNum == FINAL_SCORE) {
	  window.close();
	  break;
	}



	window.clear();
	window.draw(playerPlatform);
	window.draw(aiPlatform);
	window.draw(ballShape);

	window.draw(line);
	window.draw(playerNumText);
	window.draw(aiNumText);
	window.display();
  }
  return 0;
}
