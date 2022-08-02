#ifndef world_h
#define world_h

#include "utils.h"
#include "shader.h"
#include "entity.h"
#include "camera.h"
#include "input.h"

#define MAX_ENTITIES 1000
#define MAX_ZOMBIES 100
#define MAX_WEAPONS 10
#define CUADRANTS 4
#define MAX_SPAWNERS 10

class World {
public:
	Entity* staticEntities[MAX_ENTITIES];
	Entity* dynamicEntities[MAX_ENTITIES];
	Entity* structures[MAX_ENTITIES];
	Entity* decoration[MAX_ENTITIES];
	int numEntity = 0;
	int maxStructure = 0;
	int numStructure = 0;
	int maxDecoration = 0;
	int numDecoration = 0;
	int maxSpawns = 0;
	int numSpawns = 0;
	Zombie* zombies[MAX_ZOMBIES];
	ZombieSpawner* spawners[MAX_SPAWNERS];
	ZombieSpawner* spawnsEditor[MAX_SPAWNERS];
	Player* player;
	Weapon* weapons[MAX_WEAPONS];
	int currentWeapon = 0;
	Shader* shader;
	Entity* sky;
	Entity* selectedEntity = NULL;
	uint8* maps[CUADRANTS];
	Entity* ground;
	int typeObject = 0;
	int maxTypes = 3;
	World(Shader* shader);
	void addStaticEntity(Entity* entity);
	void addDynamicEntity(Entity* entity);
	void addSpawnSpawner(ZombieSpawner* spawner);
	void addZombie(Zombie* zombie);
	void addPlayer(Player* player);
	void addWeapon(Weapon* weapon);
	void addStructure(Entity* entity);
	void addDecoration(Entity* entity);
	void addSpawns(ZombieSpawner* spawn);
	void addObjectEditor(Entity* entity, Vector3 dir);
	//Inits
	void initCamera(Camera* camera);
	void initPlayer(Vector3 pos, Mesh* mesh, Texture* text);
	void initSky(Mesh* mesh, Texture* texture);
	void initGround(Texture* texture);
	void initMap();
	void initWeapons(); //Utilizarla explota el juego
	void initZombies();
	//Loads
	void loadDecoration();
	void loadStructure();
	void loadSpawns();
	//Otro
	void disparar();
	void selectEntityEditor(Vector3 dir);
	void deleteEntity(Entity* entity);
	int zombiesAlive();
	void spawnZombies(int num, int vida, float vel, float time);
	void moveZombies();
	void collisionPlayerZombie();
	//Renders
	void RenderStatic(Camera* camera);
	void RenderDynamic(Camera* camera);
	void RenderZombies(Camera* camera, float time);
	void RenderPlayer(Camera* camera);
	void RenderSpawns(Camera* camera);
	void RenderBoundingStatic(Camera* camera);
	void RenderBoundingDynamic(Camera* camera);
	void RenderBoundingZombies(Camera* camera);
	//WorldInfos
	void saveWorldInfo(std::string filename);
	bool loadWorldInfo(std::string filename);
};

#endif