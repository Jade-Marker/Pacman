#include "Pacman.h"
#include <sstream>

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPacmanPosOffset(20.0f), _cPacmanFrameTime(250), _cLevelEndDelay(1000), _cLevelStartDelay(5000), _cPoweredUpTime(7000), _cPelletValue(10), _cPowerPelletValue(20), _cEnemyValue(50), _cPelletFrameTime(500), _cCherryValue(100), _cCherryX(13), _cCherryY(17), _cProportionOfPelletsRequired(0.1f), _cDeathDelay(3000), _cCherryFrameTime(1000)
{
	_pacman = new Player();
	_pacman->playerSprite.direction = RIGHT;
	_pacman->playerSprite.currentFrameTime = 0;
	_pacman->playerSprite.frame = 0;
	_pacman->playerSprite.noOfFrames = 2;
	_pacman->score = 0;
	_pacman->lives = 3;
	_pacman->alive = true;

	_pauseMenu = new Menu();
	_pauseMenu->inUse = false;
	_pauseMenu->keyDown = false;
	_pauseMenu->interactKey = Input::Keys::P;

	_startMenu = new Menu();
	_startMenu->inUse = true;
	_startMenu->interactKey = Input::Keys::SPACE;

	_gameOverMenu = new Menu();
	_gameOverMenu->inUse = false;
	_gameOverMenu->interactKey = Input::Keys::R;

	_levelEnd = false;

	_poweredUp = false;
	_powerTimer = 0;

	_currentPelletFrameTime = 0;
	_pelletFrame = 0;

	_currentCherryFrameTime = 0;
	_cherryFrame = 0;

	_level = 1;
	_pelletsCollected = 0;

	_cherryGivenToPlayer = false;

	LoadMaze(_maze, cMazeTemplate);
	_noPelletsAvailable = GetNoOfPellets(_maze);

	_intro = new SoundEffect();
	_munch = new SoundEffect();

	//Initialise important Game aspects
	Audio::Initialise();
	Graphics::Initialise(argc, argv, this, 1920, 1080, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	delete _pacman->scoreOutputPos;
	delete _pacman->livesOutputPos;
	delete _pacman->playerSprite.position;
	delete _pacman->playerSprite.sourceRect;
	delete _pacman->playerSprite.texture;

	delete _pauseMenu->texture;
	delete _pauseMenu->rect;
	delete _pauseMenu->stringPosition;
	delete _pauseMenu;

	delete _startMenu->texture;
	delete _startMenu->rect;
	delete _startMenu;

	delete _gameOverMenu->texture;
	delete _gameOverMenu->rect;
	delete _gameOverMenu;

	delete _overlay;
	delete _overlayRect;

	delete _mazeTileset;
	delete _mazeTileRect;
	delete _backgroundPos;
	delete _backgroundColor;
	delete _backgroundColorVector;

	delete _intro;
	delete _munch;
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
	_pacman->playerSprite.position = new Vector2();
	SetInitialPacmanPosition();

	// Set string output positions
	_pacman->scoreOutputPos = new Vector2(10.0f, 25.0f);
	_pacman->livesOutputPos = new Vector2(10.0f, 45.0f);

	//Set Pause Menu Parameters
	_pauseMenu->texture = new Texture2D();
	_pauseMenu->texture->Load("Textures/Transparency.png", false);
	_pauseMenu->rect = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_pauseMenu->stringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

	//Start Menu Parameters
	_startMenu->texture = new Texture2D();
	_startMenu->texture->Load("Textures/Start.png", false);
	_startMenu->rect = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());

	//GameOver Menu Parameters
	_gameOverMenu->texture = new Texture2D();
	_gameOverMenu->texture->Load("Textures/GameOver.png", false);
	_gameOverMenu->rect = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());

	//load overlay
	_overlay = new Texture2D();
	_overlay->Load("Textures/Overlay.png", false);
	_overlayRect = new Rect(0.0f, -40.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());

	//+4 & +1 so that the tile for screen wrapping is not visible and so that the sprite fully dissapears before wrapping
	leftLimit = Graphics::GetViewportWidth() / 2.0f - ((cMazeWidth + 4) * cTilesetTileWidth) / 2.0f;
	rightLimit = Graphics::GetViewportWidth() / 2.0f + ((cMazeWidth + 1) * cTilesetTileWidth) / 2.0f;
	CreateAndInitGhosts();

	//Load music/SFX
	_intro->Load("Music & SFX/Intro.wav");
	_munch->Load("Music & SFX/Munch.wav");
}

