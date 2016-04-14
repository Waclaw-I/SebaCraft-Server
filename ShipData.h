#pragma once

class ShipData
{
public:

	double getPositionX();
	double getPositionY();
	double getRotation();

	int getActualHealth();
	int getActualEnergy();
	int getActualArmour();
	int getMaxHealth();
	int getMaxEnergy();
	int getMaxArmour();

	void setPositionX(double positionX);
	void setPositionY(double positionY);
	void setRotation(double rotation);

	void setActualHealth(int actualHealth);
	void setActualEnergy(int actualEnergy);
	void setActualArmour(int actualArmour);
	void setMaxHealth(int maxHealth);
	void setMaxEnergy(int maxEnergy);
	void setMaxArmour(int maxArmour);

private:

	double positionX;
	double positionY;
	double rotation;

	int actualHealth;
	int actualEnergy;
	int actualArmour;
	int maxHealth;
	int maxEnergy;
	int maxArmour;
};