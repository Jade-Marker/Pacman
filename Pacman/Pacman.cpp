#include "Pacman.h"

#include <sstream>

//todo
//https://gameinternals.com/understanding-pac-man-ghost-behavior

//Make a base struct which the player and enemy are inherited from as they share a lot of elements

//Clean up Enemy.h

//Get eating ghosts to add to the score

//Finish destructor

//move constants for left and right screen limit to another file so that it isn't repeated in pacman & enemy

//Redraw sprites

//Music when starting level

//SFX when eating pellet

//Power pellets need to be animated

//Need to add cherries

//Animation when dying

//Start screen with buttons

//Link to portfolio page about it with button on start screen
//ShellExecute(0, 0, L"https://www.google.com", 0, 0, SW_SHOW);

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPacmanPosOffset(20.0f), _cPacmanFrameTime(250), _cLevelEndDelay(1000), _cLevelStartDelay(2000), _cPoweredUpTime(7000), _cPelletValue(10), _cPowerPelletValue(20), _cEnemyValue(50)
{
	_pacman = new Player();
	_pacman->direction = RIGHT;
	_pacman->currentFrameTime = 0;
	_pacman->frame = 0;
	_pacman->score = 0;

	_pauseMenu = new Menu();
	_pauseMenu->inUse = false;
	_pauseMenu->keyDown = false;
	_pauseMenu->interactKey = Input::Keys::P;

	_startMenu = new Menu();
	_startMenu->inUse = true;
	_startMenu->interactKey = Input::Keys::SPACE;

	_levelEnd = false;

	_poweredUp = false;
	_powerTimer = 0;

	_level = 1;
	_pelletsCollected = 0;

	LoadMaze(_maze, _mazeTemplate);
	_noPelletsAvailable = GetNoOfPellets(_maze);

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1920, 1080, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	delete _pacman->texture;
	delete _pacman->sourceRect;
}

void Pacman::LoadContent()
{
	//Load background texture + setup background rect
	_mazeTileset = new Texture2D();
	_mazeTileset->Load("Textures/backgroundTiles.png", false);
	_mazeTileRect = new Rect(0.0f, 0.0f, 32, 32);
	_backgroundPos = new Vector2();

	// Load Pacman
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/Pacman.png", false);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 64, 64);
	_pacman->position = new Vector2(Graphics::GetViewportWidth() / 2.0f - _pacman->sourceRect->Width / 2.0f, 17 * _mazeTileRect->Width - _pacman->sourceRect->Height / 4.0f);

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);
	_pacman->scoreOutputPos = new Vector2(10.0f, 45.0f);

	//Set Menu Parameters
	_pauseMenu->texture = new Texture2D();
	_pauseMenu->texture->Load("Textures/Transparency.png", false);
	_pauseMenu->rect = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_pauseMenu->stringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	//Start Menu Parameters
	_startMenu->texture = new Texture2D();
	_startMenu->texture->Load("Textures/Start.png", false);

	//load overlay
	_overlay = new Texture2D();
	_overlay->Load("Textures/Overlay.png", false);
	_overlayRect = new Rect(0.0f, -40.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());

	CreateAndInitGhosts();
}

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();

	CheckStart(keyboardState, _startMenu->interactKey);

	if (!_startMenu->inUse)
	{
		CheckPaused(keyboardState, _pauseMenu->interactKey);

		if (!_pauseMenu->inUse) {
			if (!_delay) {
				Input(elapsedTime, keyboardState);
				PelletCollisionCheck();
				LevelWinCheck();
				UpdatePacman(elapsedTime);

				//+4 & +1 so that the tile for screen wrapping is not visible and so that pacman fully dissapears before wrapping
				ScreenWrapCheck(Graphics::GetViewportWidth() / 2.0f - ((_mazeWidth + 4) * _mazeTileRect->Width) / 2.0f,
					Graphics::GetViewportWidth() / 2.0f + ((_mazeWidth + 1) * _mazeTileRect->Width) / 2.0f);

				for (int i = 0; i < _enemyCount; i++)
				{
					bool colidedWithGhost = false;
					bool ghostInChaseOrScatter = false;

					_enemies[i]->Update(elapsedTime, _level, _pacman->direction, _pacman->position->X, _pacman->position->Y, _enemies[0], _poweredUp, colidedWithGhost, ghostInChaseOrScatter);
					if (colidedWithGhost)
					{
						if (!ghostInChaseOrScatter)
						{
							_enemies[i]->GhostHasBeenEaten();
							//_pacman->score += _cEnemyValue;
						}
						else
						{
							PacmanDeath();
							break;
						}
					}


				}

				_powerTimer -= elapsedTime;
				if (_powerTimer <= 0)
				{
					_poweredUp = false;
					_powerTimer = 0;
				}

			}
			DelayCountdown(elapsedTime);
		}
	}

}

