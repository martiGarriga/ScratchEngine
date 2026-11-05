#pragma once
#include "Windows.h"

class Clock 
{
public:
	Clock();
	void Reset();
	void StartTimer();
	void Tick();
	int Now();

private:
	LARGE_INTEGER ticks;
	LARGE_INTEGER frequency;
};
