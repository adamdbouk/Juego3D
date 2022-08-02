#include "gui.h"
#include "camera.h"
#include "game.h"
#include <iostream>
#include <filesystem>

Gui::Gui(Shader* shader, Texture* atlas) {
	this->shader = shader;
	this->atlas = atlas;
	this->worldPage = 0;
	//init background
	background = new Button(Vector2(400, 300), Vector4(0, 0, 1, 1), 800, 600, false);
	tutorial = new Button(Vector2(400, 300), Vector4(0, 0, 1, 1), 800, 600, false);
	initAtlas();
	initIntroButtons();
	initPauseButtons();
	initWorldButtons();
	initEndButtons();
	initTurnPageButtons();
}

void Gui::initAtlas() {
	float aWidth = atlas->width;
	float aHeight = atlas->height;
	atlasRanges[0] = Vector4(0, 0, 640 / aWidth, 160 / aHeight); //greenButton
	atlasRanges[1] = Vector4(0, (160 + 16) / aHeight, 640 / aWidth, 208 / aHeight); //greenButtonPressed
	atlasRanges[2] = Vector4(1 - 80 / aWidth, 0, 80 / aWidth, 80 / aHeight); //crosshair
	atlasRanges[3] = Vector4(0, (160 + 16 + 208 + 16) / aHeight, 640 / aWidth, 160 / aHeight); //fleshButton
	atlasRanges[4] = Vector4(0, (160 + 16 + 208 + 16 + 160 + 16) / aHeight, 640 / aWidth, 208 / aHeight); //fleshButtonPressed
	atlasRanges[5] = Vector4((640 + 16) / aWidth, 0, 144 / aWidth, 112 / aHeight); //turnPageRight
	atlasRanges[6] = Vector4((640 + 16) / aWidth, (112 + 16) / aHeight, 144 / aWidth, 112 / aHeight); //turnPageLeft
	atlasRanges[7] = Vector4(0, (160 + 16 + 208 + 16 + 160 + 16 + 208 + 16) / aHeight, 640 / aWidth, 64 / aHeight); //brownButton
	atlasRanges[8] = Vector4((640 + 16) / aWidth, (112 + 16 + 112 + 16) / aHeight, 144 / aWidth, 112 / aHeight); //turnLeftRed
}

void Gui::initIntroButtons() {
	Game* game = Game::instance;
	float button_width = 320;
	float button_height = 80;
	float buttonOffset = 10;
	float xPos = button_width / 2 + buttonOffset;
	float yPos = game->window_height / (numIntroButtons + 1);
	for (int i = 0; i < numIntroButtons; i++) {
		introButtons[i] = new Button(Vector2(xPos, yPos * (i + 1)), atlasRanges[greenButton], button_width, button_height, false);
	}
}

void Gui::initPauseButtons() {
	Game* game = Game::instance;
	float button_width = 320;
	float button_height = 80;
	float buttonOffset = 10;
	float xPos = button_width / 2 + buttonOffset;
	float yPos = game->window_height / (numPauseButtons + 1);
	for (int i = 0; i < numPauseButtons; i++) {
		pauseButtons[i] = new Button(Vector2(xPos, yPos * (i + 1)), atlasRanges[fleshButton], button_width, button_height, false);
	}
}

void Gui::initWorldButtons() {
	Game* game = Game::instance;
	float button_width = 640;
	float button_height = 64;
	float buttonOffset = 100;
	float xPos = buttonOffset + button_width / 2 - 10;
	float yPos = buttonOffset;
	for (int i = 0; i < numSaves; i++) {
		worldSavesButtons[i] = new Button(Vector2(xPos, (yPos * (i + 1)) - 30), atlasRanges[brownButton], button_width, button_height, false);
	}
}

void Gui::initEndButtons() {
	Game* game = Game::instance;
	float button_width = 320;
	float button_height = 80;
	float xPos = game->window_width / 2;
	float yPos = game->window_height / (numEndButtons + 1);
	endButtons[0] = new Button(Vector2(400, 300), Vector4(0, 0, 1, 1), 800, 600, false); //Background
	for (int i = 1; i < numEndButtons; i++) {
		endButtons[i] = new Button(Vector2(xPos, yPos * (i + 1)), atlasRanges[fleshButton], button_width, button_height, false);
	}
}

