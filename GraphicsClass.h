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


//全局变量
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLE = true;  //是尽可能快渲染还是限制帧渲染
const float SCREEN_FAR = 1000.0f;  //视截体远裁面
const float SCREEN_NEAR = 0.1f;  //视截体近裁面



class GraphicsClass
{

private:
	//D3D类
	D3DClass* mD3D;

	//相机类，用于控制场景的相机
	CameraClass* mCamera; 

	//LightShader
	LightShaderClass* mLightShader;

	//DefferredShader
	DefferredShaderClass* mDefferredShader;

	//DefferredBuffer
	DefferredBufferClass* mDefferredBuffer;

	//BitmapClass
	BitmapClass* mBitmap;

	//用于控制VertexBuffer和IndexBuffer
	ModelClass* mModel;

	//包含灯光的参数
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
