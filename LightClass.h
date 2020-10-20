#pragma once
#ifndef _LIGHT_CLASS
#define  _LIGHT_CLASS

#include<Windows.h>
#include <xnamath.h>


//��CameraClass���÷����
class LightClass
{
private:
	XMFLOAT4 mDiffuseColor;
	XMFLOAT3 mLightDiretion;

public:
	LightClass();
	LightClass(const LightClass&other);
	~LightClass();

public:
	void SetDiffuseColor(FXMVECTOR diffuseColor);
	void SetLightDirection(FXMVECTOR lightDirection);
	XMVECTOR GetDiffuseColor();
	XMVECTOR GetLightDirection();

};
#endif // !_LIGHT_CLASS
