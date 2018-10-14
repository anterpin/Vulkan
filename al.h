#pragma once
//#include "algebra.h"

/*typedef	Vec2 float2;

typedef Vec3 float3;

typedef Vec4 float4;

typedef Mat2 float2x2;
typedef Mat3 float3x3;
typedef Mat4 float4x4;

typedef int2 Rect;*/

#include<iostream>
#define M_PI 3.14151692653f


template <typename T>
struct vec2
{
	T x, y;

	vec2() :x(0), y(0)
	{
	}

	vec2(T x, T y):x(x),y(y)
	{
	}

	T& operator[](size_t i)
	{
		return reinterpret_cast<T*>(this)[i];
	}

	const T& operator[](size_t i)const
	{
		return reinterpret_cast<T*>(this)[i];
	}

	T dot(const vec2<T>& v)const
	{
		return x * v.x + y * v.y;
	}

	T length()const
	{
		return sqrt(dot(*this));
	}

	vec2<T> getNormalized()const
	{
		vec2<T> v;
		T l = length();
		v.x = x / l;
		v.y = y / l;
		return v;
	}

	void normalize()
	{
		*this = getNormalized();
	}

	vec2<T> operator +(const vec2<T>& a)const
	{
		vec2<T> v
		(x + a.x,
			y + a.y);
		return v;
	}

	void operator +=(const vec2<T>& a)
	{
		*this = *this + a;
	}

	vec2<T> operator -(const vec2<T>& a)const
	{
		vec2<T> v
		(x - a.x,
			y - a.y);
		return v;
	}

	void operator -=(const vec2<T>& a)
	{
		*this = *this - a;
	}

	vec2<T> operator *(const vec2<T>& a)const
	{
		vec2<T> v
		(x * a.x,
			y * a.y);
		return v;
	}

	void operator *=(const vec2<T>& a)
	{
		*this = *this * a;
	}

	vec2<T> operator /(const vec2<T>& a)const
	{
		vec2<T> v
		(x / a.x,
			y / a.y);
		return v;
	}

	void operator /=(const vec2<T>& a)
	{
		*this = *this / a;
	}

	vec2<T> operator *(T a)const
	{
		vec2<T> v
		(x * a,
			y * a);
		return v;
	}

	void operator *=(T a)
	{
		*this = *this * a;
	}

	vec2<T> operator /(T a)const
	{
		vec2<T> v
		(x * a,
			y * a);
		return v;
	}

	void operator /=(T a)
	{
		*this = *this * a;
	}

	vec2<T> cross(const vec2<T>& v)const
	{
		vec2<T> v(
			y*v.x - x*v.y, x*v.y - y*v.x
		);

		return v;
	}

};


template <typename T>
struct vec3
{
	T x, y, z;

	vec3() :x(0), y(0), z(0)
	{
	}

	vec3(T x, T y,T z) :x(x), y(y), z(z)
	{
	}

	T& operator[](size_t i)
	{
		return reinterpret_cast<T*>(this)[i];
	}

	const T& operator[](size_t i)const
	{
		return reinterpret_cast<T const* >(this)[i];
	}

	T dot(const vec3<T>& v)const
	{
		return x * v.x + y * v.y + z * v.z;
	}

	T length()const
	{
		return sqrt(dot(*this));
	}

	vec3<T> getNormalized()const
	{
		vec3<T> v;
		T l = length();
		v.x = x / l;
		v.y = y / l;
		v.z = z / l;
		return v;
	}

	void normalize()
	{
		*this = getNormalized();
	}

	vec3<T> operator +(const vec3<T>& a)const
	{
		vec3<T> v
		(x + a.x,
			y + a.y,
			z + a.z);
		return v;
	}

	void operator +=(const vec3<T>& a)
	{
		*this = *this + a;
	}

	vec3<T> operator -(const vec3<T>& a)const
	{
		vec3<T> v
		(x - a.x,
			y - a.y,
			z - a.z);
		return v;
	}

