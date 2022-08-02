/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the game, is in charge of creating the game, getting the user input, process the update and render.
*/

#define GAME_H

#include "includes.h"
#include "camera.h"
#include "utils.h"
#include "world.h"
#include "stage.h"
#include "gui.h"
#include "manager.h"
#include "audio.h"

#define MAX_EDITOR_ENTITIES 69

class Game
{
public:
	static Game* instance;

	//window
	SDL_Window* window;
	int window_width;
	int window_height;

	//some globals
	long frame;
    float time;
	float elapsed_time;
	int fps;
	bool must_exit;

	//some vars
	Camera* camera; //our global camera
	bool mouse_locked; //tells if the mouse is locked (not seen)

	//stages
	Stage* currentStage;
	IntroStage* intro = new IntroStage();
	SelectWorldStage* selectWorld = new SelectWorldStage();
	PlayStage* play = new PlayStage();
	EndStage* end = new EndStage();
	EditorStage* editor = new EditorStage();

	//Our audios and music
	Audio* introMusic = new Audio();
	Audio* ambiente = new Audio();
	Audio* disparo = new Audio();
	Audio* click = new Audio();
	Audio* feedbackPut = new Audio();
	Audio* narrador = new Audio();
	Audio* nextRound = new Audio();
 
	//Our stuff
	World* editorWorld;
	World* currentWorld;
	Entity* sky;
	Gui* gui;
	float mouse_speed = 10.0f;
	Manager* gameManager;
	bool tutorial = false;

	Game( int window_width, int window_height, SDL_Window* window );

	//main functions
	void render( void );
	void update( double dt );

	//events
	void onKeyDown( SDL_KeyboardEvent event );
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown( SDL_MouseButtonEvent event );
	void onMouseButtonUp(SDL_MouseButtonEvent event);
	void onMouseWheel(SDL_MouseWheelEvent event);
	void onGamepadButtonDown(SDL_JoyButtonEvent event);
	void onGamepadButtonUp(SDL_JoyButtonEvent event);
	void onResize(int width, int height);
	void initWorld(std::string filename = "");

	void setIntroStage();
	void setSelectWorldStage();
	void setPlayStage();
	void setEditorStage();
	void setEndStage();
};

