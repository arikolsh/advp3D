#include <locale>
#include "Ship.h"
#include <iostream>
#define RUB_LEN 1; //number of blocks for each ship
#define ROC_LEN 2;
#define SUB_LEN 3;
#define DES_LEN 4;
#define RUBBER_BOAT 'b'
#define ROCKET_SHIP 'p'
#define SUBMARINE 'm'
#define DESTROYER 'd'
#define NUM_SHIP_TYPES 4


Ship::Ship(char _type) {
	type = _type;
	switch (tolower(_type)) {
	case RUBBER_BOAT:
		life = RUB_LEN;
		sinkPoints = 2;
		break;
	case ROCKET_SHIP:
		life = ROC_LEN;
		sinkPoints = 3;
		break;
	case SUBMARINE:
		life = SUB_LEN;
		sinkPoints = 7;
		break;
	case DESTROYER:
		life = DES_LEN;
		sinkPoints = 8;
	default:
		break;
	}
}

Ship::~Ship() = default;


char Ship::getType() const
{
	return type;
}

int Ship::getSinkPoints() const
{
	return sinkPoints;
}

int Ship::getLife() const
{
	return life;
}

void Ship::hit()
{
	life--;
}

bool Ship::isShip(char c)
{
	char shipType[NUM_SHIP_TYPES] = { SUBMARINE, DESTROYER, RUBBER_BOAT, ROCKET_SHIP }; //types of ships
	for (int i = 0; i < NUM_SHIP_TYPES; i++) {
		if (tolower(c) == shipType[i]) {
			return true;
		}
	}
	return false;
}