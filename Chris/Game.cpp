// Game.cpp
// Main game functions

#include "Game.h"
#include "Surface.h"	// Graphics utilities
#include <math.h>

const double FRAMETIME = 1./60.;

// Default constructor
Game::Game()
{
	windowWidth = 600;
	windowHeight = 400;

	Running = true;

	menu.On = false;
};

// Game setup
bool Game::Init()
{
	// Init SDL stuff
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		return false;
	}

	// Create a new level
	currentLevel = new Level();
	if (!currentLevel->Init("resources/lvl0.txt"))
		return false;

	Surface::Padding = TILESIZE;
	windowWidth = TILESIZE * currentLevel->Grid[0].size()
				+ Surface::Padding*2;
	windowHeight = TILESIZE * currentLevel->Grid.size()
				+ Surface::Padding*2;

	// Init the display surface
	Surface::Display = SDL_SetVideoMode(windowWidth, windowHeight, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

	if (!Surface::Display)
		return false;

	// Load the sprite sheet
	Surface::SpriteSheet = Surface::LoadImage("resources/sprites.bmp");

	if (!Surface::SpriteSheet)
		return false;

	// Set sprite transparency to RGB(255, 0, 255) AKA magenta
	SDL_SetColorKey(Surface::SpriteSheet, SDL_SRCCOLORKEY | SDL_RLEACCEL,
			SDL_MapRGB(Surface::SpriteSheet->format, 255, 0, 255));


	return true;
};

// Main execution loop
int Game::Run()
{
	if (!Init())
		return -1;

	// Event queue
	SDL_Event Event;

	while (Running)
	{
		while (SDL_PollEvent(&Event))
			OnEvent(&Event);

		Update();
		Render();
		Wait();
	}

	Cleanup();

	return 0;
};

