#include "entity.h"
#include "pathfinders.h"

Vector3 Zombie::AStarPath(Vector3 target, uint8** maps) {
	int output[100];
	int path_steps = -1;
	float tileSizeX = 1.f;
	float tileSizeZ = 1.f;
	int Mapwidth = 100;
	int Mapheight = 100;

	int startx = clamp(abs(pos.x) / tileSizeX, 0, Mapwidth - 1);
	int startz = clamp(abs(pos.z) / tileSizeZ, 0, Mapheight - 1);

	float targetx = clamp(abs(target.x) / tileSizeX, 0, Mapwidth - 1);
	float targetz = clamp(abs(target.z) / tileSizeZ, 0, Mapheight - 1);

	if (startx == floor(targetx) && startz == floor(targetz)) return Vector3(startx, pos.y, startz);

	uint8* map;
	if (pos.x >= 0 && pos.z >= 0) map = maps[0];
	else if (pos.x >= 0 && pos.z <= 0) map = maps[1];
	else if (pos.x <= 0 && pos.z >= 0) map = maps[2];
	else if (pos.x <= 0 && pos.z <= 0) map = maps[3];

	path_steps = AStarFindPathNoTieDiag(
		startx, startz, //origin (tienen que ser enteros)
		targetx, targetz, //target (tienen que ser enteros)
		map, //pointer to map data
		Mapwidth, Mapheight, //map width and height
		output, //pointer where the final path will be stored
		100); //max supported steps of the final path


	int gridIndex = output[0];
	int posxgrid = gridIndex % Mapwidth;
	int posygrid = gridIndex / Mapwidth;

	Vector3 nextPos = Vector3(posxgrid * tileSizeX, 0.0f, posygrid * tileSizeZ);
	if (target.x < 0) nextPos.x *= -1;
	if (target.z < 0) nextPos.z *= -1;

	return nextPos;
}

void Zombie::move(Vector3 target) {
	pos.x += (target.x - (trunc(pos.x * 10) / 10)) * vel;
	pos.z += (target.z - (trunc(pos.z * 10) / 10)) * vel;
	m.setIdentity();
	m.setTranslation(pos.x, pos.y, pos.z);

	float catetoX = pos.x - target.x;
	float catetoZ = pos.z - target.z;
	float hipotenusa = sqrt(pow(catetoX, 2) + pow(catetoZ, 2));
	float div = abs(catetoZ / hipotenusa);
	yaw = asin(div);
	if (catetoX < 0) {
		if (catetoZ < 0) yaw += 180;
		else yaw += 90;
	}
	else if (catetoZ < 0) yaw += 270;

	m.rotate((yaw + 135)* DEG2RAD, Vector3(0, 1, 0));
}


void Zombie::renderAnimation(float time, float tiling) {
	Shader* shaderAnim = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
	shaderAnim->enable();
	Camera* camera = Camera::current;
	Animation* walk = Animation::Get("data/Zombies/Animation/animations_walking.skanim");
	walk->assignTime(time);
	if (shaderAnim) {
		shaderAnim->setUniform("u_color", Vector4(1, 1, 1, 1));
		shaderAnim->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shaderAnim->setUniform("u_texture", texture);
		shaderAnim->setUniform("u_time", time);
		shaderAnim->setUniform("u_model", m);
		shaderAnim->setUniform("u_texture_tiling", tiling);
		mesh->renderAnimated(GL_TRIANGLES, &walk->skeleton);
	}
	shaderAnim->disable();
}

void Player::CamPlayer(Camera* camera) {
	camera->enable();
	m.setTranslation(pos.x, pos.y, pos.z);
	m.rotate(yaw * DEG2RAD, Vector3(0.0f, 1.0f, 0.0f));
	
	Matrix44 pitchM;
	pitchM.rotate(pitch * DEG2RAD, Vector3(1.0f, 0.0f, 0.0f));
	Vector3 forward = pitchM.rotateVector(Vector3(0.0f, 0.0f, -1.0f));
	forward = m.rotateVector(forward);

	Vector3 eye = m * Vector3(0.0f, 1.0f, -0.5f);
	Vector3 center = eye + forward;

	Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
	camera->lookAt(eye, center, up);
}

void Entity::DeleteEntity() {
	this->m.setTranslation(0, 10000000, 0);
}

void Entity::render(Shader* shader, float tiling) {
	Camera* camera = Camera::current;
	shader->enable();
	if (shader)	{
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_time", time);
		if (texture != NULL) shader->setUniform("u_texture", texture, 0);
		shader->setUniform("u_model", m);
		shader->setUniform("u_texture_tiling", tiling);
		mesh->render(GL_TRIANGLES);
	}
	shader->disable();
}

void Entity::copy(Entity* entity) {
	this->mesh = entity->mesh;
	this->texture = entity->texture;
	this->m = entity->m;
}

void Weapon::init(float cad, int cargador, int d) {
	cadencia = cad;
	this->cargador = cargador;
	damage = d;
}

void Weapon::renderWeapon(Player* player, Shader* shader, float tiling) { //AAAAAAH
	Camera* camera = Camera::current;
	this->pos = player->pos;
	this->pitch = player->pitch;
	this->yaw = player->yaw - 90;
	m.setTranslation(camera->eye.x + 0.1, camera->eye.y - 0.3, camera->eye.z);
	m.rotate(yaw * DEG2RAD, Vector3(0.0f, 1.0f, 0.0f));
	m.rotate(pitch * DEG2RAD, Vector3(0.0f, 0.0f, 1.0f));
	render(shader, tiling);
}

void ZombieSpawner::spawnZombie(Zombie* zombie, float time) {
	ultimoSpawn = time;
	zombie->pos = pos;
	zombie->pos.y = 0;
	zombie->m.setTranslation(pos.x, 0, pos.z);
}
