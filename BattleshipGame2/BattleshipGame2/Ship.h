#pragma once

class Ship {
public:

	Ship(char _type);

	// Class destructor
	~Ship();

	int getLife() const; //returns life of ship

	char getType() const; //returns type of ship

	int getSinkPoints() const; //returns number of points for sinking

	void hit(); //Takes one off the ship's life

	static bool isShip(char c); //return true if c is a ship

private:
	int life;
	char type;
	int sinkPoints;
	Ship(const Ship& that) = delete;
};