void Gui::initTurnPageButtons() {
	Game* game = Game::instance;
	float button_width = 72;
	float button_height = 56;
	float buttonOffset = 10;
	float xPosRight = game->window_width - buttonOffset - button_width / 2;
	float yPosRight = game->window_height - buttonOffset - button_height / 2;
	float xPosLeft = buttonOffset + button_width / 2;
	float yPosLeft = game->window_height - buttonOffset - button_height / 2;;
	turnPageRightButton = new Button(Vector2(xPosRight, yPosRight), atlasRanges[turnPageRight], button_width, button_height, false);
	turnPageLeftButton = new Button(Vector2(xPosLeft, yPosLeft), atlasRanges[turnPageLeft], button_width, button_height, false);
	float xPosBack = game->window_width / 2 - button_width / 2;
	float yPosBack = game->window_height - buttonOffset - button_height / 2;
	turnBackLeftRedButton = new Button(Vector2(xPosBack, yPosBack), atlasRanges[trunLeftRed], button_width, button_height, false);
}

void Gui::initEntries() {
	std::string path = "save/";
	std::vector<std::string> entriesTemp;
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		std::string name = entry.path().u8string();
		name = name.substr(5, name.size() - 9); //9 = 5 de save/ y 4 de .dat
		entriesTemp.push_back(name);
	}
	entries = entriesTemp;
}

void Gui::RenderIntroGui()
{
	Game* game = Game::instance;
	Camera cam2D;
	cam2D.setOrthographic(0, game->window_width, game->window_height, 0, -1, 1);

	shader->enable();
	if (shader)
	{
		//upload uniforms
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", cam2D.viewprojection_matrix);
		shader->setUniform("u_texture", Texture::Get("data/Gui/menu.png"), 0);
		shader->setUniform("u_model", Matrix44());
		shader->setUniform("u_texture_tiling", 1.0f);

		shader->setUniform("u_tex_range", background->range);
		background->mesh.render(GL_LINE_STRIP);
		background->mesh.render(GL_TRIANGLES);

		shader->setUniform("u_texture", atlas, 0);
		for (int i = 0; i < numIntroButtons; i++) {
			Button* button = introButtons[i];
			shader->setUniform("u_tex_range", button->range);
			button->mesh.render(GL_LINE_STRIP);
			button->mesh.render(GL_TRIANGLES);
		}
	}
	shader->disable();
	
	float scale = 5.0f;
	float textSize = 7 * scale;
	float yOffset = 14.0f;
	float xOffset = 20;
	std::string text[] = { "play", "editor", "tutorial", "exit" };
	for (int i = 0; i < numIntroButtons; i++) {
		float yPos = introButtons[i]->pos.y;
		drawText(xOffset, yPos - yOffset - textSize/2, text[i], Vector3(1, 1, 1), scale);
	}
}

void Gui::RenderWorldsGui() {
	//Load filenames from save
	initEntries();
	//Render buttons
	Game* game = Game::instance;
	Camera cam2D;
	cam2D.setOrthographic(0, game->window_width, game->window_height, 0, -1, 1);

	shader->enable();
	if (shader)
	{
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", cam2D.viewprojection_matrix);
		shader->setUniform("u_texture", atlas, 0);
		shader->setUniform("u_model", Matrix44());
		shader->setUniform("u_texture_tiling", 1.0f);

		shader->setUniform("u_tex_range", turnBackLeftRedButton->range);
		turnBackLeftRedButton->mesh.render(GL_LINE_STRIP);
		turnBackLeftRedButton->mesh.render(GL_TRIANGLES);

		for (int i = 0; i < numSaves; i++) {
			if ((worldPage) * numSaves + i < entries.size()) {
				Button* button = worldSavesButtons[i];
				shader->setUniform("u_tex_range", button->range);
				button->mesh.render(GL_LINE_STRIP);
				button->mesh.render(GL_TRIANGLES);
			}
		}
		if ((worldPage + 1) * numSaves < entries.size() - 1) {
			shader->setUniform("u_tex_range", turnPageRightButton->range);
			turnPageRightButton->mesh.render(GL_LINE_STRIP);
			turnPageRightButton->mesh.render(GL_TRIANGLES);
		}
		if (worldPage > 0) {
			shader->setUniform("u_tex_range", turnPageLeftButton->range);
			turnPageLeftButton->mesh.render(GL_LINE_STRIP);
			turnPageLeftButton->mesh.render(GL_TRIANGLES);
		}
	}
	shader->disable();

	//Render saves
	int pagePos = numSaves * worldPage;
	for (int i = pagePos; i < pagePos + numSaves; i++) {
		if (i > entries.size() - 1) break;
		drawText(100, (100 * (i - pagePos)) + 50, entries[i], Vector3(1, 1, 1), 5);
	}
}

void Gui::RenderPlayGui() {
	Game* game = Game::instance;
	Manager* manager = game->gameManager;
	Player* player = game->currentWorld->player;
	Weapon* weapon = game->currentWorld->weapons[game->currentWorld->currentWeapon];
	drawText(10, game->window_height - 40, std::to_string(manager->round), Vector3(1, 1, 1), 5);
	drawText(10, 20, std::to_string(manager->zombiesAlive) + "/" + std::to_string(manager->zombiesPerRound), Vector3(1, 1, 1), 5);
	drawText(game->window_width / 2, game->window_height - 40, std::to_string(player->vida), Vector3(1, 1, 1), 5);
	drawText(game->window_width - 80, game->window_height - 40, std::to_string(weapon->cargador), Vector3(1, 1, 1), 5);
}

