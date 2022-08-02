#pragma once
#include "texture.h"
#include "mesh.h"

#define numImagesAtlas 9
#define numIntroButtons 4
#define numPauseButtons 3
#define numSaves 5
#define numEndButtons 3

enum atlasGuide {
	greenButton,
	greenButtonPressed,
	crosshair,
	fleshButton,
	fleshButtonPressed,
	turnPageRight,
	turnPageLeft,
	brownButton,
	trunLeftRed
};

class Button {
public:
	Vector2 pos;
	Mesh mesh;
	Vector4 range;
	float width;
	float height;
	Button(Vector2 pos, Vector4 range, float width, float height, bool flipuvs) {
		this->pos = pos;
		this->range = range;
		this->width = width;
		this->height = height;
		mesh.createQuad(pos.x, pos.y, width, height, flipuvs);
	};
};

class Gui{
public:
	Shader* shader;
	Texture* atlas;
	Vector4 atlasRanges[numImagesAtlas];
	Button* background;
	Button* introButtons[numIntroButtons];
	Button* pauseButtons[numPauseButtons];
	Button* worldSavesButtons[numSaves];
	Button* turnPageRightButton;
	Button* turnPageLeftButton;
	Button* turnBackLeftRedButton;
	Button* endButtons[numEndButtons];
	Button* tutorial;

	int worldPage;
	std::vector<std::string> entries;

	Gui(Shader* shader, Texture* atlas);
	//Inits
	void initAtlas();
	void initIntroButtons();
	void initPauseButtons();
	void initWorldButtons();
	void initEndButtons();
	void initTurnPageButtons();
	void initEntries();
	//Renders
	void RenderIntroGui();
	void RenderWorldsGui();
	void RenderPlayGui();
	void RenderCrosshair();
	void RenderPauseMenu();
	void RenderEndGui();
	void RenderTutorial();
	//ButtonPressed
	void introButtonPressed(Vector2 pos);
	int worldButtonPressed(Vector2 pos);
	void changePageButtonPressed(Vector2 pos);
	void pauseButtonPressed(Vector2 pos);
	void endButtonPressed(Vector2 pos);
};