void Pacman::Draw(int elapsedTime)
{
	std::stringstream stream;
	int outputX, outputY;

	outputX = CalculateMazeX(_pacman->position->X, _pacman->sourceRect->Width, _mazeTileRect->Width);
	outputY = CalculateMazeY(_pacman->position->Y, _pacman->sourceRect->Height, _mazeTileRect->Height);

	//stream << "Currently on: " << _maze[outputY][outputX];
	stream << "Pacman X: " << outputX << " Y: " << outputY;
	//stream << "Pacman X: " << outputX << " Y: " << outputY << "Collision: " << CollisionCheck(_pacmanPosition->X, _pacmanPosition->Y);
	//stream << "Pacman X: " << _pacmanPosition->X << " Y: " << _pacmanPosition->Y;
	//stream << "Pacman X: " << _pacmanPosition->X << " warp X: " << Graphics::GetViewportWidth() / 2.0f + (27 * 32) / 2.0f;
	stream << "  Pellets available: " << _noPelletsAvailable << "  Pellets collected: " << _pelletsCollected;
	stream << "  Level: " << _level;
	stream << "  Delay: " << _delayInMilli;
	stream << "  PoweredUp: " << _poweredUp;
	stream << "  PowerTimer: " << _powerTimer;

	SpriteBatch::BeginDraw(); // Starts Drawing

	if (_startMenu->inUse)
	{
		SpriteBatch::Draw(_startMenu->texture, _pauseMenu->rect, nullptr);
	}
	else {
		for (int i = 0; i < _mazeHeight; i++)
		{
			_backgroundPos->Y = _mazeTileRect->Height * i;
			for (int j = 0; j < _mazeWidth; j++)
			{
				//https://www.spriters-resource.com/arcade/pacman/sheet/73389/
				_backgroundPos->X = _mazeTileRect->Width * j + (Graphics::GetViewportWidth() / 2 - (_mazeWidth * _mazeTileRect->Width) / 2);
				switch (_maze[i][j])
				{
				//have separate cases for tiles which need to be animated
				case PELLET:
					_mazeTileRect->X = _mazeTileRect->Width * _maze[i][j];
					_mazeTileRect->Y = 0.0f;
					break;
				default:
					_mazeTileRect->X = _mazeTileRect->Width * _maze[i][j];
					_mazeTileRect->Y = 0.0f;
					break;
				}

				SpriteBatch::Draw(_mazeTileset, _backgroundPos, _mazeTileRect);
			}
		}

		SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect); // Draws Pacman

		//Draw enemies
		for (int i = 0; i < _enemyCount; i++)
		{
			SpriteBatch::Draw(_enemies[i]->GetTexturePointer(), _enemies[i]->GetVectorPointer(), _enemies[i]->GetRectPointer());
		}

		SpriteBatch::Draw(_overlay, _overlayRect, nullptr);

		// Draws String
		SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Red);

		std::stringstream scoreOutput;
		scoreOutput << "Score: " << _pacman->score;
		SpriteBatch::DrawString(scoreOutput.str().c_str(),_pacman->scoreOutputPos, Color::Red);

		if (_pauseMenu->inUse)
		{
			std::stringstream menuStream;
			menuStream << "PAUSED!";

			SpriteBatch::Draw(_pauseMenu->texture, _pauseMenu->rect, nullptr);
			SpriteBatch::DrawString(menuStream.str().c_str(), _pauseMenu->stringPosition, Color::Red);
		}
	}
	SpriteBatch::EndDraw(); // Ends Drawing
}

