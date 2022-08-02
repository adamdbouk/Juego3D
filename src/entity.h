#ifndef Entity_H
#define Entity_H

#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"
#include "animation.h"
#include <string>

class Entity {
public:
	Vector3 pos; //Mirar si es util por culpa del setTranslation ese que dice el feo del Adam
	Matrix44 m;
	Mesh* mesh;
	Texture* texture;
	float yaw;
	float pitch;
	bool bounding = false;
	std::string type;
	
	Entity(float x, float y, float z, Matrix44 m, float yaw = 0, Mesh* mesh = NULL, Texture* texture = NULL, float pitch = 0) {
		this->pos.x = x;
		this->pos.y = y;
		this->pos.z = z;
		this->m = m;
		this->mesh = mesh;
		this->texture = texture;
		this->yaw = yaw;
		this->pitch = pitch;
	};

	Vector3 getPos() {
		return m.getTranslation();
	};

	void loadMesh(const char* filename) {
		mesh = Mesh::Get(filename);
	};
	void loadTexture(const char* filename) {
		texture = Texture::Get(filename);
	};

	void DeleteEntity();
	void render(Shader* shader, float tiling = 1.0f);
	void copy(Entity* entity);
};

class Zombie : public Entity {
public:
	float vel = 0;
	int vida = 0;

	Zombie(float x, float y, float z, Matrix44 m, float yaw = 0, Mesh* mesh = NULL, Texture* texture = NULL, float pitch = 0) : Entity(x, y, z, m, yaw = 0, mesh = NULL, texture = NULL, pitch = 0) {
		loadMesh("data/Zombies/Animation/character.mesh");
		loadTexture("data/Zombies/image.png");
		vel = 0;
		vida = 0;
	};
	Vector3 AStarPath(Vector3 target, uint8** maps);
	void move(Vector3 target);
	void renderAnimation(float time, float tiling = 1.0f);
};

class ZombieSpawner : public Entity {
public:
	float ultimoSpawn;
	int cooldown;

	ZombieSpawner(float x, float y, float z, Matrix44 m, float yaw = 0, Mesh* mesh = NULL, Texture* texture = NULL, float pitch = 0) : Entity(x, y, z, m, yaw = 0, mesh = NULL, texture = NULL, pitch = 0) {
		loadMesh("data/Assets/Spawn/spawn.obj");
		loadTexture("data/Assets/Spawn/spawn.png");
		ultimoSpawn = 0;
		cooldown = 5;
	};
	void spawnZombie(Zombie* zombie, float time);
};
class Player : public Entity {
public:
	int vida;
	float lastHit;

	Player(float x, float y, float z, Matrix44 m, float yaw = 0, Mesh* mesh = NULL, Texture* texture = NULL, float pitch = 0) : Entity(x, y, z, m, yaw = 0, mesh = NULL, texture = NULL, pitch = 0) {
		vida = 10;
		lastHit = 0;
	};
	void CamPlayer(Camera* camera);
};

class Weapon : public Entity {
public:
	float cadencia;
	float lastShot;
	int cargador;
	int damage;

	Weapon(float x, float y, float z, Matrix44 m, float yaw = 0, Mesh* mesh = NULL, Texture* texture = NULL, float pitch = 0) : Entity(x, y, z, m, yaw = 0, mesh = NULL, texture = NULL, pitch = 0) {
		cadencia = 0;
		lastShot = 0;
		cargador = 0;
		damage = 0;
	};
	void init(float cad, int cargador, int d);
	void renderWeapon(Player* player, Shader* shader, float tiling = 1.0f);
};
#endif