void Gui::RenderCrosshair(){
	Game* game = Game::instance;
	Camera cam2D;
	cam2D.setOrthographic(0, game->window_width, game->window_height, 0, -1, 1);

	Mesh quad;
	quad.createQuad(game->window_width/2, game->window_height/2, 10, 10, false);

	shader->enable();
	if (shader)
	{
		//upload uniforms
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", cam2D.viewprojection_matrix);
		shader->setUniform("u_tex_range", atlasRanges[crosshair]);
		shader->setUniform("u_texture", atlas, 0);
		shader->setUniform("u_model", Matrix44());
		shader->setUniform("u_texture_tiling", 1.0f);
		quad.render(GL_LINE_STRIP);
	}
	//hacer draw call
	quad.render(GL_TRIANGLES);

	shader->disable();
}

void Gui::RenderPauseMenu() {
	Game* game = Game::instance;
	Camera cam2D;
	cam2D.setOrthographic(0, game->window_width, game->window_height, 0, -1, 1);

	shader->enable();
	if (shader)
	{
		//upload uniforms
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", cam2D.viewprojection_matrix);
		shader->setUniform("u_texture", atlas, 0);
		shader->setUniform("u_model", Matrix44());
		shader->setUniform("u_texture_tiling", 1.0f);

		for (int i = 0; i < numPauseButtons; i++) {
			Button* button = pauseButtons[i];
			shader->setUniform("u_tex_range", button->range);
			button->mesh.render(GL_LINE_STRIP);
			button->mesh.render(GL_TRIANGLES);
		}
	}
	shader->disable();

	float scale = 5.0f;
	float textSize = 7 * scale;
	float yOffset = 14.0f;
	float xOffset = 20;
	std::string text[] = { "resume", "tutorial", "main menu" };
	for (int i = 0; i < numPauseButtons; i++) {
		float yPos = pauseButtons[i]->pos.y;
		drawText(xOffset, yPos - yOffset - textSize / 2, text[i], Vector3(1, 1, 1), scale);
	}
}

void Gui::RenderEndGui() {
	//Load filenames from save
	initEntries();
	//Render buttons
	Game* game = Game::instance;
	Camera cam2D;
	cam2D.setOrthographic(0, game->window_width, game->window_height, 0, -1, 1);

	shader->enable();
	if (shader)
	{
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", cam2D.viewprojection_matrix);
		shader->setUniform("u_texture", Texture::Get("data/Gui/GameOver.png"), 0);
		shader->setUniform("u_model", Matrix44());
		shader->setUniform("u_texture_tiling", 1.0f);

		shader->setUniform("u_tex_range", endButtons[0]->range);
		endButtons[0]->mesh.render(GL_LINE_STRIP);
		endButtons[0]->mesh.render(GL_TRIANGLES);

		shader->setUniform("u_texture", atlas, 0);
		for (int i = 1; i < numEndButtons; i++) {
			Button* button = endButtons[i];
			shader->setUniform("u_tex_range", button->range);
			button->mesh.render(GL_LINE_STRIP);
			button->mesh.render(GL_TRIANGLES);
		}
	}
	shader->disable();

	float scale = 5.0f;
	float textSize = 7 * scale;
	float yOffset = 14.0f;
	float xOffset = 20;
	std::string text[] = { "restart", "main menu" };
	for (int i = 1; i < numEndButtons; i++) {
		float yPos = endButtons[i]->pos.y;
		drawText(game->window_width / 2 - 160 + xOffset, yPos - yOffset - textSize / 2, text[i - 1], Vector3(1, 1, 1), scale);
	}
}

void Gui::RenderTutorial() {
	Game* game = Game::instance;
	Camera cam2D;
	cam2D.setOrthographic(0, game->window_width, game->window_height, 0, -1, 1);

	shader->enable();
	if (shader)
	{
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", cam2D.viewprojection_matrix);
		shader->setUniform("u_texture", Texture::Get("data/Gui/tutorial.png"), 0);
		shader->setUniform("u_model", Matrix44());
		shader->setUniform("u_texture_tiling", 1.0f);

		shader->setUniform("u_tex_range", tutorial->range);
		tutorial->mesh.render(GL_LINE_STRIP);
		tutorial->mesh.render(GL_TRIANGLES);
	}
	shader->disable();
}

