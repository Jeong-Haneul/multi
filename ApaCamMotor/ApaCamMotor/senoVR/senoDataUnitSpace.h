#pragma once
#include "senoDataUnit.h"

class senoDataUnitSpace
{
public:
	senoDataUnitSpace(void);
	~senoDataUnitSpace(void);
	senoDataUnit UnitSpace[25600];
	int SpaceX;
	int SpaceY;
	int pX;
	int pY;
	void Create(int X, int Y);
	void CheakEOP(void);
	void DrowPos(int X, int Y, unsigned char G);
	void Clear();
};