// Event processing
void Game::OnEvent(SDL_Event *Event)
{
	switch (Event->type)
	{
		case SDL_QUIT:
			OnQuit();
			break;
		case SDL_KEYDOWN:
			OnKeyDown(Event->key.keysym.sym);
			break;
		case SDL_MOUSEMOTION:
			OnMouseMove(Event->motion.x, Event->motion.y);
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (Event->button.button)
			{
				case SDL_BUTTON_LEFT:
					OnLClick( Event->button.x,
						  Event->button.y);
					break;
				case SDL_BUTTON_RIGHT:
					OnRClick( Event->button.x,
						  Event->button.y);
					break;
				case SDL_BUTTON_MIDDLE:
					OnMClick( Event->button.x,
						  Event->button.y);
					break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch(Event->button.button)
			{
				case SDL_BUTTON_RIGHT:
					OnRClickRelease(Event->button.x,
							Event->button.y);
					break;
			}
			break;
	};
};

// Stuff to update each frame
void Game::Update()
{
	currentLevel->Update(FRAMETIME);
};

// Draw stuff
void Game::Render()
{
	// Clear Screen
//	Surface::DrawRect(Display, 0, 0, Display->w, Display->h,
//			255, 255, 255, 255);

	currentLevel->Render();
	drawMargins();
	currentLevel->RenderHUD();

	drawMenu();

	// Draw the display to screen (double buffer)
	SDL_Flip(Surface::Display);
};

void Game::drawMargins()
{
	SDL_Rect rect[4] = {
	{0, 0, windowWidth, Surface::Padding},
	{windowWidth-Surface::Padding, 0, Surface::Padding, windowHeight},
	{0, 0, Surface::Padding, windowHeight},
	{0, windowHeight-Surface::Padding,windowWidth, Surface::Padding}};
	for (int i = 0; i < 4; i++)
	{
		SDL_FillRect(Surface::Display, &rect[i], SDL_MapRGB(
				Surface::Display->format,128,128,128));
	}
};

void Game::drawMenu()
{
	if (menu.On)
	{
		int state = getMenuState();
		switch (state)
		{
			case 1:
				filledPieRGBA(Surface::Display, menu.x, menu.y,
						100, 225, 315,
						255, 255, 0, 128);
				break;
			case 2:
				filledPieRGBA(Surface::Display, menu.x, menu.y,
						100, -45, 45,
						255, 0, 0, 128);
				break;
			case 3:
				filledPieRGBA(Surface::Display, menu.x, menu.y,
						100, 45, 135,
						0, 255, 0, 128);
				break;
			case 4:
				filledPieRGBA(Surface::Display, menu.x, menu.y,
						100, 135, 225,
						0, 0, 255, 128);
				break;
			case 0:
			default:
				filledCircleRGBA(Surface::Display,
						menu.x, menu.y,
						100, 255,255,255,64);
				break;
		}
	}
};

int Game::getMenuState()
{	
	int numState = 4;
	int diffX = Mouse.x - menu.x;
	int diffY = Mouse.y - menu.y;
	if (diffX*diffX + diffY*diffY < 50*50)
		return 0;	// Inactive selection
	else
	{
		double angle = 180*atan2(diffY,diffX)/M_PI + 180;
		int value = (angle/(360/numState))+.5;
		if (value < 1)
			value += 4;

		return value;
	};

};

// Cleanup before exiting, a bit like a deconstructor
void Game::Cleanup()
{
	SDL_FreeSurface(Surface::Display);
	SDL_FreeSurface(Surface::SpriteSheet);

	delete currentLevel;

	// Cleanup SDL stuff
	SDL_Quit();
};





//////////////////////////////// Events ////////////////////////////////////

// End the game
void Game::OnQuit()
{
	Running = false;
};

// Process key press
void Game::OnKeyDown(SDLKey sym)
{
	switch (sym)
	{
		case SDLK_q:
			OnQuit();
			break;
		default:
			break;
	};
};

// Process left click
void Game::OnLClick(int x, int y)
{
};

// Process right click
void Game::OnRClick(int x, int y)
{
	menu.On = true;
	menu.x = (.5 + currentLevel->MouseGrid.x)*TILESIZE + Surface::Padding;
	menu.y = (.5 + currentLevel->MouseGrid.y)*TILESIZE + Surface::Padding;
	menu.target = currentLevel->isTower(currentLevel->MouseGrid.x,
						currentLevel->MouseGrid.y);
};

void Game::OnRClickRelease(int x, int y)
{
	if (menu.target)	// Tower selected:
	{
		switch (getMenuState())
		{
			case 1:	// Power
				currentLevel->UpgradeTower(menu.target, 0);
				break;
			case 2:	// Range
				currentLevel->UpgradeTower(menu.target, 1);
				break;
			case 3: // Rate
				currentLevel->UpgradeTower(menu.target, 2);
				break;
		};
	}
	else	// Not a tower
	{
		switch (getMenuState())
		{
			case 1:
				currentLevel->BuildTower(
					currentLevel->MouseGrid.x,
					currentLevel->MouseGrid.y,
				       	0);
				break;
		};
	}

	menu.On = false;
};

// Process middle click
void Game::OnMClick(int x, int y)
{
	currentLevel->BuildTower(	currentLevel->MouseGrid.x,
					currentLevel->MouseGrid.y,
				       		0);
};

// Trigger on mouse movement
void Game::OnMouseMove(int x, int y)
{
	// Store the new mouse position
	Mouse.x = x;
	Mouse.y = y;
	if (!menu.On)
	{
		currentLevel->MouseGrid.x = (Mouse.x-Surface::Padding)
						/ TILESIZE;
		currentLevel->MouseGrid.y = (Mouse.y-Surface::Padding)
						/ TILESIZE;
	}
};

// Waits for the next frame; based on libsdl.org/intro.en/usingtimers.html
void Game::Wait()
{
	static int next_time = 0;
	int now = SDL_GetTicks();
	if (next_time <= now)
		next_time = now + 1000*FRAMETIME;
	else
		SDL_Delay(next_time - now);
};