	void operator -=(const vec3<T>& a)
	{
		*this = *this - a;
	}

	vec3<T> operator *(const vec3<T>& a)const
	{
		vec3<T> v
		(x * a.x,
			y * a.y,
			z * a.z);
		return v;
	}

	void operator *=(const vec3<T>& a)
	{
		*this = *this * a;
	}

	vec3<T> operator /(const vec3<T>& a)const
	{
		vec3<T> v
		(x / a.x,
			y / a.y,
			z / a.z);
		return v;
	}

	void operator /=(const vec3<T>& a)
	{
		*this = *this / a;
	}

	vec3<T> operator *(T a)const
	{
		vec3<T> v
		(x * a,
			y * a,
			z * a);
		return v;
	}

	void operator *=(T a)
	{
		*this = *this * a;
	}

	vec3<T> operator /(T a)const
	{
		vec3<T> v
		(x * a,
			y * a,
			z * a);
		return v;
	}

	void operator /=(T a)
	{
		*this = *this * a;
	}

	vec3<T> cross(const vec3<T>& v)const
	{
		vec3<T> v(
			y*v.z - z*v.y, z*v.x - x*v.z,x * v.y - y*v.x
		);

		return v;
	}

};

template <typename T>
struct vec4
{
	T x, y, z, w;

	vec4() :x(0), y(0), z(0), w(0)
	{
	}

	vec4(T x, T y, T z, T w) :x(x), y(y), z(z), w(w)
	{
	}

	T& operator[](size_t i)
	{
		return reinterpret_cast<T*>(this)[i];
	}

	const T& operator[](size_t i)const
	{
		return reinterpret_cast<T const*>(this)[i];
	}

	T dot(const vec4<T>& v)const
	{
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	T length()const
	{
		return sqrt(dot(*this));
	}

	vec4<T> getNormalized()const
	{
		vec4<T> v;
		T l = length();
		v.x = x / l;
		v.y = y / l;
		v.z = z / l;
		v.w = w / l;
		return v;
	}

	void normalize()
	{
		*this = getNormalized();
	}

	vec4<T> operator +(const vec4<T>& a)const
	{
		vec4<T> v
		(x + a.x,
			y + a.y,
			z + a.z,
			w + a.w);
		return v;
	}

	void operator +=(const vec4<T>& a)
	{
		*this = *this + a;
	}

	vec4<T> operator -(const vec4<T>& a)const
	{
		vec4<T> v
		(x - a.x,
			y - a.y,
			z - a.z,
			w - a.w);
		return v;
	}

	void operator -=(const vec4<T>& a)
	{
		*this = *this - a;
	}

	vec4<T> operator *(const vec4<T>& a)const
	{
		vec4<T> v
		(x * a.x,
			y * a.y,
			z * a.z,
			w * a.w);
		return v;
	}

	void operator *=(const vec4<T>& a)
	{
		*this = *this * a;
	}

	vec4<T> operator /(const vec4<T>& a)const
	{
		vec4<T> v
		(x / a.x,
			y / a.y,
			z / a.z,
			w / a.w);
		return v;
	}

	void operator /=(const vec4<T>& a)
	{
		*this = *this / a;
	}

	vec4<T> operator *(T a)const
	{
		vec4<T> v
		(x * a,
			y * a,
			z * a);
		return v;
	}

	void operator *=(T a)
	{
		*this = *this * a;
	}

	vec4<T> operator /(T a)const
	{
		vec4<T> v
		(x * a,
			y * a,
			z * a);
		return v;
	}

	void operator /=(T a)
	{
		*this = *this * a;
	}

	vec4<T> cross(const vec4<T>& v)const
	{
		vec4<T> v(
			y*v.z - z*v.y, z*v.x - x*v.z, x * v.y - y*v.x
		);

		return v;
	}

};

template <typename T>
struct mat2
{
	vec2<T> arr[2];

	mat2()
	{
		arr[0][0] = 1;
		arr[1][1] = 1;
	}
	
