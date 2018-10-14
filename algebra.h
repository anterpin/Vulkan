#pragma once
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <cmath>
#define M_PI 3.14151692653

template<typename T, size_t N>
struct Vec
{
};

template <typename T, size_t N>
inline T _dot(const Vec<T, N>& a, const Vec<T, N>& b)
{
	T v = {};

	for (size_t i = 0; i<N; i++)
	{
		v += a.at(i) * b.at(i);
	}

	return v;
}

inline float _squareRoot(const float& a)
{
	return sqrtf(a);
}

inline double _squareRoot(const double& a)
{
	return sqrt(a);
}

template <typename T, size_t N>
inline T _length(const Vec<T, N>& a)
{
	return _squareRoot(_dot(a, a));
}

template <typename T, size_t N>
inline Vec<T, N> _getNormalized(const Vec<T, N>& a)
{
	T length = _length(a);
	Vec<T, N> vec;

	for (size_t i = 0; i<N; i++)
	{
		vec[i] = a.at(i) / length;
	}

	return vec;
}

template <typename T, size_t N>
inline Vec<T, N> add(const Vec<T, N>& a, const T& b)
{
	Vec<T, N> vec;

	for (size_t i = 0; i<N; i++)
	{
		vec[i] = a.at(i) + b;
	}

	return vec;
}

template <typename T, size_t N>
inline Vec<T, N> sum(const Vec<T, N>& a, const Vec<T, N>& b)
{
	Vec<T, N> vec;

	for (size_t i = 0; i<N; i++)
	{
		vec[i] = a.at(i) + b.at(i);
	}

	return vec;
}

template <typename T, size_t N>
inline Vec<T, N> sub(const Vec<T, N>& a, const Vec<T, N>& b)
{
	Vec<T, N> vec;

	for (size_t i = 0; i<N; i++)
	{
		vec[i] = a.at(i) - b.at(i);
	}

	return vec;
}

template <typename T, size_t N>
inline Vec<T, N> mul(const Vec<T, N>& a, const T& scalar)//vec scalar mul
{
	Vec<T, N> vec;

	for (size_t i = 0; i<N; i++)
	{
		vec[i] = a.at(i) * scalar;
	}

	return vec;
}

template <typename T, size_t N>
inline Vec<T, N> mul(const Vec<Vec<T, N>, N>& mat, const Vec<T, N>& v)//mat vector mul
{
	Vec<T, N> vec;

	for (size_t i = 0; i<N; i++)
	{
		vec[i] = _dot(mat.at(i), v);
	}

	return vec;
}

template <typename T, size_t N>
inline Vec<T, N> div(const Vec<T, N>& a, const T& scalar)
{
	Vec<T, N> vec;

	for (size_t i = 0; i<N; i++)
	{
		vec[i] = a.at(i) / scalar;
	}

	return vec;
}

template <typename T, size_t N>
inline Vec<T, N> cross(const Vec<T, N>& a, const Vec<T, N>& b)
{
	Vec<T, N> vec;


	for (size_t i = 0; i<N; i++)
	{
		size_t index1 = (i + 1) % N;
		size_t index2 = (i + 2) % N;

		vec[i] = a.at(index1)*b.at(index2) - a.at(index2)*b.at(index1);
	}

	return vec;
}

template <typename T, size_t N>
inline Vec<Vec<T, N>, N> cross(const Vec<Vec<T, N>, N>& a, const Vec<Vec<T, N>, N>& b)
{
	Vec<Vec<T, N>, N> mat;


	for (size_t i = 0; i<N; i++)
	{
		for (size_t j = 0; j<N; j++)
		{
			T sum = 0;
			for (size_t v = 0; v<N; v++)
			{
				sum += a.at(i).at(v) * b.at(v).at(j);
			}
			mat[i][j] = sum;
		}

	}

	return mat;
}

template <typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const Vec<T, N>& a)
{
	for (size_t i = 0; i<N; i++)
	{
		std::cout << a.at(i) << " ";
	}

	return os;
}

template <typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const Vec<Vec<T, N>, N>& a)//for matrices
{
	for (size_t i = 0; i<N; i++)
	{
		std::cout << a.at(i);

		if (i<N - 1)
			std::cout << std::endl;
	}

	return os;
}

template <typename T, size_t N>
inline void constructor(Vec<T, N>& a)
{
	memset(&a, 0, sizeof(Vec<T, N>));
}

template <typename T, size_t N>
inline void constructor(Vec<Vec<T, N>, N>& a)//indentity
{
	for (size_t i = 0; i<N; i++)
	{
		for (size_t j = 0; j<N; j++)
		{
			a[i][j] = i == j ? 1 : 0;
		}
	}
}

#define N 2
template<typename T>
struct Vec<T, N>
{
	T x, y;

	Vec(T x, T y) :x(x), y(y)
	{
	}

	Vec()
	{
		constructor(*this);
	}

	T& operator[](size_t i)
	{
		return ((T*)this)[i];
	}

	const T& at(size_t i)const
	{
		return ((T*)this)[i];
	}

