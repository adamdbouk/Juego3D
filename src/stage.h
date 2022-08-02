#include "world.h"
#pragma once
class Stage {
public:
	virtual void render(World* world) {};
	virtual void update(double seconds_elapsed, World* world) {};
};

class IntroStage : public Stage {
public:
	virtual void render(World* world);
	virtual void update(double seconds_elapsed, World* world);
};

class SelectWorldStage : public Stage {
public:
	virtual void render(World* world);
	virtual void update(double seconds_elapsed, World* world);
};

class PlayStage : public Stage {
public:
	virtual void render(World* world);
	virtual void update(double seconds_elapsed, World* world);
};

class EndStage : public Stage {
public:
	virtual void render(World* world);
	virtual void update(double seconds_elapsed, World* world) {};
};

class EditorStage : public Stage {
public:
	virtual void render(World* world);
	virtual void update(double seconds_elapsed, World* world);
};

