#include "w2sAndUtils.h"
#include "DirectXMath.h"
#include "stdio.h"

vec3 Subtract(vec3 src, vec3 dst) {
	vec3 diff;
	diff.x = src.x - dst.x;
	diff.y = src.y - dst.y;
	diff.z = src.z - dst.z;
	return diff;
}

vec3 Divide(vec3 src, float num) {
	vec3 vec;
	vec.x = src.x / num;
	vec.y = src.y / num;
	vec.z = src.z / num;

	return vec;
}

float DotProduct(vec3 src, vec3 dst)
{
	return src.x * dst.x + src.y * dst.y + src.z * dst.z;
}

DirectX::XMMATRIX Multiplier = { 1, 0, 0, 0,
								0, 1, 0, 0, 
								0, 0, 1, 0,
								0, 0, 0, 1 };
bool DirectXWorldToScreen(vec3 point, vec3& screen, D3DX11Matricies* matrixies, float width, float height)
{
	DirectX::XMFLOAT3 out_2d;

	DirectX::XMVECTOR out = DirectX::XMVector3Project(DirectX::XMVectorSet(point.x, point.y, point.z, 0.f),
		0, 0, width, height, 0, 1, XMMatrixMultiply(matrixies->ProjectionMatrix, Multiplier),
		matrixies->ViewMatrix, (matrixies->WorldMatrix));

	DirectX::XMStoreFloat3(&out_2d, out);
	//printf("%4.2f %4.2f %4.2f\r\n", out_2d.x, out_2d.y, out_2d.z);
	float* matrix = (float*)matrixies;
	
	if (out_2d.z > 0.f) {
		screen.x = out_2d.x;
		screen.y = out_2d.y;
		//printf("%4.2f %4.2f %4.2f %4.2f -- %4.2f %4.2f\r\n", out_2d.x, out_2d.y, matrix[18], matrix[0], screen.x, screen.y);
		return true;
	}
		
	return false;
}

bool RotateDot(float* rotationMatrix, vec3 originDot, vec3& rotatedDot) {
	rotatedDot.x = originDot.x * rotationMatrix[0] + originDot.y * rotationMatrix[3] + originDot.z * rotationMatrix[6];
	rotatedDot.y = originDot.x * rotationMatrix[1] + originDot.y * rotationMatrix[4] + originDot.z * rotationMatrix[7];
	rotatedDot.z = originDot.x * rotationMatrix[2] + originDot.y * rotationMatrix[5] + originDot.z * rotationMatrix[8];

	return true;
}