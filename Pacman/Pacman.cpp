#include "Pacman.h"

#include <sstream>

//todo
//https://gameinternals.com/understanding-pac-man-ghost-behavior

//Clean up code based on week 7

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

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPacmanPosOffset(20.0f), _cPacmanFrameTime(250), _cLevelEndDelay(1000), _cLevelStartDelay(2000), _cPoweredUpTime(7000)
{
	_frameCount = 0;
	_paused = false;
	_pKeyDown = false;
	_start = true;
	_levelEnd = false;

	_poweredUp = false;
	_powerTimer = 0;

	_pacmanDirection = RIGHT;
	_pacmanCurrentFrameTime = 0;
	_pacmanFrame = 0;
	_score = 0;
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
	delete _pacmanTexture;
	delete _pacmanSourceRect;
	delete _munchieBlueTexture;
	delete _munchieInvertedTexture;
	delete _munchieRect;
}

void Pacman::LoadContent()
{
	//Load background texture + setup background rect
	_backgroundElements = new Texture2D();
	_backgroundElements->Load("Textures/backgroundTiles.png", false);
	_backgroundRect = new Rect(0.0f, 0.0f, 32, 32);
	_backgroundPos = new Vector2();

	// Load Pacman
	_pacmanTexture = new Texture2D();
	_pacmanTexture->Load("Textures/Pacman.png", false);
	_pacmanSourceRect = new Rect(0.0f, 0.0f, 64, 64);
	_pacmanPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f - _pacmanSourceRect->Width / 2.0f, 17 * _backgroundRect->Width - _pacmanSourceRect->Height / 4.0f);

	// Load Munchie
	_munchieBlueTexture = new Texture2D();
	_munchieBlueTexture->Load("Textures/Munchie.tga", true);
	_munchieInvertedTexture = new Texture2D();
	_munchieInvertedTexture->Load("Textures/MunchieInverted.tga", true);
	_munchieRect = new Rect(100.0f, 450.0f, 12, 12);

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);
	_scorePosition = new Vector2(10.0f, 45.0f);

	//Set Menu Parameters
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	//Start Menu Parameters
	_startBackground = new Texture2D();
	_startBackground->Load("Textures/Start.png", false);

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

	CheckStart(keyboardState, Input::Keys::SPACE);

	if (!_start)
	{
		CheckPaused(keyboardState, Input::Keys::P);

		if (!_paused) {
			if (!_delay) {
				Input(elapsedTime, keyboardState);
				PelletCollisionCheck();
				LevelWinCheck();
				UpdatePacman(elapsedTime);

				//+4 & +1 so that the tile for screen wrapping is not visible and so that pacman fully dissapears before wrapping
				ScreenWrapCheck(Graphics::GetViewportWidth() / 2.0f - ((_mazeWidth + 4) * _backgroundRect->Width) / 2.0f,
					Graphics::GetViewportWidth() / 2.0f + ((_mazeWidth + 1) * _backgroundRect->Width) / 2.0f);

				bool collision = false;
				bool anyInChaseOrScatter = false;

				for (int i = 0; i < _enemyCount; i++)
				{
					bool colidedWithGhost = false;
					bool ghostInChaseOrScatter = false;

					_enemies[i]->Update(elapsedTime, _level, _pacmanDirection, _pacmanPosition->X, _pacmanPosition->Y, _enemies[0], _poweredUp, colidedWithGhost, ghostInChaseOrScatter);
					if (colidedWithGhost)
					{
						collision = true;
						if (!ghostInChaseOrScatter)
						{
							_enemies[i]->GhostHasBeenEaten();
						}
						else
						{
							PacmanDeath();
							break;
						}
					}
					//if (ghostInChaseOrScatter)
					//{
					//	anyInChaseOrScatter = true;
					//}


				}

				//if (anyInChaseOrScatter && collision)
				//{
				//	//_pelletsCollected = _noPelletsAvailable;
				//	//should die here
				//	PacmanDeath();
				//}

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

	outputX = CalculateMazeX(_pacmanPosition->X, _pacmanSourceRect->Width, _backgroundRect->Width);
	outputY = CalculateMazeY(_pacmanPosition->Y, _pacmanSourceRect->Height, _backgroundRect->Height);

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

	if (_start)
	{
		SpriteBatch::Draw(_startBackground, _menuRectangle, nullptr);
	}
	else {
		for (int i = 0; i < _mazeHeight; i++)
		{
			_backgroundPos->Y = _backgroundRect->Height * i;
			for (int j = 0; j < _mazeWidth; j++)
			{
				//https://www.spriters-resource.com/arcade/pacman/sheet/73389/
				_backgroundPos->X = _backgroundRect->Width * j + (Graphics::GetViewportWidth() / 2 - (_mazeWidth * _backgroundRect->Width) / 2);
				switch (_maze[i][j])
				{
				//have separate cases for tiles which need to be animated
				case PELLET:
					_backgroundRect->X = _backgroundRect->Width * _maze[i][j];
					_backgroundRect->Y = 0.0f;
					break;
				default:
					_backgroundRect->X = _backgroundRect->Width * _maze[i][j];
					_backgroundRect->Y = 0.0f;
					break;
				}

				SpriteBatch::Draw(_backgroundElements, _backgroundPos, _backgroundRect);
			}
		}

		SpriteBatch::Draw(_pacmanTexture, _pacmanPosition, _pacmanSourceRect); // Draws Pacman

		//Draw enemies
		for (int i = 0; i < _enemyCount; i++)
		{
			SpriteBatch::Draw(_enemies[i]->GetTexturePointer(), _enemies[i]->GetVectorPointer(), _enemies[i]->GetRectPointer());
		}

		SpriteBatch::Draw(_overlay, _overlayRect, nullptr);

		// Draws String
		SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Red);

		std::stringstream scoreOutput;
		scoreOutput << "Score: " << _score;
		SpriteBatch::DrawString(scoreOutput.str().c_str(),_scorePosition, Color::Red);

		if (_paused)
		{
			std::stringstream menuStream;
			menuStream << "PAUSED!";

			SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
			SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
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
		_pacmanDirection = RIGHT;
		if (CollisionCheck(_pacmanPosition->X + _cPacmanSpeed * elapsedTime, _pacmanPosition->Y, RIGHT))
			_pacmanPosition->X += _cPacmanSpeed * elapsedTime;
	}
	else if (state->IsKeyDown(Input::Keys::A))
	{
		_pacmanDirection = LEFT;
		if (CollisionCheck(_pacmanPosition->X - _cPacmanSpeed * elapsedTime, _pacmanPosition->Y, LEFT))
			_pacmanPosition->X -= _cPacmanSpeed * elapsedTime;
	}
	//Vertical movement
	else if (state->IsKeyDown(Input::Keys::W))
	{
		_pacmanDirection = UP;
		if (CollisionCheck(_pacmanPosition->X, _pacmanPosition->Y - _cPacmanSpeed * elapsedTime, UP))
			_pacmanPosition->Y -= _cPacmanSpeed * elapsedTime;
	}
	else if (state->IsKeyDown(Input::Keys::S))
	{
		_pacmanDirection = DOWN;
		if (CollisionCheck(_pacmanPosition->X, _pacmanPosition->Y + _cPacmanSpeed * elapsedTime, DOWN))
			_pacmanPosition->Y += _cPacmanSpeed * elapsedTime;
	}
}

/// <summary> Checks if the player has pressed the pause key and then pauses or unpauses</summary>
void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	if (state->IsKeyDown(pauseKey) && !_pKeyDown)
	{
		_pKeyDown = true;
		_paused = !_paused;
	}

	if (state->IsKeyUp(pauseKey))
		_pKeyDown = false;
}

/// <summary> Checks if the player has pressed the start key </summary>
void Pacman::CheckStart(Input::KeyboardState* state, Input::Keys startKey)
{
	if (state->IsKeyDown(startKey))
	{
		_start = false;
		_delayInMilli = _cLevelStartDelay;
		_delay = true;
	}
}

/// <summary> Update rect for pacman based on frame and direction </summary>
void Pacman::UpdatePacman(int elapsedTime)
{
	_pacmanCurrentFrameTime += elapsedTime;
	if (_pacmanCurrentFrameTime > _cPacmanFrameTime)
	{
		_pacmanFrame++;
		if (_pacmanFrame >= 2)
			_pacmanFrame = 0;

		_pacmanCurrentFrameTime = 0;
	}

	_pacmanSourceRect->X = _pacmanSourceRect->Width * _pacmanFrame;
	_pacmanSourceRect->Y = _pacmanSourceRect->Height * _pacmanDirection;
}

/// <summary> Returns true if Pacman is able to move </summary>
bool Pacman::CollisionCheck(float pacmanX, float pacmanY, direction directionOfMovement)
{
	float collisionX, collisionY;
	
	int roundedX, roundedY;

	switch (directionOfMovement)
	{
	case UP:
		roundedX = CalculateMazeX(_pacmanPosition->X, _pacmanSourceRect->Width, _backgroundRect->Width);
		roundedY = CalculateMazeY(_pacmanPosition->Y - _cPacmanPosOffset, _pacmanSourceRect->Height, _backgroundRect->Height);
		//subtracts offset so that the position is the centre
		break;

	case DOWN:
		roundedX = CalculateMazeX(_pacmanPosition->X, _pacmanSourceRect->Width, _backgroundRect->Width);
		roundedY = CalculateMazeY(_pacmanPosition->Y + _cPacmanPosOffset, _pacmanSourceRect->Height, _backgroundRect->Height);;
		//adds offset so that the position is the centre
		break;

	case LEFT:
		roundedX = CalculateMazeX(_pacmanPosition->X - _cPacmanPosOffset, _pacmanSourceRect->Width, _backgroundRect->Width);
		//subtracts offset so that the position is the centre
		roundedY = CalculateMazeY(_pacmanPosition->Y, _pacmanSourceRect->Height, _backgroundRect->Height);
		break;

	case RIGHT:
		roundedX = CalculateMazeX(_pacmanPosition->X + _cPacmanPosOffset, _pacmanSourceRect->Width, _backgroundRect->Width);
		//adds offset so that the position is the centre
		roundedY = CalculateMazeY(_pacmanPosition->Y, _pacmanSourceRect->Height, _backgroundRect->Height);
		break;

	default:
		roundedX = CalculateMazeX(_pacmanPosition->X, _pacmanSourceRect->Width, _backgroundRect->Width);
		roundedY = CalculateMazeY(_pacmanPosition->Y, _pacmanSourceRect->Height, _backgroundRect->Height);
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
	if (_maze[CalculateMazeY(_pacmanPosition->Y,_pacmanSourceRect->Height,_backgroundRect->Height)][CalculateMazeX(_pacmanPosition->X,_pacmanSourceRect->Width,_backgroundRect->Width)] == PELLET)
	{
		_maze[CalculateMazeY(_pacmanPosition->Y, _pacmanSourceRect->Height, _backgroundRect->Height)][CalculateMazeX(_pacmanPosition->X, _pacmanSourceRect->Width, _backgroundRect->Width)] = EMPTY;
		_score += 10;
		_pelletsCollected++;
	}
	else if (_maze[CalculateMazeY(_pacmanPosition->Y, _pacmanSourceRect->Height, _backgroundRect->Height)][CalculateMazeX(_pacmanPosition->X, _pacmanSourceRect->Width, _backgroundRect->Width)] == POWER_PELLET)
	{
		_maze[CalculateMazeY(_pacmanPosition->Y, _pacmanSourceRect->Height, _backgroundRect->Height)][CalculateMazeX(_pacmanPosition->X, _pacmanSourceRect->Width, _backgroundRect->Width)] = EMPTY;
		_score += 20;
		_pelletsCollected++;
		_poweredUp = true;
		_powerTimer = _cPoweredUpTime;
	}
}

/// <summary> Checks if pacman has gone off screen and then alters pacmans position </summary>
void Pacman::ScreenWrapCheck(const float leftLimit, const float rightLimit)
{
	if (_pacmanPosition->X < leftLimit)
	{
		_pacmanPosition->X += (_mazeWidth + 1) * _backgroundRect->Width;
	}

	if (_pacmanPosition->X > rightLimit)
	{
		_pacmanPosition->X -= (_mazeWidth + 2) * _backgroundRect->Width;
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
	_pacmanPosition->X = Graphics::GetViewportWidth() / 2.0f - _pacmanSourceRect->Width / 2.0f;
	_pacmanPosition->Y = 17 * _backgroundRect->Width - _pacmanSourceRect->Height / 4.0f;

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
	Vector2* blinkyPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f - _pacmanSourceRect->Width / 2.0f, 11 * _backgroundRect->Width - _pacmanSourceRect->Height / 4.0f);
	_enemies[0] = new Enemy(blinkyTexture, blinkyPosition, blinkyRect, &_maze, 0, _backgroundRect->Width, _backgroundRect->Height);

	Texture2D* pinkyTexture = new Texture2D();
	pinkyTexture->Load("Textures/Pinky.png", false);
	Rect* pinkyRect = new Rect(0.0f, 0.0f, 64, 64);
	Vector2* pinkyPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f - _pacmanSourceRect->Width / 2.0f, 14 * _backgroundRect->Width - _pacmanSourceRect->Height / 4.0f);
	_enemies[1] = new Enemy(pinkyTexture, pinkyPosition, pinkyRect, &_maze, 1, _backgroundRect->Width, _backgroundRect->Height);

	Texture2D* inkyTexture = new Texture2D();
	inkyTexture->Load("Textures/Inky.png", false);
	Rect* inkyRect = new Rect(0.0f, 0.0f, 64, 64);
	Vector2* inkyPos = new Vector2(Graphics::GetViewportWidth() / 2.0f - _pacmanSourceRect->Width / 2.0f, 14 * _backgroundRect->Width - _pacmanSourceRect->Height / 4.0f);
	_enemies[2] = new Enemy(inkyTexture, inkyPos, inkyRect, &_maze, 2, _backgroundRect->Width, _backgroundRect->Height);

	Texture2D* clydeTexture = new Texture2D();
	clydeTexture->Load("Textures/Clyde.png", false);
	Rect* clydeRect = new Rect(0.0f, 0.0f, 64, 64);
	Vector2* clydePosition = new Vector2(Graphics::GetViewportWidth() / 2.0f - _pacmanSourceRect->Width / 2.0f, 14 * _backgroundRect->Width - _pacmanSourceRect->Height / 4.0f);
	_enemies[3] = new Enemy(clydeTexture, clydePosition, clydeRect, &_maze, 3, _backgroundRect->Width, _backgroundRect->Height);
}

/// <summary> Pacman has died, so reset the level (but not the maze) and start a delay </summary>
void Pacman::PacmanDeath()
{
	ResetLevel();
	_delay = true;
	_delayInMilli = _cLevelStartDelay;
}