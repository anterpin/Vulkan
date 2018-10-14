#pragma once
#include <cstdlib>


class VertexBuffer
{
public:
	virtual void Bind(uint32_t binding)const = 0;
	virtual void Draw()const = 0;
	virtual ~VertexBuffer() = default;
};

class IndexBuffer
{
public:
	virtual void Bind()const = 0;
	virtual void DrawIndexed()const = 0;
	virtual ~IndexBuffer() = default;
};

class UniformBuffer
{
public:
	virtual void Update(void * const obj, size_t bytes)= 0;
	virtual ~UniformBuffer() = default;
};



