// Tower.h -- Tower class defintion

#ifndef TOWER_H
#define TOWER_H

#include "Enemy.h"
#include "Bullet.h"

class Tower {
	public:
		Tower(int x, int y, int type);
		void SetParams(double rng, double pow, double rt);
		void SetType(int type);

		void Render();
		void DrawRange();

		void Update(double dt);
		void Upgrade(int type);
		int UpgradeCost(int type);

		bool Fire(Enemy &enemy,	vector <Bullet> &Shots);

		// Grid coords
		int gridX;
		int gridY;

		int direction;

		// Tower properties
		int type;
		double range;
		double power;
		double rate;
		double reloadTime;
	private:
};

/*
#include "Entity.h"

class Tower : public Entity {
	public:
		Tower(SDL_Surface *Dest, SDL_Rect &DestRect,
				SDL_Surface *SrcSprite, SDL_Rect &SrcRect);
		void SetParams(double rng, double pow, double rt);
		double range;
		double power;
		double rate;
		unsigned int lastShotTime;

		void Render();
	private:
};
*/
#endif	// TOWER_H
