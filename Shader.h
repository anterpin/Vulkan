#pragma once


class Shader
{
public:

	virtual ~Shader() = default;
	virtual void Start()const = 0;
	virtual void UpdateUniform() = 0;
	virtual void Init() = 0;
	virtual void Delete() = 0;
};

template<typename V,typename U>
class ChildShader : public virtual Shader
{
public:
	U uniformStruct;

};

