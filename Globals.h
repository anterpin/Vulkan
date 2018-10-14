#pragma once
#include "al.h"



namespace Global
{
	

	constexpr float FOV = 70;
	constexpr float zNear = 0.1f;
	constexpr float zFar = 1000.0f;

	constexpr float colors[4] = { 1,0,0,1 };

	namespace Input
	{
		void SetKey(int value, size_t i);
		int GetKey(size_t i);

		bool GetDrag();
		void SetDrag(bool d);

		Point& Mouse();
	};

	float4x4& ProjectionMatrix();
	Rect& ViewPort();
	Rect& Windowrect();

	


};