void Gui::introButtonPressed(Vector2 pos) {
	Game* game = Game::instance;
	int i = 0;
	for (i; i < numIntroButtons; i++) {
		Button* button = introButtons[i];
		int min_x = button->pos.x - (button->width / 2);
		int min_y = button->pos.y - (button->height / 2);
		int max_x = button->pos.x + (button->width / 2);
		int max_y = button->pos.y + (button->height / 2);
		if (pos.x > min_x && pos.y > min_y && pos.x < max_x && pos.y < max_y) break;
		if (i == numIntroButtons - 1) i++;
	}
	if (i == 0) {
		initEntries();
		game->setSelectWorldStage();
	}
	if (i == 1) game->setEditorStage();
	if (i == 2) {
		game->tutorial = true; 
		game->mouse_locked = false;
	}
	if (i == 3) game->must_exit = true;
}

int Gui::worldButtonPressed(Vector2 pos) {
	Game* game = Game::instance;
	int i = 0;
	for (i; i < numSaves; i++) {
		Button* button = worldSavesButtons[i];
		int min_x = button->pos.x - (button->width / 2);
		int min_y = button->pos.y - (button->height / 2);
		int max_x = button->pos.x + (button->width / 2);
		int max_y = button->pos.y + (button->height / 2);
		if (pos.x > min_x && pos.y > min_y && pos.x < max_x && pos.y < max_y) return i;
	}
	return -1;
}

void Gui::changePageButtonPressed(Vector2 pos) {
	if ((worldPage + 1) * numSaves < entries.size() - 1) {
		int minRight_x = turnPageRightButton->pos.x - (turnPageRightButton->width / 2);
		int minRight_y = turnPageRightButton->pos.y - (turnPageRightButton->height / 2);
		int maxRight_x = turnPageRightButton->pos.x + (turnPageRightButton->width / 2);
		int maxRight_y = turnPageRightButton->pos.y + (turnPageRightButton->height / 2);
		if (pos.x > minRight_x && pos.y > minRight_y && pos.x < maxRight_x && pos.y < maxRight_y) worldPage++;
	}
	if (worldPage > 0) {
		int minLeft_x = turnPageLeftButton->pos.x - (turnPageLeftButton->width / 2);
		int minLeft_y = turnPageLeftButton->pos.y - (turnPageLeftButton->height / 2);
		int maxLeft_x = turnPageLeftButton->pos.x + (turnPageLeftButton->width / 2);
		int maxLeft_y = turnPageLeftButton->pos.y + (turnPageLeftButton->height / 2);
		if (pos.x > minLeft_x && pos.y > minLeft_y && pos.x < maxLeft_x && pos.y < maxLeft_y) worldPage--;
	}
	//Turn Back
	int minLeft_x = turnBackLeftRedButton->pos.x - (turnBackLeftRedButton->width / 2);
	int minLeft_y = turnBackLeftRedButton->pos.y - (turnBackLeftRedButton->height / 2);
	int maxLeft_x = turnBackLeftRedButton->pos.x + (turnBackLeftRedButton->width / 2);
	int maxLeft_y = turnBackLeftRedButton->pos.y + (turnBackLeftRedButton->height / 2);
	if (pos.x > minLeft_x && pos.y > minLeft_y && pos.x < maxLeft_x && pos.y < maxLeft_y) Game::instance->setIntroStage();
}

void Gui::pauseButtonPressed(Vector2 pos) {
	Game* game = Game::instance;
	int i = 0;
	for (i; i < numPauseButtons; i++) {
		Button* button = pauseButtons[i];
		int min_x = button->pos.x - (button->width / 2);
		int min_y = button->pos.y - (button->height / 2);
		int max_x = button->pos.x + (button->width / 2);
		int max_y = button->pos.y + (button->height / 2);
		if (pos.x > min_x && pos.y > min_y && pos.x < max_x && pos.y < max_y) break;
		if (i == numIntroButtons - 1) i++;
	}
	if (i == 0) {
		game->mouse_locked = !game->mouse_locked;
		SDL_ShowCursor(!game->mouse_locked);
	}
	if (i == 1) {
		game->tutorial = true;
		game->mouse_locked = false;
	}
	if (i == 2) game->setIntroStage();
}

void Gui::endButtonPressed(Vector2 pos) {
	Game* game = Game::instance;
	int i = 1;
	for (i; i < numEndButtons; i++) {
		Button* button = endButtons[i];
		int min_x = button->pos.x - (button->width / 2);
		int min_y = button->pos.y - (button->height / 2);
		int max_x = button->pos.x + (button->width / 2);
		int max_y = button->pos.y + (button->height / 2);
		if (pos.x > min_x && pos.y > min_y && pos.x < max_x && pos.y < max_y) break;
		if (i == numIntroButtons - 1) i++;
	}
	if (i == 1) game->setSelectWorldStage();
	if (i == 2) game->setIntroStage();
}