	mat2(const vec2<T>& a, const vec2<T>& b)
	{
		arr[0] = a;
		arr[1] = b;
	}

	vec2<T>& operator[](size_t i)
	{
		return arr[i];
	}

	const vec2<T>& operator[](size_t i)const
	{
		return arr[i];
	}

	mat2<T> operator +(const mat2<T>& mat)const
	{
		mat2<T> m;
		for (unsigned int i = 0; i < 2; i++)
		{
			m[i] = arr[i] + mat[i];
		}

		return m;
	}

	void operator +=(const mat2<T>& mat)
	{
		*this = *this + mat;
	}

	mat2<T> operator -(const mat2<T>& mat)const
	{
		mat2<T> m;
		for (unsigned int i = 0; i < 2; i++)
		{
			m[i] = arr[i] + mat[i];
		}

		return m;
	}

	void operator -=(const mat2<T>& mat)
	{
		*this = *this - mat;
	}

	vec2<T> operator *(const vec2<T>& v)const
	{
		vec2<T> a;
		a[0] = arr[0][0] * v[0] + arr[0][1] * v[1];
		a[1] = arr[1][0] * v[0] + arr[1][1] * v[1];

		return a;
	}

	mat2<T> operator *(const mat2<T>& t)const
	{
		const mat2<T>& v = *this;
		mat2<T> m;
		vec2<T> v1(v[0][0],v[1][0]);
		m[0] = t * v1;

		vec2<T> v2(v[0][1], v[1][1]);
		m[1] = t * v2;
		return m;
	}

	void operator *=(const mat2<T>& v)
	{
		&this = *this * v;
	}
};

template <typename T>
struct mat3
{
	vec3<T> arr[3];

	mat3()
	{
		arr[0][0] = 1;
		arr[1][1] = 1;
		arr[2][2] = 1;
	}

	mat3(const vec3<T>& a, const vec3<T>& b,const vec3<T>& c)
	{
		arr[0] = a;
		arr[1] = b;
		arr[2] = c;
	}

	vec3<T>& operator[](size_t i)
	{
		return arr[i];
	}

	const vec3<T>& operator[](size_t i)const
	{
		return arr[i];
	}

	mat3<T> operator +(const mat3<T>& mat)const
	{
		mat3<T> m;
		for (unsigned int i = 0; i < 3; i++)
		{
			m[i] = arr[i] + mat[i];
		}

		return m;
	}

	void operator +=(const mat3<T>& mat)
	{
		*this = *this + mat;
	}

	mat3<T> operator -(const mat3<T>& mat)const
	{
		mat3<T> m;
		for (unsigned int i = 0; i < 3; i++)
		{
			m[i] = arr[i] + mat[i];
		}

		return m;
	}

	void operator -=(const mat3<T>& mat)
	{
		*this = *this - mat;
	}

	vec3<T> operator *(const vec3<T>& v)const
	{
		vec3<T> a;
		a[0] = arr[0][0] * v[0] + arr[0][1] * v[1] + arr[0][2] * v[2];
		a[1] = arr[1][0] * v[0] + arr[1][1] * v[1] + arr[1][2] * v[2];
		a[2] = arr[2][0] * v[0] + arr[2][1] * v[1] + arr[2][2] * v[2];

		return a;
	}

	mat3<T> operator *(const mat3<T>& t)const
	{
		const mat3<T>& v = *this;
		mat3<T> m;
		vec3<T> v1(v[0][0], v[1][0],v[2][0]);
		m[0] = t * v1;

		vec3<T> v2(v[0][1], v[1][1],v[2][1]);
		m[1] = t * v2;

		vec3<T> v3(v[0][2], v[1][2], v[2][2]);
		m[2] = t * v3;

		return m;
	}

	void operator *=(const mat3<T>& v)
	{
		&this = *this * v;
	}
};


template <typename T>
struct mat4
{
	vec4<T> arr[4];