void Pacman::Update(int elapsedTime)
{
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();

	CheckGameOver(keyboardState, _gameOverMenu->interactKey);
	if (!_gameOverMenu->inUse)
	{
		CheckStart(keyboardState, _startMenu->interactKey);

		if (!_startMenu->inUse)
		{
			CheckPaused(keyboardState, _pauseMenu->interactKey);

			if (!_pauseMenu->inUse) {
				if (!_delay) {
					if (!_pacman->alive)
					{
						ResetLevel();
						_delay = true;
						_delayInMilli = _cLevelStartDelay;
						_pacman->alive = true;
					}
					else {
						Input(elapsedTime, keyboardState);

						PelletCollisionCheck();
						CherryGiveCheck();
						LevelWinCheck();

						UpdatePacman(elapsedTime, _cPacmanFrameTime);
						ScreenWrapCheck();

						UpdateGhostAndCheckCollisions(elapsedTime);

						HandlePowerTimer(elapsedTime);
						UpdatePelletAndCherry(elapsedTime);
					}
				}
				else if (!_pacman->alive)
				{
					_pacman->playerSprite.noOfFrames = 5;
					//+ _cDeathDelay/8 so that the animation doesn't fully play
					//this means that the first frame doesn't appear before it should
					UpdatePacman(elapsedTime, _cDeathDelay / 5 + _cDeathDelay / 8);

				}
				DelayCountdown(elapsedTime);
			}
		}
	}
}

void Pacman::Draw(int elapsedTime)
{
	std::stringstream scoreOutput;
	std::stringstream livesOutput;
	std::stringstream menuStream;


	SpriteBatch::BeginDraw();

	SpriteBatch::DrawRectangle(_backgroundColorVector, Graphics::GetViewportWidth(), Graphics::GetViewportHeight(), _backgroundColor);

	if (_gameOverMenu->inUse)
	{
		SpriteBatch::Draw(_gameOverMenu->texture, _gameOverMenu->rect, nullptr);
	}
	else if (_startMenu->inUse)
	{
		SpriteBatch::Draw(_startMenu->texture, _startMenu->rect, nullptr);
	}
	else {
		for (int i = 0; i < cMazeHeight; i++)
		{
			_backgroundPos->Y = cTilesetTileHeight * i;
			for (int j = 0; j < cMazeWidth; j++)
			{
				_backgroundPos->X = cTilesetTileWidth * j + (Graphics::GetViewportWidth() / 2 - (cMazeWidth * cTilesetTileWidth) / 2);
				_mazeTileRect->X = cTilesetTileWidth * _maze[i][j];
				switch (_maze[i][j])
				{
				//have separate cases for tiles which need to be animated
				case POWER_PELLET:
					_mazeTileRect->Y = cTilesetTileHeight * _pelletFrame;
					break;
				case CHERRY:
					_mazeTileRect->Y = cTilesetTileHeight * _cherryFrame;
					break;
				default:
					_mazeTileRect->Y = 0.0f;
					break;
				}

				SpriteBatch::Draw(_mazeTileset, _backgroundPos, _mazeTileRect);
			}
		}

		SpriteBatch::Draw(_pacman->playerSprite.texture, _pacman->playerSprite.position, _pacman->playerSprite.sourceRect);

		for (int i = 0; i < _cEnemyCount; i++)
		{
			SpriteBatch::Draw(_enemies[i]->GetTexturePointer(), _enemies[i]->GetVectorPointer(), _enemies[i]->GetRectPointer());
		}

		SpriteBatch::Draw(_overlay, _overlayRect, nullptr);

		scoreOutput << "Score: " << _pacman->score;
		SpriteBatch::DrawString(scoreOutput.str().c_str(),_pacman->scoreOutputPos, Color::Red);

		livesOutput << "Lives: " << _pacman->lives;
		SpriteBatch::DrawString(livesOutput.str().c_str(), _pacman->livesOutputPos, Color::Red);


		if (_pauseMenu->inUse)
		{
			menuStream << "PAUSED!";

			SpriteBatch::Draw(_pauseMenu->texture, _pauseMenu->rect, nullptr);
			SpriteBatch::DrawString(menuStream.str().c_str(), _pauseMenu->stringPosition, Color::Red);
		}
	}
	SpriteBatch::EndDraw();
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
	if (state->IsKeyDown(pauseKey) && !_pauseMenu->keyDown && !_delay)
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
		Audio::Play(_intro);
	}
}

