#pragma once
#ifndef _GRAPHICS_CLASS_H
#define _GRAPHICS_CLASS_H

#include"D3DClass.h"
#include"CameraClass.h"
#include"LightShaderClass.h"
#include"DefferredBufferClass.h"
#include"DefferredShaderClass.h"
#include"ModelClass.h"
#include"LightClass.h"
#include"BitmapClass.h"


//ȫ�ֱ���
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLE = true;  //�Ǿ����ܿ���Ⱦ��������֡��Ⱦ
const float SCREEN_FAR = 1000.0f;  //�ӽ���Զ����
const float SCREEN_NEAR = 0.1f;  //�ӽ��������



class GraphicsClass
{

private:
	//D3D��
	D3DClass* mD3D;

	//����࣬���ڿ��Ƴ��������
	CameraClass* mCamera; 

	//LightShader
	LightShaderClass* mLightShader;

	//DefferredShader
	DefferredShaderClass* mDefferredShader;

	//DefferredBuffer
	DefferredBufferClass* mDefferredBuffer;

	//BitmapClass
	BitmapClass* mBitmap;

	//���ڿ���VertexBuffer��IndexBuffer
	ModelClass* mModel;

	//�����ƹ�Ĳ���
	LightClass* mLight;

private:
	bool Render(float);
	bool RenderSceneToTexture(float);
	bool RenderTextureToBack();

public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

public:
	bool Initialize(int ScreenWidth, int ScreenHeight, HWND hwnd);
	void Shutdown();
	bool Frame(); 
};
#endif // !_GRAPHICS_CLASS_H
