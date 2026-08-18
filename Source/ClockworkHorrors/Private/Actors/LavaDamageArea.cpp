// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Actors/LavaDamageArea.h"

ALavaDamageArea::ALavaDamageArea()
{
	bDamagePlayer = true;
	bDamageOnEntry = true;
	bDamageOverTime = true;

	DamageAmount = 10.0f;
	DamageInterval = 1.0f;
}
