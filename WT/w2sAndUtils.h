#pragma once
#include <Windows.h>
#include <cmath>
#include <DirectXMath.h>
#include "imgui.h"

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

	float x, y, z;

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
	float operator[](int i) const {
		return ((float*)this)[i];
	}
	float& operator[](int i);
	float dot() const { return x * x + y * y + z * z; }
	float Length() const { return sqrtf(dot()); }
	vec3 Normalize() const { return *this * (1 / Length()); }
	float Distance(const vec3& rhs) const { return (*this - rhs).Length(); }
	float calcAngle(vec3& vec1, vec3& vec2) {
		return acos((vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z) / (vec1.Length() * vec2.Length()));
	}
};

using Vector3 = vec3;

struct vec4 { float x, y, z, w; };

typedef struct {
	float element[4][4];
} Matrix44;


struct D3DX11Matricies {
	DirectX::XMMATRIX ViewMatrix;
	DirectX::XMMATRIX WorldMatrix;
	DirectX::XMMATRIX ProjectionMatrix;
};


bool DirectXWorldToScreen(vec3 point, vec3& screen, D3DX11Matricies* matrixies, float width, float height);
bool RotateDot(float* rotationMatrix, vec3 originDot, vec3& rotatedDot);