/// <summary> Handles player input and moves pacman based on input </summary>
void Pacman::Input(int elapsedTime, Input::KeyboardState* state)
{
	//Horizontal movement
	if (state->IsKeyDown(Input::Keys::D))
	{
		_pacman->direction = RIGHT;
		if (CollisionCheck(_pacman->position->X + _cPacmanSpeed * elapsedTime, _pacman->position->Y, RIGHT))
			_pacman->position->X += _cPacmanSpeed * elapsedTime;
	}
	else if (state->IsKeyDown(Input::Keys::A))
	{
		_pacman->direction = LEFT;
		if (CollisionCheck(_pacman->position->X - _cPacmanSpeed * elapsedTime, _pacman->position->Y, LEFT))
			_pacman->position->X -= _cPacmanSpeed * elapsedTime;
	}
	//Vertical movement
	else if (state->IsKeyDown(Input::Keys::W))
	{
		_pacman->direction = UP;
		if (CollisionCheck(_pacman->position->X, _pacman->position->Y - _cPacmanSpeed * elapsedTime, UP))
			_pacman->position->Y -= _cPacmanSpeed * elapsedTime;
	}
	else if (state->IsKeyDown(Input::Keys::S))
	{
		_pacman->direction = DOWN;
		if (CollisionCheck(_pacman->position->X, _pacman->position->Y + _cPacmanSpeed * elapsedTime, DOWN))
			_pacman->position->Y += _cPacmanSpeed * elapsedTime;
	}
}

/// <summary> Checks if the player has pressed the pause key and then pauses or unpauses</summary>
void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	if (state->IsKeyDown(pauseKey) && !_pauseMenu->keyDown)
	{
		_pauseMenu->keyDown = true;
		_pauseMenu->inUse = !_pauseMenu->inUse;
	}

	if (state->IsKeyUp(pauseKey))
		_pauseMenu->keyDown = false;
}

/// <summary> Checks if the player has pressed the start key </summary>
void Pacman::CheckStart(Input::KeyboardState* state, Input::Keys startKey)
{
	if (state->IsKeyDown(startKey) && _startMenu->inUse)
	{
		_startMenu->inUse = false;
		_delayInMilli = _cLevelStartDelay;
		_delay = true;
	}
}

/// <summary> Update rect for pacman based on frame and direction </summary>
void Pacman::UpdatePacman(int elapsedTime)
{
	_pacman->currentFrameTime += elapsedTime;
	if (_pacman->currentFrameTime > _cPacmanFrameTime)
	{
		_pacman->frame++;
		if (_pacman->frame >= 2)
			_pacman->frame = 0;

		_pacman->currentFrameTime = 0;
	}

	_pacman->sourceRect->X = _pacman->sourceRect->Width * _pacman->frame;
	_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;
}

/// <summary> Returns true if Pacman is able to move </summary>
bool Pacman::CollisionCheck(float pacmanX, float pacmanY, direction directionOfMovement)
{
	float collisionX, collisionY;
	
	int roundedX, roundedY;

	switch (directionOfMovement)
	{
	case UP:
		roundedX = CalculateMazeX(_pacman->position->X, _pacman->sourceRect->Width, _mazeTileRect->Width);
		roundedY = CalculateMazeY(_pacman->position->Y - _cPacmanPosOffset, _pacman->sourceRect->Height, _mazeTileRect->Height);
		//subtracts offset so that the position is the centre
		break;

	case DOWN:
		roundedX = CalculateMazeX(_pacman->position->X, _pacman->sourceRect->Width, _mazeTileRect->Width);
		roundedY = CalculateMazeY(_pacman->position->Y + _cPacmanPosOffset, _pacman->sourceRect->Height, _mazeTileRect->Height);;
		//adds offset so that the position is the centre
		break;

	case LEFT:
		roundedX = CalculateMazeX(_pacman->position->X - _cPacmanPosOffset, _pacman->sourceRect->Width, _mazeTileRect->Width);
		//subtracts offset so that the position is the centre
		roundedY = CalculateMazeY(_pacman->position->Y, _pacman->sourceRect->Height, _mazeTileRect->Height);
		break;

	case RIGHT:
		roundedX = CalculateMazeX(_pacman->position->X + _cPacmanPosOffset, _pacman->sourceRect->Width, _mazeTileRect->Width);
		//adds offset so that the position is the centre
		roundedY = CalculateMazeY(_pacman->position->Y, _pacman->sourceRect->Height, _mazeTileRect->Height);
		break;

	default:
		roundedX = CalculateMazeX(_pacman->position->X, _pacman->sourceRect->Width, _mazeTileRect->Width);
		roundedY = CalculateMazeY(_pacman->position->Y, _pacman->sourceRect->Height, _mazeTileRect->Height);
		break;
	}

	if (roundedX >= _mazeWidth)
		roundedX = _mazeWidth - 1;

	if (roundedX < 0)
		roundedX = 0;

	return (_maze[roundedY][roundedX] == EMPTY || _maze[roundedY][roundedX] == PELLET || _maze[roundedY][roundedX] == POWER_PELLET);
}