	mat4()
	{
		arr[0][0] = 1;
		arr[1][1] = 1;
		arr[2][2] = 1;
		arr[3][3] = 1;
	}

	mat4(const vec4<T>& a, const vec4<T>& b, const vec4<T>& c, const vec4<T>& d)
	{
		arr[0] = a;
		arr[1] = b;
		arr[2] = c;
		arr[3] = d;
	}

	vec4<T>& operator[](size_t i)
	{
		return arr[i];
	}

	const vec4<T>& operator[](size_t i)const
	{
		return arr[i];
	}

	mat4<T> operator +(const mat4<T>& mat)const
	{
		mat4<T> m;
		for (unsigned int i = 0; i < 4; i++)
		{
			m[i] = arr[i] + mat[i];
		}

		return m;
	}

	void operator +=(const mat4<T>& mat)
	{
		*this = *this + mat;
	}

	mat4<T> operator -(const mat4<T>& mat)const
	{
		mat4<T> m;
		for (unsigned int i = 0; i < 4; i++)
		{
			m[i] = arr[i] + mat[i];
		}

		return m;
	}

	void operator -=(const mat4<T>& mat)
	{
		*this = *this - mat;
	}

	vec4<T> operator *(const vec4<T>& v)const
	{
		vec4<T> a;
		a[0] = arr[0][0] * v[0] + arr[0][1] * v[1] + arr[0][2] * v[2] + arr[0][3] * v[3];
		a[1] = arr[1][0] * v[0] + arr[1][1] * v[1] + arr[1][2] * v[2] + arr[1][3] * v[3];
		a[2] = arr[2][0] * v[0] + arr[2][1] * v[1] + arr[2][2] * v[2] + arr[2][3] * v[3];
		a[3] = arr[3][0] * v[0] + arr[3][1] * v[1] + arr[3][2] * v[2] + arr[3][3] * v[3];

		return a;
	}

	mat4<T> operator *(const mat4<T>& t)const
	{
		const mat4<T>& v = *this;
		mat4<T> m;
		vec4<T> v1(v[0][0], v[1][0], v[2][0],v[3][0]);
		m[0] = t * v1;

		vec4<T> v2(v[0][1], v[1][1], v[2][1],v[3][1]);
		m[1] = t * v2;


		vec4<T> v3(v[0][2], v[1][2], v[2][2],v[3][2]);
		m[2] = t * v3;

		vec4<T> v4(v[0][3], v[1][3], v[2][3], v[3][3]);
		m[3] = t * v4;

		return m;
	}

