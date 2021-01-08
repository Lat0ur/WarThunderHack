#pragma once
#include <Windows.h>
#include <cmath>


#define P(x) (intptr_t*)x
#define I(x) (intptr_t)x
#define C(x) (char*)x
#define UP(x) (uintptr_t*)x
#define UI(x) (uintptr_t)x

//game specific defines
//angles
#define YAW 0  //x
#define ROLL 1 //y, normally this is Z
#define PITCH 2 //z

//position
#define POS_FORWARD 0  //x
#define POS_VERTICAL 1 //y, normally this is Z
#define POS_STRAFE 2 //z

class vec3
{
public:

	union
	{
		struct { float x, y, z; };
		float v[3];
	};

	vec3() { x = y = z = 0; }
	vec3(const float x, const float y, const float z) : x(x), y(y), z(z) {}
	vec3 operator + (const vec3& rhs) const { return vec3(x + rhs.x, y + rhs.y, z + rhs.z); }
	vec3 operator - (const vec3& rhs) const { return vec3(x - rhs.x, y - rhs.y, z - rhs.z); }
	vec3 operator * (const float& rhs) const { return vec3(x * rhs, y * rhs, z * rhs); }
	vec3 operator / (const float& rhs) const { return vec3(x / rhs, y / rhs, z / rhs); }
	vec3& operator += (const vec3& rhs) { return *this = *this + rhs; }
	vec3& operator -= (const vec3& rhs) { return *this = *this - rhs; }
	vec3& operator *= (const float& rhs) { return *this = *this * rhs; }
	vec3& operator /= (const float& rhs) { return *this = *this / rhs; }
	float dot() const { return x * x + y * y + z * z; }
	float Length() const { return sqrtf(dot()); }
	vec3 Normalize() const { return *this * (1 / Length()); }
	float Distance(const vec3& rhs) const { return (*this - rhs).Length(); }
};

using Vector3 = vec3;

struct vec4 { float x, y, z, w; };

typedef struct {
	float element[4][4];
} Matrix44;

struct D3DX11Matricies {
	Matrix44 ViewMatrix;
	Matrix44 WorldMatrix;
	Matrix44 ProjectionMatrix;
};