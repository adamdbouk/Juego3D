#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"

#include <cmath>

//some globals
Animation* anim = NULL;
FBO* fbo = NULL;
Game* Game::instance = NULL;

Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer
	
	//GameManager
	gameManager = new Manager();

	//Worlds
	initWorld();
	currentWorld = editorWorld;
	currentStage = intro;

	//GUI
	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/gui.fs");
	Texture* atlas = Texture::Get("data/atlas.png");
	gui = new Gui(shader, atlas);

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse

	//Audio
	if (BASS_Init(-1, 44100, 0, 0, NULL) == false) {
		std::cout << "AUDIO ERROR: tarjeta de sonido" << std::endl;
	}
	introMusic->channelSample = *introMusic->Play("data/Audio/menu.mp3");
}

//what to do when the image has to be draw
void Game::render(void)
{
	currentStage->render(currentWorld);
	
	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	currentStage->update(seconds_elapsed, currentWorld);
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		//case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
		case SDLK_F1: Shader::ReloadAll(); break; 
	}
}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse
	{
		if (tutorial) {
			tutorial = false;
		}
		else {
			Camera* camera = Camera::current;
			if (currentStage == intro) {
				gui->introButtonPressed(Vector2(Input::mouse_position.x, Input::mouse_position.y));
				click->Play("data/Audio/click.mp3");
			}
			else if (currentStage == selectWorld) {
				gui->changePageButtonPressed(Vector2(Input::mouse_position.x, Input::mouse_position.y));
				int worldPos = gui->worldButtonPressed(Vector2(Input::mouse_position.x, Input::mouse_position.y));
				if (worldPos >= 0) {
					std::string filename = gui->entries[worldPos + (gui->worldPage * 5)];
					initWorld(filename);
					setPlayStage();
				}
				click->Play("data/Audio/click.mp3");
			}
			else if (currentStage == play) {
				if (!mouse_locked) {
					gui->pauseButtonPressed(Vector2(Input::mouse_position.x, Input::mouse_position.y)); click->Play("data/Audio/click.mp3"); //sonidito de click 
				}
			}
			else if (currentStage == editor) {
				if (mouse_locked) {
					Vector3 dir = camera->getRayDirection(this->window_width / 2, this->window_height / 2, this->window_width, this->window_height);
					currentWorld->selectEntityEditor(dir);
					feedbackPut->Play("data/Audio/feedback.mp3");
				}
				else gui->pauseButtonPressed(Vector2(Input::mouse_position.x, Input::mouse_position.y));
				click->Play("data/Audio/click.mp3");
			}
			else if (currentStage == end) {
				gui->endButtonPressed(Vector2(Input::mouse_position.x, Input::mouse_position.y));
			}
		}
	}
	if (event.button == SDL_BUTTON_RIGHT) //right mouse
	{
		if (currentStage == editor) {
			if (currentWorld->selectedEntity != NULL) {
				currentWorld->selectedEntity->bounding = false;
				currentWorld->selectedEntity = NULL;
			}
		}
	}
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
	mouse_speed *= event.y > 0 ? 1.1 : 0.9;
}


void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

void Game::initWorld(std::string filename){
	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	Matrix44 m;
	//Player
	m.setTranslation(100, 100, 100);
	Vector3 playerInitPos = Vector3(0, 0.5, 0);
	Mesh* playerMesh = Mesh::Get("data/Assets/Players/man.obj");
	Texture* playerText = Texture::Get("data/Assets/Players/man.png");
	//Weapon
	Weapon* AK47 = new Weapon(0, 0.5, 0, m);
	AK47->loadMesh("data/Assets/Weapons/AK47.obj");
	AK47->loadTexture("data/Assets/Weapons/AK47.png");
	AK47->init(0.2f, 30, 5);
	//Sky
	Mesh* meshCielo = Mesh::Get("data/Assets/Ambiente/cielo.ASE");
	Texture* textCielo = Texture::Get("data/Assets/Ambiente/cielo.tga");
	//Ground
	Texture* groundText = Texture::Get("data/Assets/Structure/aigua.png"); 
	//Camera
	camera = new Camera();
	if (filename == "") {
		editorWorld = new World(shader);
		editorWorld->loadDecoration();
		editorWorld->loadStructure();
		editorWorld->loadSpawns();
		editorWorld->initPlayer(playerInitPos, playerMesh, playerText);
		editorWorld->initSky(meshCielo, textCielo);
		editorWorld->initGround(groundText);
		editorWorld->initCamera(camera);
		editorWorld->addWeapon(AK47);
		editorWorld->initZombies();
	}
	else {
		World* world = new World(shader);
		world->loadWorldInfo(filename);
		world->loadDecoration();
		world->loadStructure();
		world->loadSpawns();
		world->initPlayer(playerInitPos, playerMesh, playerText);
		world->initSky(meshCielo, textCielo);
		world->initGround(groundText);
		world->initCamera(camera);
		world->addWeapon(AK47);
		world->initMap();
		world->initZombies();
		currentWorld = world;
	}
}

//STAGES SETTINGS 
void Game::setIntroStage(){
	mouse_locked = false;
	currentStage = intro;
	SDL_ShowCursor(!mouse_locked);
	introMusic->Stop(introMusic->channelSample);
	introMusic->channelSample = *introMusic->Play("data/Audio/menu.mp3");
	ambiente->Stop(ambiente->channelSample);
	narrador->Stop(narrador->channelSample);
}

void Game::setSelectWorldStage() {
	mouse_locked = false;
	currentStage = selectWorld;
	SDL_ShowCursor(!mouse_locked);
	ambiente->Stop(ambiente->channelSample);
}

void Game::setPlayStage(){
	mouse_locked = true;
	gameManager->round = 0;
	gameManager->startedRoundTime = time;
	gameManager->initRound();
	SDL_ShowCursor(!mouse_locked);
	ambiente->channelSample = *ambiente->Play("data/Audio/ambiente.mp3");
	introMusic->Stop(introMusic->channelSample);
	currentStage = play;
}

void Game::setEditorStage(){
	mouse_locked = true;
	currentStage = editor;
	currentWorld = editorWorld;
	SDL_ShowCursor(!mouse_locked);
	ambiente->channelSample = *ambiente->Play("data/Audio/ambiente.mp3");
	introMusic->Stop(introMusic->channelSample);
}

void Game::setEndStage(){
	mouse_locked = false;
	currentStage = end;
	SDL_ShowCursor(!mouse_locked);
	ambiente->Stop(ambiente->channelSample);
	narrador->channelSample = *narrador->Play("data/Audio/gameover.mp3");
	introMusic->channelSample = *introMusic->Play("data/Audio/menu.mp3");
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{
}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
}