	void operator *=(const mat4<T>& v)
	{
		&this = *this * v;
	}
};


template<typename T>
std::ostream& operator <<(std::ostream& of, const vec2<T>& v)
{
	of << v.x << " " << v.y;
	return of;
}

template<typename T>
std::ostream& operator <<(std::ostream& of, const vec3<T>& v)
{
	of << v.x << " " << v.y << " " << v.z ;
	return of;
}

template<typename T>
std::ostream& operator <<(std::ostream& of, const vec4<T>& v)
{
	of << v.x << " " << v.y << " "<<v.z<<" "<<v.w;
	return of;
}

template<typename T>
std::ostream& operator <<(std::ostream& of, const mat2<T>& v)
{
	for (unsigned int i = 0; i < 2;i++)
	{
		of << v[i] << std::endl;
	}

	return of;
}

template<typename T>
std::ostream& operator <<(std::ostream& of, const mat3<T>& v)
{
	for (unsigned int i = 0; i < 3; i++)
	{
		of << v[i] << std::endl;
	}

	return of;
}

template<typename T>
std::ostream& operator <<(std::ostream& of, const mat4<T>& v)
{
	for (unsigned int i = 0; i < 4; i++)
	{
		of << v[i] << std::endl;
	}

	return of;
}

typedef vec2<float> float2;
typedef vec2<int> int2;
typedef vec2<unsigned int> uint2;

typedef vec3<float> float3;
typedef vec3<int> int3;
typedef vec3<unsigned int> uint3;

typedef vec4<float> float4;
typedef vec4<int> int4;
typedef vec4<unsigned int> uint4;

typedef mat2<float> float2x2;
typedef mat3<float> float3x3;
typedef mat4<float> float4x4;

static float toRadians(float angle)
{
	return (angle * M_PI) / 180.0f;
}

static float toDegree(float angle)
{
	return (angle * 180.0f) / M_PI;
}

static float4x4 translate(const float3& position)
{
	float4x4 m;

	m[3][0] = position[0];
	m[3][1] = position[1];
	m[3][2] = position[2];

	return m;
}

static float4x4 _scale(const float3& scale)
{
	float4x4 m;
	m[0][0] = scale[0];
	m[1][1] = scale[1];
	m[2][2] = scale[2];

	return m;
}

static float4x4 rotate(const float3& axis, float angle)
{
	float4x4 mat;
	int32_t layer = 0;
	

	for (size_t i = 0; i<3; i++)
	{
		if (axis[i] == 1)
		{
			continue;
		}
		int32_t row = 0;
		for (size_t j = 0; j<3; j++)
		{
			if (axis[j] == 1)
			{
				continue;
			}

			if (row == layer)
			{
				mat[j][i] = cosf(-angle);
			}
			else
			{
				float sign = axis[1] ? -1.0f : 1.0f;//y axis is different;
				mat[j][i] = sinf(-angle) * (layer - row) * sign;
			}


			row++;
		}
		layer++;
	}
	
	return mat;
}

static float4x4 modelMatrix(const float3& position, const float3& rotation, const float3& scale)
{
	float4x4 t = translate(position);
	
	float4x4 rx = rotate(float3(1, 0, 0), toRadians(rotation.x));
	float4x4 ry = rotate(float3(0, 1, 0), toRadians(rotation.y));
	float4x4 rz = rotate(float3(0, 0, 1), toRadians(rotation.z));

	float4x4 r = rx * ry * rz;
	//float4x4 r = rotate(rotation);
	float4x4 s = _scale(scale);

	return  t * r * s;
}

static float4x4 viewMatrix(const float3& eye, const float3& forward, const float3& up)
{
	float3 zaxis = forward.getNormalized();// (eye - target).getNormalized();//forward
	float3 xaxis = (up * zaxis).getNormalized();//right
	float3 yaxis = zaxis * xaxis;//up vector

	float4x4 viewMatrix(float4(xaxis.x, yaxis.x, zaxis.x, 0),
		float4(xaxis.y, yaxis.y, zaxis.y, 0),
		float4(xaxis.z, yaxis.z, zaxis.z, 0),
		float4(-xaxis.dot(eye), -yaxis.dot(eye), -zaxis.dot(eye), 1));

	return viewMatrix;
}

static float4x4 projectionMatrix(float fov, float aspect, float zNear, float zFar)
{
	float4x4 mat;

	float f = 1.0f / tanf(toRadians(fov / 2.0f));

	mat[0][0] = f / aspect;
	mat[1][1] = f;
	mat[2][2] = (zNear + zFar) / (zNear - zFar);
	mat[3][2] = (2.0f * zNear *zFar )/ (zNear - zFar);
	mat[2][3] = -1.0f;
	mat[3][3] = 0;



	return mat;
}

class Transform
{
public:
	float3 position;
	float3 rotation;
	float3 scale;

	Transform() :scale(float3(1, 1, 1))
	{
	}

	Transform(const float3& position, const float3& rotation, const float3& scale) :position(position), rotation(rotation), scale(scale)
	{
	}

	float4x4 GetModelMatrix()const
	{
		//std::cout << toRadians(rotation.y) << std::endl;
		//std::cout << rotate(float3(0, 1, 0), toRadians(rotation.y)) << std::endl << std::endl;
		return modelMatrix(position, rotation, scale);
	}
};

typedef int2 Rect;
typedef int2 Point;