/// <summary> Checks if the player has pressed the key to restart </summary>
void Pacman::CheckGameOver(Input::KeyboardState* state, Input::Keys restartKey)
{
	if (state->IsKeyDown(restartKey) && _gameOverMenu->inUse)
	{
		_gameOverMenu->inUse = false;
		ResetLevel();
		ResetMaze();
		_delay = true;
		_delayInMilli = _cLevelStartDelay;
		_level = 1;
		_pacman->score = 0;
		_pacman->lives = 3;
		_pacman->alive = true;
	}
}

/// <summary> Update rect for pacman based on frame and direction </summary>
void Pacman::UpdatePacman(int elapsedTime, int frameTime)
{
	_pacman->playerSprite.Animate(elapsedTime, frameTime);

	_pacman->playerSprite.sourceRect->X = _pacman->playerSprite.sourceRect->Width * _pacman->playerSprite.frame;
	_pacman->playerSprite.sourceRect->Y = _pacman->playerSprite.sourceRect->Height * _pacman->playerSprite.direction;
}

/// <summary> Returns true if Pacman is able to move </summary>
bool Pacman::CollisionCheck(float pacmanX, float pacmanY, direction directionOfMovement)
{	
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

	return (_maze[roundedY][roundedX] == EMPTY || _maze[roundedY][roundedX] == PELLET || _maze[roundedY][roundedX] == POWER_PELLET || _maze[roundedY][roundedX] == CHERRY);
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

/// <summary> Checks if the player is colliding with a pellet, power pellet or a cherry and increments score if they are </summary>
void Pacman::PelletCollisionCheck()
{
	int mazeX = CalculateMazeX(_pacman->playerSprite.position->X, _pacman->playerSprite.sourceRect->Width, cTilesetTileWidth);
	int mazeY = CalculateMazeY(_pacman->playerSprite.position->Y, _pacman->playerSprite.sourceRect->Height, cTilesetTileHeight);

	switch (_maze[mazeY][mazeX])
	{
	case PELLET:
		_maze[mazeY][mazeX] = EMPTY;
		_pacman->score += _cPelletValue;
		_pelletsCollected++;
		if (_munch->GetState() != SoundEffectState::PLAYING)
			Audio::Play(_munch);
		break;

	case POWER_PELLET:
		_maze[mazeY][mazeX] = EMPTY;
		_pacman->score += _cPowerPelletValue;
		_pelletsCollected++;
		_poweredUp = true;
		_powerTimer = _cPoweredUpTime;
		if (_munch->GetState() != SoundEffectState::PLAYING)
			Audio::Play(_munch);
		break;

	case CHERRY:
		_maze[mazeY][mazeX] = EMPTY;
		_pacman->score += _cCherryValue;
		if (_munch->GetState() != SoundEffectState::PLAYING)
			Audio::Play(_munch);
		break;
	}
}

/// <summary> Places the cherry in the maze if the player hasn't already been given it and has collected enough pellets </summary>
void Pacman::CherryGiveCheck()
{
	if (_pelletsCollected >= _cProportionOfPelletsRequired * _noPelletsAvailable && !_cherryGivenToPlayer)
	{
		_maze[_cCherryY][_cCherryX] = CHERRY;
		_cherryGivenToPlayer = true;
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
	SetInitialPacmanPosition();

	_pacman->playerSprite.sourceRect->X = 0;
	_pacman->playerSprite.sourceRect->Y = 0;
	_pacman->playerSprite.frame = 0;
	_pacman->playerSprite.noOfFrames = 2;
	_pacman->playerSprite.currentFrameTime = 0;

	_poweredUp = false;
	_powerTimer = 0;

	for (int i = 0; i < _cEnemyCount; i++)
	{
		delete _enemies[i];
	}
	CreateAndInitGhosts();

	_maze[_cCherryY][_cCherryX] = EMPTY;

	Audio::Play(_intro);
}

/// <summary> Resets the maze</summary>
void Pacman::ResetMaze()
{
	LoadMaze(_maze, cMazeTemplate);
	_pelletsCollected = 0;
	_noPelletsAvailable = GetNoOfPellets(_maze);
	_cherryGivenToPlayer = false;
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
	_enemies[0] = new Ghost(blinkyTexture, blinkyPosition, blinkyRect, &_maze, 0, leftLimit, rightLimit);

	Texture2D* pinkyTexture = new Texture2D();
	pinkyTexture->Load("Textures/Pinky.png", false);
	Rect* pinkyRect = new Rect(0.0f, 0.0f, 64, 64);
	Vector2* pinkyPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f - _pacman->playerSprite.sourceRect->Width / 2.0f, 14 * cTilesetTileWidth - _pacman->playerSprite.sourceRect->Height / 4.0f);
	_enemies[1] = new Ghost(pinkyTexture, pinkyPosition, pinkyRect, &_maze, 1, leftLimit, rightLimit);

	Texture2D* inkyTexture = new Texture2D();
	inkyTexture->Load("Textures/Inky.png", false);
	Rect* inkyRect = new Rect(0.0f, 0.0f, 64, 64);
	Vector2* inkyPos = new Vector2(Graphics::GetViewportWidth() / 2.0f - _pacman->playerSprite.sourceRect->Width / 2.0f, 14 * cTilesetTileWidth - _pacman->playerSprite.sourceRect->Height / 4.0f);
	_enemies[2] = new Ghost(inkyTexture, inkyPos, inkyRect, &_maze, 2, leftLimit, rightLimit);

	Texture2D* clydeTexture = new Texture2D();
	clydeTexture->Load("Textures/Clyde.png", false);
	Rect* clydeRect = new Rect(0.0f, 0.0f, 64, 64);
	Vector2* clydePosition = new Vector2(Graphics::GetViewportWidth() / 2.0f - _pacman->playerSprite.sourceRect->Width / 2.0f, 14 * cTilesetTileWidth - _pacman->playerSprite.sourceRect->Height / 4.0f);
	_enemies[3] = new Ghost(clydeTexture, clydePosition, clydeRect, &_maze, 3, leftLimit, rightLimit);
}

/// <summary> Pacman has died, so reset the level (but not the maze) and start a delay </summary>
void Pacman::PacmanDeath()
{
	_pacman->alive = false;

	_pacman->lives--;
	if (_pacman->lives <= 0)
		_gameOverMenu->inUse = true;

	_delay = true;
	_delayInMilli = _cDeathDelay;
}

/// <summary> Updates the ghosts and checks for collisions with ghosts </summary>
void Pacman::UpdateGhostAndCheckCollisions(int elapsedTime)
{
	for (int i = 0; i < _cEnemyCount; i++)
	{
		bool colidedWithGhost = false;

		_enemies[i]->Update(elapsedTime, _level, _pacman->playerSprite.direction, _pacman->playerSprite.position->X, _pacman->playerSprite.position->Y, _enemies[0], _poweredUp, colidedWithGhost);
		ghostMode ghostMode = _enemies[i]->GetMode();
		if (colidedWithGhost)
		{
			if (ghostMode == FRIGHTENED)
			{
				_enemies[i]->GhostHasBeenEaten();
				_pacman->score += _cEnemyValue;
				if (_munch->GetState() != SoundEffectState::PLAYING)
					Audio::Play(_munch);
			}
			else if (ghostMode != EATEN)
			{
				PacmanDeath();
				break;
			}
		}
	}
}

/// <summary> Handles powerTimer </summary>
void Pacman::HandlePowerTimer(int elapsedTime)
{
	_powerTimer -= elapsedTime;
	if (_powerTimer <= 0)
	{
		_poweredUp = false;
		_powerTimer = 0;
	}
}

/// <summary> Updates the frame values for the power pellets and for the cherry </summary>
void Pacman::UpdatePelletAndCherry(int elapsedTime)
{
	Sprite::Animate(elapsedTime, _cPelletFrameTime, 2, _pelletFrame, _currentPelletFrameTime);

	Sprite::Animate(elapsedTime, _cCherryFrameTime, 2, _cherryFrame, _currentCherryFrameTime);
}

/// <summary> Sets the inital xy position for pacman </summary>
void Pacman::SetInitialPacmanPosition()
{
	_pacman->playerSprite.position->X = Graphics::GetViewportWidth() / 2.0f - _pacman->playerSprite.sourceRect->Width / 2.0f;
	_pacman->playerSprite.position->Y = 17 * cTilesetTileWidth - _pacman->playerSprite.sourceRect->Height / 4.0f;
}