///<summary> Loads the maze from mazeToCopy into maze
void Pacman::LoadMaze(mazeUnits(&maze)[_mazeHeight][_mazeWidth], const mazeUnits(&mazeToCopy)[_mazeHeight][_mazeWidth])
{
	for (int y = 0; y < _mazeHeight; y++)
	{
		for (int x = 0; x < _mazeWidth; x++)
			maze[y][x] = mazeToCopy[y][x];
	}
}

/// <summary> Returns the number of pellets in a given maze </summary>
int Pacman::GetNoOfPellets(mazeUnits(&mazeToCheck)[_mazeHeight][_mazeWidth])
{
	int pellets = 0;
	for (int y = 0; y < _mazeHeight; y++)
	{
		for (int x = 0; x < _mazeWidth; x++)
		{
			if (mazeToCheck[y][x] == PELLET || mazeToCheck[y][x] == POWER_PELLET)
				pellets++;
		}
	}

	return pellets;
}

/// <summary> Checks if the player is colliding with a pellet and increments score if they are </summary>
void Pacman::PelletCollisionCheck()
{
	if (_maze[CalculateMazeY(_pacman->position->Y,_pacman->sourceRect->Height,_mazeTileRect->Height)][CalculateMazeX(_pacman->position->X, _pacman->sourceRect->Width,_mazeTileRect->Width)] == PELLET)
	{
		_maze[CalculateMazeY(_pacman->position->Y, _pacman->sourceRect->Height, _mazeTileRect->Height)][CalculateMazeX(_pacman->position->X, _pacman->sourceRect->Width, _mazeTileRect->Width)] = EMPTY;
		_pacman->score += _cPelletValue;
		_pelletsCollected++;
	}
	else if (_maze[CalculateMazeY(_pacman->position->Y, _pacman->sourceRect->Height, _mazeTileRect->Height)][CalculateMazeX(_pacman->position->X, _pacman->sourceRect->Width, _mazeTileRect->Width)] == POWER_PELLET)
	{
		_maze[CalculateMazeY(_pacman->position->Y, _pacman->sourceRect->Height, _mazeTileRect->Height)][CalculateMazeX(_pacman->position->X, _pacman->sourceRect->Width, _mazeTileRect->Width)] = EMPTY;
		_pacman->score += _cPowerPelletValue;
		_pelletsCollected++;
		_poweredUp = true;
		_powerTimer = _cPoweredUpTime;
	}
}

/// <summary> Checks if pacman has gone off screen and then alters pacmans position </summary>
void Pacman::ScreenWrapCheck(const float leftLimit, const float rightLimit)
{
	if (_pacman->position->X < leftLimit)
	{
		_pacman->position->X += (_mazeWidth + 1) * _mazeTileRect->Width;
	}

	if (_pacman->position->X > rightLimit)
	{
		_pacman->position->X -= (_mazeWidth + 2) * _mazeTileRect->Width;
	}
}

/// <summary> Checks if the player has all available pellets and resets the level if they do </summary>
void Pacman::LevelWinCheck()
{
	if (_pelletsCollected == _noPelletsAvailable && !_levelEnd)
	{
		_levelEnd = true;
		_delayInMilli = _cLevelEndDelay;
		_delay = true;
	}

	if (_levelEnd && !_delay)
	{
		//should have a function for setting intial position
		ResetLevel();
		ResetMaze();
		_level++;
		_levelEnd = false;
		_delay = true;
		_delayInMilli = _cLevelStartDelay;
	}
}