	Vec<T, N> operator+(const Vec<T, N>& b)const
	{
		return sum(*this, b);
	}

	void operator+=(const Vec<T, N>& b)
	{
		*this = sum(*this, b);
	}

	Vec<T, N> operator-(const Vec<T, N>& b)const
	{
		return sub(*this, b);
	}

	void operator-=(const Vec<T, N>& b)
	{
		*this = sub(*this, b);
	}

	auto operator*(const T& b)const -> decltype(mul(*this, b))//mat x vector = vector
	{
		return mul(*this, b);
	}

	Vec<T, N> operator*(const Vec<T, N>& b)const
	{
		return cross(*this, b);
	}

	void operator*=(const T& b)
	{
		*this = mul(*this, b);
	}

	Vec<T, N> operator/(const T& b)const
	{
		return div(*this, b);
	}

	void operator/=(const T& b)
	{
		*this = div(*this, b);
	}

	T dot(const Vec<T, N>& b)const
	{
		return _dot(*this, b);
	}

	T length()const
	{
		return _length(*this);
	}

	Vec<T, N> getNormalized()const
	{
		return _getNormalized(*this);
	}

	void normalize()
	{
		*this = _getNormalized(*this);
	}

	Vec<T, N> operator+(const T& scalar)
	{
		return add(*this);
	}

	void operator+=(const T& scalar)
	{
		*this = add(*this);
	}


};
#undef N 

#define N 3
template<typename T>
struct Vec<T, N>
{
	T x, y, z;

	Vec(T x, T y, T z) :x(x), y(y), z(z)
	{
	}

	Vec()
	{
		constructor(*this);
	}


	T& operator[](size_t i)
	{
		return ((T*)this)[i];
	}

	const T& at(size_t i)const
	{
		return ((T*)this)[i];
	}

	Vec<T, N> operator+(const Vec<T, N>& b)const
	{
		return sum(*this, b);
	}

	void operator+=(const Vec<T, N>& b)
	{
		*this = sum(*this, b);
	}

	Vec<T, N> operator-(const Vec<T, N>& b)const
	{
		return sub(*this, b);
	}

	void operator-=(const Vec<T, N>& b)
	{
		*this = sub(*this, b);
	}

	auto operator*(const T& b)const -> decltype(mul(*this, b))
	{
		return mul(*this, b);
	}

	void operator*=(const T& b)
	{
		*this = mul(*this, b);
	}

	Vec<T, N> operator/(const T& b)const
	{
		return div(*this, b);
	}

	void operator/=(const T& b)
	{
		*this = div(*this, b);
	}

	T dot(const Vec<T, N>& b)const
	{
		return _dot(*this, b);
	}

	T length()const
	{
		return _length(*this);
	}

	Vec<T, N> getNormalized()const
	{
		return _getNormalized(*this);
	}

	void normalize()
	{
		*this = _getNormalized(*this);
	}

	Vec<T, N> operator+(const T& scalar)
	{
		return add(*this);
	}

	void operator+=(const T& scalar)
	{
		*this = add(*this);
	}

	Vec<T, N> operator*(const Vec<T, N>& b)const
	{
		return cross(*this, b);
	}

};
#undef N 

#define N 4
template<typename T>
struct Vec<T, N>
{
	T x, y, z, w;


	Vec(T x, T y, T z, T w) :x(x), y(y), z(z), w(w)
	{
	}

	Vec()
	{
		constructor(*this);
	}


	T& operator[](size_t i)
	{
		return ((T*)this)[i];
	}

	const T& at(size_t i)const
	{
		return ((T*)this)[i];
	}

	Vec<T, N> operator+(const Vec<T, N>& b)const
	{
		return sum(*this, b);
	}

	void operator+=(const Vec<T, N>& b)
	{
		*this = sum(*this, b);
	}

	Vec<T, N> operator-(const Vec<T, N>& b)const
	{
		return sub(*this, b);
	}

	void operator-=(const Vec<T, N>& b)
	{
		*this = sub(*this, b);
	}

	auto operator*(const T& b)const -> decltype(mul(*this, b))
	{
		return mul(*this, b);
	}

	void operator*=(const T& b)
	{
		*this = mul(*this, b);
	}

	Vec<T, N> operator/(const T& b)const
	{
		return div(*this, b);
	}

	void operator/=(const T& b)
	{
		*this = div(*this, b);
	}

	T dot(const Vec<T, N>& b)const
	{
		return _dot(*this, b);
	}

	T length()const
	{
		return _length(*this);
	}

	Vec<T, N> getNormalized()const
	{
		return _getNormalized(*this);
	}

	void normalize()
	{
		*this = _getNormalized(*this);
	}

	Vec<T, N> operator+(const T& scalar)
	{
		return add(*this);
	}

	void operator+=(const T& scalar)
	{
		*this = add(*this);
	}

	Vec<T, N> operator*(const Vec<T, N>& b)const
	{
		return cross(*this, b);
	}

};
#undef N 


typedef Vec<float, 2> Vec2;
typedef Vec<Vec2, 2> Mat2;

typedef Vec<int32_t, 2> Vec2i;
typedef Vec<Vec2i, 2> Mat2i;

