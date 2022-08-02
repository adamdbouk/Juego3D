#include "world.h"
#include "game.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>
using std::cerr;
using std::endl;
using std::cout;
using std::ofstream;
using std::ifstream;

World::World(Shader* shader) {
	for (int i = 0; i < MAX_ENTITIES; i++) {
		staticEntities[i] = NULL;
	}
	for (int i = 0; i < MAX_ENTITIES; i++) {
		dynamicEntities[i] = NULL;
	}
	for (int i = 0; i < MAX_ZOMBIES; i++) {
		zombies[i] = NULL;
	}
	for (int i = 0; i < MAX_ENTITIES; i++) {
		structures[i] = NULL;
	}
	for (int i = 0; i < MAX_ENTITIES; i++) {
		decoration[i] = NULL;
	}
	for (int i = 0; i < MAX_WEAPONS; i++) {
		weapons[i] = NULL;
	}
	for (int i = 0; i < MAX_SPAWNERS; i++) {
		spawners[i] = NULL;
	}
	this->shader = shader;
	this->player = (Player*) new Entity(0, 0, 0, Matrix44());
	this->ground = NULL;
	this->sky = NULL;
	for (int i = 0; i < CUADRANTS; i++) this->maps[i] = new uint8[100 * 100]; //Mapwidth * Mapheight
}

//ADDITION OF ENTITIES IN THEIR CORRESPONDING LIST
void World::addStaticEntity(Entity* entity) {
	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (staticEntities[i] == NULL){
			staticEntities[i] = entity;
			break;
		}
	}
}

void World::addDynamicEntity(Entity* entity) {
	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (dynamicEntities[i] == NULL) {
			dynamicEntities[i] = entity;
			break;
		}
	}
}

void World::addSpawnSpawner(ZombieSpawner* spawner)
{
	for (int i = 0; i < MAX_SPAWNERS; i++) {
		if (spawners[i] == NULL) {
			spawners[i] = spawner;
			break;
		}
	}
}

void World::addZombie(Zombie* zombie) {
	for (int i = 0; i < MAX_ZOMBIES; i++) {
		if (zombies[i] == NULL) {
			zombies[i] = zombie;
			break;
		}
	}
}

void World::addPlayer(Player* player) {
	this->player = player;
}

void World::addWeapon(Weapon* weapon) {
	for (int i = 0; i < MAX_WEAPONS; i++) {
		if (weapons[i] == NULL) {
			weapons[i] = weapon;
			break;
		}
	}
}

void World::addStructure(Entity* entity) {
	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (structures[i] == NULL) {
			structures[i] = entity;
			break;
		}
	}
}

void World::addDecoration(Entity* entity)
{
	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (decoration[i] == NULL) {
			decoration[i] = entity;
			break;
		}
	}
}

void World::addSpawns(ZombieSpawner* spawn)
{
	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (spawnsEditor[i] == NULL) {
			spawnsEditor[i] = spawn;
			break;
		}
	}
}

