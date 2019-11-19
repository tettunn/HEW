#pragma once
#ifndef CAMERA_H_
#define CAMERA_H_
#include<d3dx9.h>

#define AT_L (10.0f)

typedef struct LocalVecter_Tag {
	D3DXVECTOR3 Front;
	D3DXVECTOR3 Right;
	D3DXVECTOR3 Up;
}LocalVecter;

typedef struct Camera_Tag {
	LocalVecter LocalVec;
	D3DXVECTOR3 Pos;
	float Fov;
	float MoveSpeed;
	float RotationSpeed;
}Camera;

void Camera_Init();
void Camera_Update();

Camera Camera_GetPosition(void);

#endif // !CAMERA_H_