/// <summary> Resets pacman position, enemies and powerup timer</summary>
void Pacman::ResetLevel()
{
	_pacman->position->X = Graphics::GetViewportWidth() / 2.0f - _pacman->sourceRect->Width / 2.0f;
	_pacman->position->Y = 17 * _mazeTileRect->Width - _pacman->sourceRect->Height / 4.0f;

	_poweredUp = false;
	_powerTimer = 0;

	delete _enemies[0];
	delete _enemies[1];
	delete _enemies[2];
	delete _enemies[3];

	CreateAndInitGhosts();
}

/// <summary> Resets the maze</summary>
void Pacman::ResetMaze()
{
	LoadMaze(_maze, _mazeTemplate);
	_pelletsCollected = 0;
	_noPelletsAvailable = GetNoOfPellets(_maze);
}

/// <summary> Decrements the delay counter if _delay is true and resets _delay when the counter reaches 0 </summary>
void Pacman::DelayCountdown(int elapsedTime) 
{
	if (_delay) {
		_delayInMilli -= elapsedTime;

		if (_delayInMilli <= 0)
		{
			_delay = false;
			_delayInMilli = 0;
		}
	}
}

/// <summary>Creates new enemy objects for each ghost and initialises them </summary>
void Pacman::CreateAndInitGhosts()
{
	Texture2D* blinkyTexture = new Texture2D();
	blinkyTexture->Load("Textures/Blinky.png", false);
	Rect* blinkyRect = new Rect(0.0f, 0.0f, 64, 64);
	Vector2* blinkyPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f - _pacman->sourceRect->Width / 2.0f, 11 * _mazeTileRect->Width - _pacman->sourceRect->Height / 4.0f);
	_enemies[0] = new Enemy(blinkyTexture, blinkyPosition, blinkyRect, &_maze, 0, _mazeTileRect->Width, _mazeTileRect->Height);

	Texture2D* pinkyTexture = new Texture2D();
	pinkyTexture->Load("Textures/Pinky.png", false);
	Rect* pinkyRect = new Rect(0.0f, 0.0f, 64, 64);
	Vector2* pinkyPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f - _pacman->sourceRect->Width / 2.0f, 14 * _mazeTileRect->Width - _pacman->sourceRect->Height / 4.0f);
	_enemies[1] = new Enemy(pinkyTexture, pinkyPosition, pinkyRect, &_maze, 1, _mazeTileRect->Width, _mazeTileRect->Height);

	Texture2D* inkyTexture = new Texture2D();
	inkyTexture->Load("Textures/Inky.png", false);
	Rect* inkyRect = new Rect(0.0f, 0.0f, 64, 64);
	Vector2* inkyPos = new Vector2(Graphics::GetViewportWidth() / 2.0f - _pacman->sourceRect->Width / 2.0f, 14 * _mazeTileRect->Width - _pacman->sourceRect->Height / 4.0f);
	_enemies[2] = new Enemy(inkyTexture, inkyPos, inkyRect, &_maze, 2, _mazeTileRect->Width, _mazeTileRect->Height);

	Texture2D* clydeTexture = new Texture2D();
	clydeTexture->Load("Textures/Clyde.png", false);
	Rect* clydeRect = new Rect(0.0f, 0.0f, 64, 64);
	Vector2* clydePosition = new Vector2(Graphics::GetViewportWidth() / 2.0f - _pacman->sourceRect->Width / 2.0f, 14 * _mazeTileRect->Width - _pacman->sourceRect->Height / 4.0f);
	_enemies[3] = new Enemy(clydeTexture, clydePosition, clydeRect, &_maze, 3, _mazeTileRect->Width, _mazeTileRect->Height);
}

/// <summary> Pacman has died, so reset the level (but not the maze) and start a delay </summary>
void Pacman::PacmanDeath()
{
	ResetLevel();
	_delay = true;
	_delayInMilli = _cLevelStartDelay;
}