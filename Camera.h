#pragma once
#include "al.h"

class Camera
{
private:
	float3 position;
	float3 forward;
public:
	Camera();
	~Camera();

	inline void SetPosition(const float3& pos)
	{
		position = pos;
	}

	inline const float3 GetPosition()const
	{
		return position;
	}

	inline void SetRotation(const float3& rot)
	{
		forward = rot;
	}

	inline const float3 GetForward()const
	{
		return forward;
	}

	inline float4x4 GetViewMatrix()const
	{
		return viewMatrix(position, forward, float3(0, 1, 0));
	}


};