typedef Vec<uint32_t, 2> Vec2ui;
typedef Vec<Vec2ui, 2> Mat2ui;

typedef Vec<double, 2> Vec2d;
typedef Vec<Vec2d, 2> Mat2d;

typedef Vec<float, 3> Vec3;
typedef Vec<Vec3, 3> Mat3;

typedef Vec<int32_t, 3> Vec3i;
typedef Vec<Vec3i, 3> Mat3i;

typedef Vec<uint32_t, 3> Vec3ui;
typedef Vec<Vec3ui, 3> Mat3ui;

typedef Vec<double, 3> Vec3d;
typedef Vec<Vec3d, 3> Mat3d;

typedef Vec<float, 4> Vec4;
typedef Vec<Vec4, 4> Mat4;

typedef Vec<int32_t, 4> Vec4i;
typedef Vec<Vec4i, 4> Mat4i;

typedef Vec<uint32_t, 4> Vec4ui;
typedef Vec<Vec4ui, 4> Mat4ui;

typedef Vec<double, 4> Vec4d;
typedef Vec<Vec4d, 4> Mat4d;

template <typename T, size_t N>
Vec<Vec<T, N + 1>, N + 1> transform(const Vec<T, N>& a)
{
	Vec<Vec<T, N + 1>, N + 1> mat;

	for (size_t i = 0; i<N; i++)
	{
		//mat[i][N] = a.at(i);
		mat[N][i] = a.at(i);
	}

	return mat;
}

template <typename T, size_t N>
Vec<Vec<T, N + 1>, N + 1> _scale(const Vec<T, N>& a)
{
	Vec<Vec<T, N + 1>, N + 1> mat;

	for (size_t i = 0; i<N; i++)
	{
		mat[i][i] = a.at(i);
	}

	return mat;
}

template <typename T, size_t N>
Vec<Vec<T, N + 1>, N + 1> rotate(const Vec<T, N>& axis, const T& angle)
{
	Vec<Vec<T, N + 1>, N + 1> mat;
	int32_t layer = 0;

	for (size_t i = 0; i<N; i++)
	{
		if (axis.at(i) == 1)
		{
			continue;
		}
		int32_t row = 0;
		for (size_t j = 0; j<N; j++)
		{
			if (axis.at(j) == 1)
			{
				continue;
			}

			if (row == layer)
			{
				mat[i][j] = cosf(-angle);
			}
			else
			{
				T sign = axis.at(1) ? -1 : 1;//y axis is different;
				mat[i][j] = sinf(-angle) * (layer - row) * sign;
			}


			row++;
		}
		layer++;
	}


	return mat;
}

float toRadians(float angle)
{
	return (angle * M_PI) / 180.0f;
}

float toDegree(float angle)
{
	return (angle * 180.0f) / M_PI;
}

Mat4 ModelMatrix(const Vec3& pos, const Vec3& rot, const Vec3& scale)
{
	//static bool once = true;
	Mat4 t = transform(pos);

	Mat4 rx = rotate(Vec3(1, 0, 0), toRadians(rot.x));
	Mat4 ry = rotate(Vec3(0, 1, 0), toRadians(rot.y));
	Mat4 rz = rotate(Vec3(0, 0, 1), toRadians(rot.z));

	Mat4 r = rx * ry * rz;

	Mat4 s = _scale(scale);

	/*if(once)
	{
	std::cout<<"Angle rotation z :"<<toRadians(rot.z)<<std::endl;
	std::cout<<"Rotation x :"<<std::endl<<rx<<std::endl;
	std::cout<<"Rotation y :"<<std::endl<<ry<<std::endl;
	std::cout<<"Rotation z :"<<std::endl<<rz<<std::endl;
	std::cout<<"Rotation :"<<std::endl<<r<<std::endl;
	}*/

	return t * r * s;
}

Mat4 ViewMatrix(const Vec3& eye, const Vec3& target, const Vec3& up)
{
	Vec3 zaxis = (eye - target).getNormalized();//forward
	Vec3 xaxis = (up * zaxis).getNormalized();//right
	Vec3 yaxis = zaxis * xaxis;//up vector

	Mat4 viewMatrix(Vec4(xaxis.x, yaxis.x, zaxis.x, 0),
		Vec4(xaxis.y, yaxis.y, zaxis.y, 0),
		Vec4(xaxis.z, yaxis.z, zaxis.z, 0),
		Vec4(-xaxis.dot(eye), -yaxis.dot(eye), -zaxis.dot(eye), 1));

	return viewMatrix;
}

static Mat4 ProjectionMatrix(float fov, float aspect, float zNear, float zFar)
{
	Mat4 mat;

	float f = 1.0f / tanf(toRadians(fov / 2.0f));

	mat[0][0] = f / aspect;
	mat[1][1] = f;
	mat[2][2] = (zNear + zFar) / (zNear - zFar);
	mat[3][2] = 2.0f * zNear *+zFar / (zNear - zFar);
	mat[2][3] = -1.0f;
	mat[3][3] = 0;


	return mat;
}