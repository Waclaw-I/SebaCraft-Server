#include "ShipData.h"

double ShipData::getPositionX() { return this->positionX; }
double ShipData::getPositionY() { return this->positionY; }
double ShipData::getRotation() { return this->rotation; }

void ShipData::setPositionX(double positionX) { this->positionX = positionX; }
void ShipData::setPositionY(double positionY) { this->positionY = positionY; }
void ShipData::setRotation(double rotation) { this->rotation = rotation; }

int ShipData::getActualHealth() { return this->actualHealth; }
int ShipData::getActualEnergy() { return this->actualEnergy; }
int ShipData::getActualArmour() { return this->actualArmour; }
int ShipData::getMaxHealth() { return this->maxHealth; }
int ShipData::getMaxEnergy() { return this->maxEnergy; }
int ShipData::getMaxArmour() { return this->maxArmour; }

void ShipData::setActualHealth(int actualHealth) { this->actualHealth = actualHealth; }
void ShipData::setActualEnergy(int actualEnergy) { this->actualEnergy = actualEnergy; }
void ShipData::setActualArmour(int actualArmour) { this->actualArmour = actualArmour; }
void ShipData::setMaxHealth(int maxHealth) { this->maxHealth = maxHealth; }
void ShipData::setMaxEnergy(int maxEnergy) { this->maxEnergy = maxEnergy; }
void ShipData::setMaxArmour(int maxArmour) { this->maxArmour = maxArmour; }

