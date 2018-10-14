#include "Globals.h"


int keyArr[350] = {};
bool dragging = false;
Point mouse;

float4x4 projection;
Rect windowRect = { 600,400 };
Rect viewPort;

float4x4 & Global::ProjectionMatrix()
{
	return projection;
}

Rect& Global::ViewPort()
{
	return viewPort;
}

Rect& Global::Windowrect()
{
	return windowRect;
}

void Global::Input::SetKey(int value, size_t i)
{
	keyArr[i] = value;
}

int Global::Input::GetKey(size_t i)
{
	return keyArr[i];
}

bool Global::Input::GetDrag()
{
	return dragging;
}

void Global::Input::SetDrag(bool d)
{
	dragging = d;
}

Point & Global::Input::Mouse()
{
	return mouse;
}