void World::addObjectEditor(Entity* entity, Vector3 dir) {
	Camera* camera = Camera::current;
	Vector3 origin = camera->eye;
	Vector3 pos = RayPlaneCollision(Vector3(0, 0, 0), Vector3(0, 1, 0), origin, dir);
	Matrix44 m;
	Entity* copia = new Entity(0, 0, 0, m);
	copia->copy(entity);
	copia->m.setTranslation(pos.x, pos.y, pos.z);
	copia->m.rotate(entity->yaw * DEG2RAD, Vector3(0,1,0));
	if (typeObject == 0) {
		addStaticEntity(copia);
	}
	else if (typeObject == 1) {
		addDynamicEntity(copia);
	}
	else {
		ZombieSpawner* copia = new ZombieSpawner(0, 0, 0, m);
		copia->copy(entity);
		copia->m.setTranslation(pos.x, pos.y, pos.z);
		copia->m.rotate(entity->yaw * DEG2RAD, Vector3(0, 1, 0));
		addSpawnSpawner(copia);
	}
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//INIT BASIC THINGS OF THE WORLD
void World::initCamera(Camera* camera) {
	Camera::current = camera;
	int window_width = 800;
	int window_height = 600;
	camera->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective
	Vector3 eye = player->m * Vector3(0.f, 1.f, 0.f);
	Vector3 center = player->m * Vector3(0.f, 5.f, -1.f);
	Vector3 up = player->m.rotateVector(Vector3(0.f, 1.f, 0.f));
	camera->lookAt(eye, center, up);
}

void World::initPlayer(Vector3 pos, Mesh* mesh, Texture* texture) {
	Matrix44 m;
	m.setTranslation(pos.x, pos.y, pos.z);
	Player* player = new Player(pos.x, pos.y, pos.z, m);
	player->mesh = mesh;
	player->texture = texture;

	addPlayer(player);
}

void World::initSky(Mesh* mesh, Texture* texture) {
	Matrix44 m;
	Vector3 playerPos = player->getPos();
	m.setTranslation(playerPos.x, playerPos.y, playerPos.z);
	sky = new Entity(playerPos.x, playerPos.y, playerPos.z, m);
	sky->mesh = mesh;
	sky->texture = texture;
}

void World::initGround(Texture* texture) {
	Matrix44 m;
	m.setTranslation(0, -2, 0);
	ground = new Entity(0, -2, 0, m);
	ground->mesh = new Mesh();
	ground->mesh->createPlane(2000);
	ground->texture = texture;

}

void World::initWeapons() {
	Matrix44 m;
	m.setTranslation(0, 0, 0);
	Weapon* AK47 = (Weapon*) new Entity(0, 0.5, 0, m);
	AK47->loadMesh("data/Assets/Weapons/AK47.obj");
	AK47->loadTexture("data/Assets/Weapons/AK47.png");
	AK47->init(10, 30, 1);
	addWeapon(AK47);
}

void World::initMap() {
	int Mapwidth = 100;
	int Mapheight = 100;

	for (int j = 0; j < Mapwidth * Mapheight; j++) {
		maps[0][j] = 1;
		maps[1][j] = 1;
		maps[2][j] = 1;
		maps[3][j] = 1;
	}
	//Intentos para crear el map
	//for (int i = 0; i < CUADRANTS; i++) {
	//	uint8* currentMap = maps[i];
	//	for (int j = 0; j < Mapwidth * Mapheight; j++) {//Recorrer mapa
	//		for (int k = 0; k < MAX_ENTITIES; k++) {	//Recorrer entidades
	//			if (staticEntities[k] != NULL) {
	//				Entity* current = staticEntities[k];
	//				Vector3 coll;
	//				Vector3 collNormal;
	//				Vector3 pos;
	//				if (i == 0) Vector3 pos = Vector3(j % Mapwidth, 0.1, j / Mapwidth);		//AllPositive
	//				else if (i == 1) Vector3 pos = Vector3(j % Mapwidth, 0.1, -(j / Mapwidth));	//xPositive
	//				else if (i == 2) Vector3 pos = Vector3(-(j % Mapwidth), 0.1, j / Mapwidth);	//zPositive
	//				else if (i == 3) Vector3 pos = Vector3(-(j % Mapwidth), 0.1, -(j / Mapwidth));//AllNegative

	//				if (current->mesh != NULL) {
	//					if (current->pos.x <= pos.x && current->pos.x >= pos.x - 1 && current->pos.z <= pos.z && current->pos.z >= pos.z - 1) 
	//						currentMap[j] = 0;
	//					//if (current->mesh->testSphereCollision(current->m, pos, 0.5, coll, collNormal)) currentMap[j] = 0;
	//					else currentMap[j] = 1;
	//				}
	//			}
	//			else break;
	//		}
	//	}
	//	maps[i] = currentMap;
	//}
	//El otro intento
	/*for (int i = 0; i < MAX_ENTITIES; i++) {
		Entity* current = staticEntities[i];
		if (current == NULL) break;
		int index = (floor(abs(current->pos.z)) * Mapwidth) + floor(abs(current->pos.x));
		if (current->pos.x >= 0 && current->pos.z >= 0) 
			maps[0][index] = 0;
		else if (current->pos.x >= 0 && current->pos.z <= 0) 
			maps[1][index] = 0;
		else if (current->pos.x <= 0 && current->pos.z >= 0) 
			maps[2][index] = 0;
		else if (current->pos.x <= 0 && current->pos.z <= 0) 
			maps[3][index] = 0;
	}*/
}

void World::initZombies() {
	Matrix44 m;
	m.setTranslation(0, 1000, 0);
	for (int i = 0; i < MAX_ZOMBIES; i++) {
		Zombie* zombie = new Zombie(0, 0, 0, m);
		zombies[i] = zombie;
	}
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void World::loadDecoration() {
	//Load filenames from save
	std::string path = "data/Assets/Decoration/";
	std::vector<std::string> deco;
	std::string mbin = ".mbin";
	int numElements = 0;
	for (const auto& object : std::filesystem::directory_iterator(path)) {
		std::string name = object.path().u8string();

		std::string ext = name.substr((name.size() - 5), 5);
		//std::cerr << ext << std::endl;;
		if (ext == mbin) continue;
		deco.push_back(name);
		numElements++;
		//std::cerr << name << std::endl;;
	}
	Matrix44 m;
	for (int i = 0; i < numElements - 1; i += 2) {
		Entity* entity = new Entity(0, 0, 0, m);
		const char* meshObject = deco[i].c_str();
		entity->loadMesh(meshObject);

		const char* textureObject = deco[i + 1].c_str();
		entity->loadTexture(textureObject);

		std::string tipo = deco[i].substr(23, deco[i].size() - 27);
		entity->type = tipo;
		//std::cerr << tipo << std::endl;;
		addDecoration(entity);
	}
	maxDecoration = (deco.size() / 2) - 1;
}

void World::loadStructure() {
	//Load filenames from save
	std::string path = "data/Assets/Structure/";
	std::string mbin = ".mbin";
	std::vector<std::string> stru;
	int numElements = 0;
	for (const auto& object : std::filesystem::directory_iterator(path)) {
		std::string name = object.path().u8string();
		std::string ext = name.substr((name.size() - 5), 5);
		//std::cerr << ext << std::endl;;
		if (ext == mbin) continue;
		stru.push_back(name);
		numElements++;
		//std::cerr << name << std::endl;;
	}
	Matrix44 m;
	for (int i = 0; i < numElements - 1; i += 2) {
		Entity* entity = new Entity(0, 0, 0, m);
		const char* meshObject = stru[i].c_str();
		entity->loadMesh(meshObject);

		const char* textureObject = stru[i + 1].c_str();
		entity->loadTexture(textureObject);

		std::string tipo = stru[i].substr(22, stru[i].size() - 26);
		entity->type = tipo;
		//std::cerr << tipo << std::endl;;
		addStructure(entity);
	}
	maxStructure = (stru.size() / 2) - 1;
}

void World::loadSpawns()
{
	for (int i = 0; i < MAX_SPAWNERS; i++) {
		Matrix44 m;
		m.setTranslation(0, 0, 0);
		ZombieSpawner* spawner = new ZombieSpawner(0, 0, 0, m);
		spawnsEditor[i] = spawner;
	}
}

//BASIC FUNCTIONALITIES OF EACH WORLD, FOR PLAYSTAGE AND ALSO EDITOR STAGE
void World::disparar() {
	Game* game = Game::instance;
	Camera* camera = Camera::current;
	Weapon* weapon = weapons[currentWeapon];
	Vector3 colPoint;
	Vector3 colNormal;
	Vector3 origin = camera->eye;
	Vector3 dir = camera->getRayDirection(Input::mouse_position.x, Input::mouse_position.y, 800, 600);
	float maxDistance = 100;
	for (int i = 0; i < MAX_ZOMBIES; i++) {
		Zombie* zombie = zombies[i];
		if (zombie == NULL)	break;
		if (zombie->vida <= 0) continue;
		Matrix44 scaled = zombie->m;
		scaled.scale(0.01, 0.01, 0.01);
		if (zombie->mesh->testRayCollision(scaled, origin, dir, colPoint, colNormal, maxDistance, false)) {
			if (weapon->lastShot + weapon->cadencia < game->time) {
				weapon->lastShot = game->time;
				weapon->cargador--;
				if (weapon->cargador < 0) weapon->cargador = 30;
				zombie->vida -= weapons[currentWeapon]->damage;
				if (zombie->vida <= 0) Game::instance->gameManager->zombiesAlive--;
				game->disparo->channelSample = *game->disparo->Play("data/Audio/disparoAK47.mp3");
				break;
			}
		}
	}
}

void World::selectEntityEditor(Vector3 dir){
	Camera* camera = Camera::current;
	Vector3 origin = camera->eye;
	
	Entity* rayEntity = new Entity(100000000000, 1000000000, 1000000000000, Matrix44());
	for (int i = 0; i < MAX_ENTITIES; i++)
	{
		Vector3 col;
		Vector3 normal;

		if (typeObject == 0) {
			Entity* current = staticEntities[i];
			if (current == NULL) break;
			if (!current->mesh->testRayCollision(current->m, origin, dir, col, normal, 10000)) continue;
			Vector3 pos = current->m.getTranslation();
			float catetoX = camera->eye.x - rayEntity->pos.x;
			float catetoZ = camera->eye.z - rayEntity->pos.z;
			float catetoY = camera->eye.y - rayEntity->pos.y;
			float rayCamera = sqrt(pow(catetoX, 2) + pow(catetoZ, 2));

			catetoX = camera->eye.x - pos.x;
			catetoZ = camera->eye.z - pos.z;
			catetoY = camera->eye.y - pos.y;
			float currentCamera = sqrt(pow(catetoX, 2) + pow(catetoZ, 2));
			if (currentCamera < rayCamera) {
				rayEntity = current;
			}
			
		}
		else if(typeObject == 1){
			Entity* current = dynamicEntities[i];
			if (current == NULL) break;
			if (!current->mesh->testRayCollision(current->m, origin, dir, col, normal, 10000)) continue;
			Vector3 pos = current->m.getTranslation();
			float catetoX = camera->eye.x - rayEntity->pos.x;
			float catetoZ = camera->eye.z - rayEntity->pos.z;
			float catetoY = camera->eye.y - rayEntity->pos.y;
			float rayCamera = sqrt(pow(catetoX, 2) + pow(catetoZ, 2));

			catetoX = camera->eye.x - pos.x;
			catetoZ = camera->eye.z - pos.z;
			catetoY = camera->eye.y - pos.y;
			float currentCamera = sqrt(pow(catetoX, 2) + pow(catetoZ, 2));
			if (currentCamera < rayCamera) {
				rayEntity = current;
			}
		}
		
	}
	if (selectedEntity != NULL) selectedEntity->bounding = false;
	rayEntity->bounding = true;
	selectedEntity = rayEntity;
}

void World::deleteEntity(Entity* entity) {
	for (int i = 0; i < MAX_ENTITIES; i++) {
		if (staticEntities[i] == entity) staticEntities[i] = NULL; break;
		if (dynamicEntities[i] == entity) dynamicEntities[i] = NULL; break;
	}
}

void World::moveZombies() {
	for (int i = 0; i < MAX_ZOMBIES; i++) {
		Zombie* zombie = zombies[i];
		if (zombie == NULL || zombie->vida <= 0) continue;
		Vector3 target;
		target = zombie->AStarPath(player->pos, maps);
		zombie->move(target);

	}
}

void World::collisionPlayerZombie() {
	Vector3 playerCenter = player->pos + Vector3(0, 1, 0);
	int hitCooldown = 1;
	for (int i = 0; i < MAX_ZOMBIES; i++) {
		Zombie* zombie = zombies[i];
		if (zombie == NULL || zombie->vida <= 0) continue;
		Vector3 coll;
		Vector3 collNormal;
		if (zombie->mesh != NULL) {
			//Matrix44 scaled = zombie->m;
			//scaled.scale(0.01, 0.01, 0.01);
			if (zombie->mesh->testSphereCollision(zombie->m, playerCenter, 0.5, coll, collNormal)) {
				/*std::cerr << zombie->m.getTranslation().x << " Z: " << zombie->m.getTranslation().z << std::endl;
				std::cerr << player->m.getTranslation().x << " Z: " << player->m.getTranslation().z << std::endl;*/
				if (player->lastHit + hitCooldown < Game::instance->time) {
					player->lastHit = Game::instance->time;
					player->vida -= 1;
				}
			}
		}
	}
}

void World::spawnZombies(int num, int vida, float vel, float time) {
	for (int i = 0; i < num; i++) {
		Zombie* zombie = zombies[i];
		if (zombie->vida <= 0) {
			for (int j = 0; j < MAX_SPAWNERS; j++) {
				ZombieSpawner* zombieSpawner = spawners[j];
				//Calcular distancia?
				if (zombieSpawner != NULL && zombieSpawner->ultimoSpawn + zombieSpawner->cooldown < time) {
					zombieSpawner->spawnZombie(zombie, time);
					Game::instance->gameManager->spawnedZombies++;
					zombie->vida = vida;
					zombie->vel = vel;
					break;
				}
			}
		}
	}
}

int World::zombiesAlive()
{
	int numZombies = 0;
	for (int i = 0; i < MAX_ZOMBIES; i++) {
		if (zombies[i]->vida > 0) numZombies++;
	}
	return numZombies;
}
//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::...
//RENDERS OF EVERYTHING NECESSARY
void World::RenderStatic(Camera* camera)
{
	for (int i = 0; i < MAX_ENTITIES; i++) {
		Entity* entity = staticEntities[i];
		if (entity == NULL) {
			break;
		}
		if (entity->mesh != NULL) {

		BoundingBox currentBox = transformBoundingBox(entity->m, entity->mesh->box);
		if (!camera->testBoxInFrustum(currentBox.center, currentBox.halfsize)) continue;
		entity->render(shader);
		}

	}
}

void World::RenderDynamic(Camera* camera)
{
	for (int i = 0; i < MAX_ENTITIES; i++) {
		Entity* entity = dynamicEntities[i];
		if (entity == NULL) {
			break;
		}
		BoundingBox currentBox = transformBoundingBox(entity->m, entity->mesh->box);
		if (!camera->testBoxInFrustum(currentBox.center, currentBox.halfsize)) continue;
		entity->render(shader);

	}
}

void World::RenderZombies(Camera* camera, float time) {
	for (int i = 0; i < MAX_ZOMBIES; i++) {
		Zombie* zombie = zombies[i];
		if (zombie == NULL) {
			break;
		}
		if (zombie->vida > 0) {
			Matrix44 scaled = zombie->m;
			scaled.scale(0.01, 0.01, 0.01);
			BoundingBox currentBox = transformBoundingBox(scaled, zombie->mesh->box);
			//if (!camera->testSphereInFrustum(currentBox.center, zombie->mesh->radius)) continue;
			zombie->renderAnimation(time);
			//zombie->render(shader);
		}
	}
}

void World::RenderPlayer(Camera* camera) {
	player->render(shader);
	weapons[0]->renderWeapon(player, shader); //CAMBIAR EL [0] AL ARMA QUE TENGA SELECCIONADA
}

void World::RenderSpawns(Camera* camera) {
	for (int i = 0; i < MAX_SPAWNERS; i++) {
		ZombieSpawner* spawner = spawners[i];
		if (spawner == NULL) continue;
		BoundingBox currentBox = transformBoundingBox(spawner->m, spawner->mesh->box);
		if (!camera->testBoxInFrustum(currentBox.center, currentBox.halfsize)) continue;
		spawner->render(shader);
	}
}

void World::RenderBoundingStatic(Camera* camera)
{
	for (int i = 0; i < MAX_ENTITIES; i++) {
		Entity* entity = staticEntities[i];
		if (entity == NULL || entity->mesh == NULL) break;
		BoundingBox currentBox = transformBoundingBox(entity->m, entity->mesh->box);
		if (!camera->testBoxInFrustum(currentBox.center, currentBox.halfsize)) continue;
		if (entity->bounding)entity->mesh->renderBounding(entity->m);
	}
}

void World::RenderBoundingDynamic(Camera* camera)
{
	for (int i = 0; i < MAX_ENTITIES; i++) {
		Entity* entity = dynamicEntities[i];
		if (entity == NULL || entity->mesh == NULL) break;
		BoundingBox currentBox = transformBoundingBox(entity->m, entity->mesh->box);
		if (!camera->testBoxInFrustum(currentBox.center, currentBox.halfsize)) continue;
		if (entity->bounding)entity->mesh->renderBounding(entity->m);
	}
}

void World::RenderBoundingZombies(Camera* camera)
{
	for (int i = 0; i < MAX_ZOMBIES; i++) {
		Zombie* zombie = zombies[i];
		if (zombie == NULL) break;
		Matrix44 scaled = zombie->m;
		scaled.scale(0.01, 0.01, 0.01);
		BoundingBox currentBox = transformBoundingBox(scaled, zombie->mesh->box);
		if (!camera->testBoxInFrustum(currentBox.center, currentBox.halfsize)) continue;
		zombie->mesh->renderBounding(scaled);
	}
}
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
void World::saveWorldInfo(std::string filename) {
	ofstream outdata;
	outdata.open("save/" + filename + ".dat");
	if (!outdata) {
		cerr << "Error: file could not be opened" << endl;
		exit(1);
	}
	cerr << "Saving world" << endl;
	for (size_t i = 0; i < MAX_ENTITIES; i++)
	{
		if (staticEntities[i] != NULL) {
			Matrix44 mat = staticEntities[i]->m;
			for (int j = 0; j < 16; j++) {
				outdata << mat.m[j] << " ";
			}
			outdata << endl;
			outdata << staticEntities[i]->mesh->name << endl;
			outdata << staticEntities[i]->texture->filename << endl;
			outdata << "static" << endl;
		}
		else {
			break;
		}
	}
	for (size_t i = 0; i < MAX_ENTITIES; i++)
	{
		if (dynamicEntities[i] != NULL) {
			Matrix44 mat = dynamicEntities[i]->m;
			for (int j = 0; j < 16; j++) {
				outdata << mat.m[j] << " ";
			}
			outdata << endl;
			outdata << dynamicEntities[i]->mesh->name << endl;
			outdata << dynamicEntities[i]->texture->filename << endl;
			outdata << "dynamic" << endl;
		}
		else {
			break;
		}
	}
	for (size_t i = 0; i < MAX_SPAWNERS; i++)
	{
		if (spawners[i] != NULL) {
			Matrix44 mat = spawners[i]->m;
			for (int j = 0; j < 16; j++) {
				outdata << mat.m[j] << " ";
			}
			outdata << endl;
			outdata << spawners[i]->mesh->name << endl;
			outdata << spawners[i]->texture->filename << endl;
			outdata << "spawner" << endl;
		}
		else {
			break;
		}
	}
	cerr << "World saved!" << endl;
	outdata.close();
}

bool World::loadWorldInfo(std::string filename) {
	ifstream indata;
	std::string strMatrix;
	std::string meshName;
	std::string textureName;
	std::string entityType;

	indata.open("save/" + filename + ".dat");
	if (!indata) {
		cerr << "Error tu prima" << endl;
		return false;
	}
	cerr << "Loading world " << filename << endl;
	
	int staticIndex = 0;
	int dynamicIndex = 0;
	int spawnerIndex = 0;
	while (indata.good()) {
		Matrix44 m = Matrix44();
		for (int i = 0; i < 16; i++) {
			indata >> strMatrix;
			m.m[i] = ::atof(strMatrix.c_str());
		}
		Vector3 pos = m.getTranslation();
		indata >> meshName;
		indata >> textureName;
		indata >> entityType;
		if (pos.y == 10000000.0) {
			continue;
		}

		if (entityType == "static") {
			Entity* newEntity = new Entity(pos.x, pos.y, pos.z, m);
			newEntity->loadMesh(meshName.c_str());
			newEntity->loadTexture(textureName.c_str());
			staticEntities[staticIndex] = newEntity;
			staticIndex++;
		}
		else if (entityType == "dynamic") {
			Entity* newEntity = new Entity(pos.x, pos.y, pos.z, m);
			newEntity->loadMesh(meshName.c_str());
			newEntity->loadTexture(textureName.c_str());
			dynamicEntities[dynamicIndex] = newEntity;
			dynamicIndex++;
		}
		else if (entityType == "spawner") {
			ZombieSpawner* spawn = new ZombieSpawner(pos.x, pos.y, pos.z,m);//rellenarlo con lo que necesite
			spawn->loadMesh(meshName.c_str());
			spawn->loadTexture(textureName.c_str());
			spawners[spawnerIndex] = spawn;
			spawnerIndex++;
		}
	}
	cerr << "World loaded!" << endl;
	return true;
}