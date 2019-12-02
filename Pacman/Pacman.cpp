#include "Pacman.h"

#include <sstream>

//todo
//https://gameinternals.com/understanding-pac-man-ghost-behavior

//Redraw ghosts

//Music when starting level

//SFX when eating pellet

//Power pellets need to be animated

//Need to add cherries

//Add lives

//Animation when dying

//Start screen with buttons

//Link to portfolio page about it with button on start screen
//ShellExecute(0, 0, L"https://www.google.com", 0, 0, SW_SHOW);

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPacmanPosOffset(20.0f), _cPacmanFrameTime(250), _cLevelEndDelay(1000), _cLevelStartDelay(2000), _cPoweredUpTime(7000), _cPelletValue(10), _cPowerPelletValue(20), _cEnemyValue(50)
{
	_pacman = new Player();
	_pacman->playerSprite.direction = RIGHT;
	_pacman->playerSprite.currentFrameTime = 0;
	_pacman->playerSprite.frame = 0;
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

	LoadMaze(_maze, cMazeTemplate);
	_noPelletsAvailable = GetNoOfPellets(_maze);

	//Initialise important Game aspects
	Graphics::Initialise(argc, argv, this, 1920, 1080, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	delete _pacman->scoreOutputPos;
	delete _pacman->playerSprite.position;
	delete _pacman->playerSprite.sourceRect;
	delete _pacman->playerSprite.texture;

	delete _stringPosition;

	delete _pauseMenu->texture;
	delete _pauseMenu->rect;
	delete _pauseMenu->stringPosition;
	delete _pauseMenu;

	delete _startMenu->texture;
	delete _startMenu->rect;
	delete _startMenu;

	delete _overlay;
	delete _overlayRect;

	delete _mazeTileset;
	delete _mazeTileRect;
	delete _backgroundPos;
	delete _backgroundColor;
	delete _backgroundColorVector;
}

void Pacman::LoadContent()
{
	//Load background texture + setup background rect
	_mazeTileset = new Texture2D();
	_mazeTileset->Load("Textures/backgroundTiles.png", false);
	_mazeTileRect = new Rect(0.0f, 0.0f, cTilesetTileWidth, cTilesetTileHeight);
	_backgroundPos = new Vector2();

	//setup background color for maze
	_backgroundColor = new Color();
	_backgroundColor->R = 208.0f / 255.0f;
	_backgroundColor->B = 48.0f / 255.0f;
	_backgroundColor->G = 52.0f / 255.0f;
	_backgroundColor->A = 0.8f;
	_backgroundColorVector = new Vector2(0, 0);

	// Load Pacman
	_pacman->playerSprite.texture = new Texture2D();
	_pacman->playerSprite.texture->Load("Textures/Pacman.png", false);
	_pacman->playerSprite.sourceRect = new Rect(0.0f, 0.0f, 64, 64);
	_pacman->playerSprite.position = new Vector2(Graphics::GetViewportWidth() / 2.0f - _pacman->playerSprite.sourceRect->Width / 2.0f, 17 * cTilesetTileWidth - _pacman->playerSprite.sourceRect->Height / 4.0f);

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
	_startMenu->rect = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());

	//load overlay
	_overlay = new Texture2D();
	_overlay->Load("Textures/Overlay.png", false);
	_overlayRect = new Rect(0.0f, -40.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());

	//+4 & +1 so that the tile for screen wrapping is not visible and so that the sprite fully dissapears before wrapping
	leftLimit = Graphics::GetViewportWidth() / 2.0f - ((cMazeWidth + 4) * cTilesetTileWidth) / 2.0f;
	rightLimit = Graphics::GetViewportWidth() / 2.0f + ((cMazeWidth + 1) * cTilesetTileWidth) / 2.0f;
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

				ScreenWrapCheck();

				for (int i = 0; i < _enemyCount; i++)
				{
					bool colidedWithGhost = false;
					bool ghostInChaseOrScatter = false;

					_enemies[i]->Update(elapsedTime, _level, _pacman->playerSprite.direction, _pacman->playerSprite.position->X, _pacman->playerSprite.position->Y, _enemies[0], _poweredUp, colidedWithGhost);
					ghostMode ghostMode = _enemies[i]->GetMode();
					if (colidedWithGhost)
					{
						if (ghostMode == FRIGHTENED)
						{
							_enemies[i]->GhostHasBeenEaten();
							_pacman->score += _cEnemyValue;
						}
						else if (ghostMode != EATEN)
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

	outputX = CalculateMazeX(_pacman->playerSprite.position->X, _pacman->playerSprite.sourceRect->Width, cTilesetTileWidth);
	outputY = CalculateMazeY(_pacman->playerSprite.position->Y, _pacman->playerSprite.sourceRect->Height, cTilesetTileHeight);

	//stream << "Currently on: " << _maze[outputY][outputX];
	stream << "Pacman X: " << outputX << " Y: " << outputY;
	//stream << "Pacman X: " << outputX << " Y: " << outputY << "Collision: " << CollisionCheck(_pacmanPosition->X, _pacmanPosition->Y);
	//stream << "Pacman X: " << _pacman->playerSprite.position->X<< " Y: " << _pacman->playerSprite.position->Y;
	//stream << "Pacman X: " << _pacmanPosition->X << " warp X: " << Graphics::GetViewportWidth() / 2.0f + (27 * 32) / 2.0f;
	stream << "  Pellets available: " << _noPelletsAvailable << "  Pellets collected: " << _pelletsCollected;
	stream << "  Level: " << _level;
	stream << "  Delay: " << _delayInMilli;
	stream << "  PoweredUp: " << _poweredUp;
	stream << "  PowerTimer: " << _powerTimer;

	SpriteBatch::BeginDraw(); // Starts Drawing

	SpriteBatch::DrawRectangle(_backgroundColorVector, Graphics::GetViewportWidth(), Graphics::GetViewportHeight(), _backgroundColor);

	if (_startMenu->inUse)
	{
		SpriteBatch::Draw(_startMenu->texture, _startMenu->rect, nullptr);
	}
	else {
		for (int i = 0; i < cMazeHeight; i++)
		{
			_backgroundPos->Y = cTilesetTileHeight * i;
			for (int j = 0; j < cMazeWidth; j++)
			{
				//https://www.spriters-resource.com/arcade/pacman/sheet/73389/
				_backgroundPos->X = cTilesetTileWidth * j + (Graphics::GetViewportWidth() / 2 - (cMazeWidth * cTilesetTileWidth) / 2);
				switch (_maze[i][j])
				{
				//have separate cases for tiles which need to be animated
				case PELLET:
					_mazeTileRect->X = cTilesetTileWidth * _maze[i][j];
					_mazeTileRect->Y = 0.0f;
					break;
				default:
					_mazeTileRect->X = cTilesetTileWidth * _maze[i][j];
					_mazeTileRect->Y = 0.0f;
					break;
				}

				SpriteBatch::Draw(_mazeTileset, _backgroundPos, _mazeTileRect);
			}
		}

		SpriteBatch::Draw(_pacman->playerSprite.texture, _pacman->playerSprite.position, _pacman->playerSprite.sourceRect); // Draws Pacman

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
		_pacman->playerSprite.direction = RIGHT;
		if (CollisionCheck(_pacman->playerSprite.position->X + _cPacmanSpeed * elapsedTime, _pacman->playerSprite.position->Y, RIGHT))
			_pacman->playerSprite.position->X += _cPacmanSpeed * elapsedTime;
	}
	else if (state->IsKeyDown(Input::Keys::A))
	{
		_pacman->playerSprite.direction = LEFT;
		if (CollisionCheck(_pacman->playerSprite.position->X - _cPacmanSpeed * elapsedTime, _pacman->playerSprite.position->Y, LEFT))
			_pacman->playerSprite.position->X -= _cPacmanSpeed * elapsedTime;
	}
	//Vertical movement
	else if (state->IsKeyDown(Input::Keys::W))
	{
		_pacman->playerSprite.direction = UP;
		if (CollisionCheck(_pacman->playerSprite.position->X, _pacman->playerSprite.position->Y - _cPacmanSpeed * elapsedTime, UP))
			_pacman->playerSprite.position->Y -= _cPacmanSpeed * elapsedTime;
	}
	else if (state->IsKeyDown(Input::Keys::S))
	{
		_pacman->playerSprite.direction = DOWN;
		if (CollisionCheck(_pacman->playerSprite.position->X, _pacman->playerSprite.position->Y + _cPacmanSpeed * elapsedTime, DOWN))
			_pacman->playerSprite.position->Y += _cPacmanSpeed * elapsedTime;
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
	_pacman->playerSprite.Animate(elapsedTime, _cPacmanFrameTime);

	_pacman->playerSprite.sourceRect->X = _pacman->playerSprite.sourceRect->Width * _pacman->playerSprite.frame;
	_pacman->playerSprite.sourceRect->Y = _pacman->playerSprite.sourceRect->Height * _pacman->playerSprite.direction;
}

/// <summary> Returns true if Pacman is able to move </summary>
bool Pacman::CollisionCheck(float pacmanX, float pacmanY, direction directionOfMovement)
{
	float collisionX, collisionY;
	
	int roundedX, roundedY;

	switch (directionOfMovement)
	{
	case UP:
		roundedX = CalculateMazeX(_pacman->playerSprite.position->X, _pacman->playerSprite.sourceRect->Width, cTilesetTileWidth);
		roundedY = CalculateMazeY(_pacman->playerSprite.position->Y - _cPacmanPosOffset, _pacman->playerSprite.sourceRect->Height, cTilesetTileHeight);
		//subtracts offset so that the position is the centre
		break;

	case DOWN:
		roundedX = CalculateMazeX(_pacman->playerSprite.position->X, _pacman->playerSprite.sourceRect->Width, cTilesetTileWidth);
		roundedY = CalculateMazeY(_pacman->playerSprite.position->Y + _cPacmanPosOffset, _pacman->playerSprite.sourceRect->Height, cTilesetTileHeight);;
		//adds offset so that the position is the centre
		break;

	case LEFT:
		roundedX = CalculateMazeX(_pacman->playerSprite.position->X - _cPacmanPosOffset, _pacman->playerSprite.sourceRect->Width, cTilesetTileWidth);
		//subtracts offset so that the position is the centre
		roundedY = CalculateMazeY(_pacman->playerSprite.position->Y, _pacman->playerSprite.sourceRect->Height, cTilesetTileHeight);
		break;

	case RIGHT:
		roundedX = CalculateMazeX(_pacman->playerSprite.position->X + _cPacmanPosOffset, _pacman->playerSprite.sourceRect->Width, cTilesetTileWidth);
		//adds offset so that the position is the centre
		roundedY = CalculateMazeY(_pacman->playerSprite.position->Y, _pacman->playerSprite.sourceRect->Height, cTilesetTileHeight);
		break;

	default:
		roundedX = CalculateMazeX(_pacman->playerSprite.position->X, _pacman->playerSprite.sourceRect->Width, cTilesetTileWidth);
		roundedY = CalculateMazeY(_pacman->playerSprite.position->Y, _pacman->playerSprite.sourceRect->Height, cTilesetTileHeight);
		break;
	}

	if (roundedX >= cMazeWidth)
		roundedX = cMazeWidth - 1;

	if (roundedX < 0)
		roundedX = 0;

	return (_maze[roundedY][roundedX] == EMPTY || _maze[roundedY][roundedX] == PELLET || _maze[roundedY][roundedX] == POWER_PELLET);
}

///<summary> Loads the maze from mazeToCopy into maze
void Pacman::LoadMaze(mazeUnits(&maze)[cMazeHeight][cMazeWidth], const mazeUnits(&mazeToCopy)[cMazeHeight][cMazeWidth])
{
	for (int y = 0; y < cMazeHeight; y++)
	{
		for (int x = 0; x < cMazeWidth; x++)
			maze[y][x] = mazeToCopy[y][x];
	}
}

/// <summary> Returns the number of pellets in a given maze </summary>
int Pacman::GetNoOfPellets(mazeUnits(&mazeToCheck)[cMazeHeight][cMazeWidth])
{
	int pellets = 0;
	for (int y = 0; y < cMazeHeight; y++)
	{
		for (int x = 0; x < cMazeWidth; x++)
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
	if (_maze[CalculateMazeY(_pacman->playerSprite.position->Y,_pacman->playerSprite.sourceRect->Height,cTilesetTileHeight)][CalculateMazeX(_pacman->playerSprite.position->X, _pacman->playerSprite.sourceRect->Width,cTilesetTileWidth)] == PELLET)
	{
		_maze[CalculateMazeY(_pacman->playerSprite.position->Y, _pacman->playerSprite.sourceRect->Height, cTilesetTileHeight)][CalculateMazeX(_pacman->playerSprite.position->X, _pacman->playerSprite.sourceRect->Width, cTilesetTileWidth)] = EMPTY;
		_pacman->score += _cPelletValue;
		_pelletsCollected++;
	}
	else if (_maze[CalculateMazeY(_pacman->playerSprite.position->Y, _pacman->playerSprite.sourceRect->Height, cTilesetTileHeight)][CalculateMazeX(_pacman->playerSprite.position->X, _pacman->playerSprite.sourceRect->Width, cTilesetTileWidth)] == POWER_PELLET)
	{
		_maze[CalculateMazeY(_pacman->playerSprite.position->Y, _pacman->playerSprite.sourceRect->Height, cTilesetTileHeight)][CalculateMazeX(_pacman->playerSprite.position->X, _pacman->playerSprite.sourceRect->Width, cTilesetTileWidth)] = EMPTY;
		_pacman->score += _cPowerPelletValue;
		_pelletsCollected++;
		_poweredUp = true;
		_powerTimer = _cPoweredUpTime;
	}
}

/// <summary> Checks if pacman has gone off screen and then alters pacmans position </summary>
void Pacman::ScreenWrapCheck()
{
	if (_pacman->playerSprite.position->X < leftLimit)
	{
		_pacman->playerSprite.position->X += (cMazeWidth + 1) * cTilesetTileWidth;
	}

	if (_pacman->playerSprite.position->X > rightLimit)
	{
		_pacman->playerSprite.position->X -= (cMazeWidth + 2) * cTilesetTileWidth;
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
	_pacman->playerSprite.position->X = Graphics::GetViewportWidth() / 2.0f - _pacman->playerSprite.sourceRect->Width / 2.0f;
	_pacman->playerSprite.position->Y = 17 * cTilesetTileWidth - _pacman->playerSprite.sourceRect->Height / 4.0f;

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
	LoadMaze(_maze, cMazeTemplate);
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
	Vector2* blinkyPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f - _pacman->playerSprite.sourceRect->Width / 2.0f, 11 * cTilesetTileWidth - _pacman->playerSprite.sourceRect->Height / 4.0f);
	_enemies[0] = new Enemy(blinkyTexture, blinkyPosition, blinkyRect, &_maze, 0, leftLimit, rightLimit);

	Texture2D* pinkyTexture = new Texture2D();
	pinkyTexture->Load("Textures/Pinky.png", false);
	Rect* pinkyRect = new Rect(0.0f, 0.0f, 64, 64);
	Vector2* pinkyPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f - _pacman->playerSprite.sourceRect->Width / 2.0f, 14 * cTilesetTileWidth - _pacman->playerSprite.sourceRect->Height / 4.0f);
	_enemies[1] = new Enemy(pinkyTexture, pinkyPosition, pinkyRect, &_maze, 1, leftLimit, rightLimit);

	Texture2D* inkyTexture = new Texture2D();
	inkyTexture->Load("Textures/Inky.png", false);
	Rect* inkyRect = new Rect(0.0f, 0.0f, 64, 64);
	Vector2* inkyPos = new Vector2(Graphics::GetViewportWidth() / 2.0f - _pacman->playerSprite.sourceRect->Width / 2.0f, 14 * cTilesetTileWidth - _pacman->playerSprite.sourceRect->Height / 4.0f);
	_enemies[2] = new Enemy(inkyTexture, inkyPos, inkyRect, &_maze, 2, leftLimit, rightLimit);

	Texture2D* clydeTexture = new Texture2D();
	clydeTexture->Load("Textures/Clyde.png", false);
	Rect* clydeRect = new Rect(0.0f, 0.0f, 64, 64);
	Vector2* clydePosition = new Vector2(Graphics::GetViewportWidth() / 2.0f - _pacman->playerSprite.sourceRect->Width / 2.0f, 14 * cTilesetTileWidth - _pacman->playerSprite.sourceRect->Height / 4.0f);
	_enemies[3] = new Enemy(clydeTexture, clydePosition, clydeRect, &_maze, 3, leftLimit, rightLimit);
}

/// <summary> Pacman has died, so reset the level (but not the maze) and start a delay </summary>
void Pacman::PacmanDeath()
{
	ResetLevel();
	_delay = true;
	_delayInMilli = _cLevelStartDelay;
}