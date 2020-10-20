#pragma once

#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <xnamath.h>

class CameraClass
{
private:
	float mPostionX, mPostionY, mPostionZ; //相机在世界空间的位置
	float mRotationX, mRotationY, mRotationZ; //相机在局部空间旋转的float值(用于算出旋转欧拉角)
	XMMATRIX mViewMatrix;
	XMMATRIX mBaseViewMatrix;  //绘制2D文字(2D Rendering)的BaseViewMatrix

public:
	CameraClass()
	{
		mPostionX = 0;
		mPostionY = 0;
		mPostionZ = -5.0f;
		mRotationX = 0;
		mRotationY = 0;
		mRotationZ = 0;
	}

	CameraClass(const CameraClass&other)
	{

	}
	~CameraClass()
	{

	}

public:

	void SetPostion(float x, float y, float z)
	{
		mPostionX = x;
		mPostionY = y;
		mPostionZ = z;
	}

	void SetRotation(float x, float y, float z)
	{
		mRotationX = x;
		mRotationY = y;
		mRotationZ = z;
	}



	XMVECTOR GetPostion()
	{

		return XMVectorSet(mPostionX, mPostionY, mPostionZ, 0.0f);
	}

	XMVECTOR GetRotation()
	{
		return XMVectorSet(mRotationX, mRotationY, mRotationZ, 0.0f);
	}


	void Render()
	{
		//上向量,位置向量,观察向量
		XMVECTOR Up, Postion, LookAt;
		float yaw, pitch, roll; //欧拉角


								//设置上向量
		Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		//设置相机的位置
		Postion = XMVectorSet(mPostionX, mPostionY, mPostionZ, 0.0f);

		//设置相机默认情况下看哪的
		LookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

		//设置 yaw(Y轴), pitch(X轴), roll(Z轴),为弧度值
		pitch = mRotationX*0.0174532925f;
		yaw = mRotationY*0.0174532925f;
		roll = mRotationZ*0.0174532925f;

		//创建旋转矩阵，并旋转局部空间相机的Up向量和LookAt向量
		XMMATRIX RotateMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
		LookAt = XMVector3TransformCoord(LookAt, RotateMatrix);
		Up = XMVector3TransformCoord(Up, RotateMatrix);

		//将相机从局部模型空间变为世界空间
		LookAt = LookAt + Postion;

		//最后创建ViewMatrix
		mViewMatrix = XMMatrixLookAtLH(Postion, LookAt, Up);



		//创建BaseViewMatrix

		//设置相机的位置
		Postion = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);

		LookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);


		//将相机从局部模型空间变为世界空间
		LookAt = LookAt + Postion;

		//最后创建ViewMatrix
		mBaseViewMatrix = XMMatrixLookAtLH(Postion, LookAt, Up);
	}

	XMMATRIX GetViewMatrix()
	{
		return mViewMatrix;
	}

	XMMATRIX GetBaseViewMatrix()
	{

		return mBaseViewMatrix;
	}

};


#endif 
