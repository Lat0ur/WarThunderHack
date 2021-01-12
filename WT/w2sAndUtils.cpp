#include "w2sAndUtils.h"
#include "DirectXMath.h"
#include "stdio.h"
/*
bool WorldToScreen_(vec3 worldPos, float* matrix, vec3& screenPos) {
	vec3 transform;
	float xc, yc;
	float px, py;
	float z;

	px = tan(nys.fov.x * PI / 360.0);
	py = tan(nys.fov.y * PI / 360.0);

	transform = Subtract(worldPos, nys.camPos);

	xc = 1024 / 2.0; // 1024 is hardcoded because I'm testing with that resolution.
	yc = 768 / 2.0; // 768 is hardcoded because I'm testing with that resolution.

	z = DotProduct(transform, nys.vMatrix[0]);

	if (z <= 0.1) {
		return false;
	}

	screenPos.x = xc - DotProduct(transform, nys.vMatrix[1]) * xc / (z * px);
	screenPos.y = yc - DotProduct(transform, nys.vMatrix[2]) * yc / (z * py);

	return true;
}*/

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

bool WorldToScreen0(vec3 pos, vec3& screen, float* matrix, float windowWidth, float windowHeight)
{
	 // Get matrix...

	//printf("%4.2f %4.2f %4.2f", matrix[0], matrix[1], matrix[2]);

	float _width = 1024;
	float _height = 768;
	float cW = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];


	bool visible = cW >= 0.1f;
	if (!visible)
		cW = -1.0f / cW;
	else
		cW = 1.0f / cW;

	float cX = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
	float cY = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
	float cZ = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
	
	float nx = cX / cW;
	float ny = cY / cW;
	float nz = cZ / cW;
	
	screen.x = (_width / 2 * nx) + (nx + _width / 2);
	screen.y = -(_height / 2 * ny) + (ny + _height / 2);
	//printf("%4.2f * %4.2f, %4.2f * %4.2f, %4.2f * %4.2f, %4.2f = %4.2f\r\n", worldPosition.x, matrix[3], worldPosition.y, matrix[7], worldPosition.z, matrix[11], matrix[15], cW);
	printf("%4.2f %4.2f %4.2f %4.2f -- %4.2f %4.2f\r\n", cW, cX, nx, matrix[3], screen.x, screen.y);
	return visible;
}

bool WorldToScreen1(vec3 pos, vec3& screen, float* matrix, float windowWidth, float windowHeight)
{
	//Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
	vec4 clipCoords;

	clipCoords.w = pos.v[POS_FORWARD] * matrix[3] + pos.v[POS_VERTICAL] * matrix[7] + pos.v[POS_STRAFE] * matrix[11] + matrix[15];

	bool visible = clipCoords.w >= 0.1f;
	if (!visible)
		clipCoords.w = -clipCoords.w;
	else
		clipCoords.w = clipCoords.w;

	clipCoords.x = pos.v[POS_FORWARD] * matrix[0] + pos.v[POS_VERTICAL] * matrix[4] + pos.v[POS_STRAFE] * matrix[8] + matrix[12];
	clipCoords.y = pos.v[POS_FORWARD] * matrix[1] + pos.v[POS_VERTICAL] * matrix[5] + pos.v[POS_STRAFE] * matrix[9] + matrix[13];

	//perspective division, dividing by clip.W = Normalized Device Coordinates
	vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;

	//convert NDC to screen coordinates
	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	screen.z = 0;
	screen = screen.Normalize();
	printf("%4.2f %4.2f %4.2f %4.2f -- %4.2f %4.2f\r\n", matrix[0], matrix[1], matrix[2], NDC.x, screen.x, screen.y);
	return true;
}

//Both W2S functions produce the same output
bool WorldToScreen2(vec3 pos, vec3& screen, float* matrix, float windowWidth, float windowHeight)
{
	float matrix2[4][4];

	memcpy(matrix2, matrix, 16 * sizeof(float));

	float mX = windowWidth / 2;
	float mY = windowHeight / 2;

	float w =
		matrix2[0][3] * pos.x +
		matrix2[1][3] * pos.y +
		matrix2[2][3] * pos.z +
		matrix2[3][3];

	if (w < 0.65f)
		return false;

	float x =
		matrix2[0][0] * pos.x +
		matrix2[1][0] * pos.y +
		matrix2[2][0] * pos.z +
		matrix2[3][0];

	float y =
		matrix2[0][1] * pos.x +
		matrix2[1][1] * pos.y +
		matrix2[2][1] * pos.z +
		matrix2[3][1];

	screen.x = (mX + mX * x / w);
	screen.y = (mY - mY * y / w);
	screen.z = 0;

	return true;
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