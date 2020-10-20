#include"GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	//初始化四个类的指针
	mD3D = NULL;
	mCamera = NULL;
	mLightShader = NULL;
	mDefferredBuffer = NULL;
	mDefferredShader = NULL;
	mBitmap = NULL;
	mModel = NULL;
	mLight = NULL;
}

GraphicsClass::~GraphicsClass()
{

}

GraphicsClass::GraphicsClass(const GraphicsClass&other)
{

}

bool GraphicsClass::Initialize(int ScreenWidth, int ScreenHeight, HWND hwnd)
{
	bool result;
	
	//第一,创建D3DClass类并且初始化,D3DClass应该是第一个被创建并且初始化的类,因为后面的的ColroShaderClass,ModelClass都需要d3dDevice和d3dDeviceContext
	mD3D = new D3DClass();
	if (!mD3D)
	{
		return false;
	}
	result = mD3D->Initialize(ScreenWidth, ScreenHeight, VSYNC_ENABLE, hwnd, FULL_SCREEN, SCREEN_FAR, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"D3DClass Initialize failure", NULL, MB_OK);
		return false;
	}


	//第二,创建CameraClass类
	mCamera = new CameraClass();
	if (!mCamera)
	{
		return false;
	}
	//初始化相机的位置
	mCamera->SetPostion(0.0f, 0.0f, -5.0f); 

	//第三,创建ModelClass并且初始化
	mModel = new ModelClass();
	if (!mModel)
	{
		return false;
	}
	result = mModel->Initialize(mD3D->GetDevice(),(WCHAR*)L"Texture/food.dds","Txt/cube.txt");
	if (!result)
	{
		MessageBox(hwnd, L"ModelClass Initialize failure", NULL, MB_OK);
		return false;
	}

	//第四,创建DefferredShader,并且进行初始化
	mDefferredShader = new DefferredShaderClass();
	if (!mDefferredShader)
	{
		return false;
	}
	result = mDefferredShader->Initialize(mD3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"mDefferredShader Initialize failure", NULL, MB_OK);
		return false;
	}

	//第五,创建mLightShader,并且进行初始化
	mLightShader= new LightShaderClass();
	if (!mLightShader)
	{
		return false;
	}
	result = mLightShader->Initialize(mD3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"mLightShader Initialize failure", NULL, MB_OK);
		return false;
	}

	//第六,创建灯光类
	mLight = new LightClass();
	if (!mLight)
	{
		return false;
	}
	mLight->SetDiffuseColor(XMVectorSet(1.0f, 1.0f, 1.0f,1.0f)); //取四个数
	mLight->SetLightDirection(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)); //仅取三个数  

	//第七,创建DefferredBuffer(这其实就是RTT类,渲染到纹理类)
	mDefferredBuffer = new DefferredBufferClass();
	if (!mDefferredBuffer)
	{
		return false;
	}

	result = mDefferredBuffer->Initialize(mD3D->GetDevice(), ScreenWidth, ScreenHeight, SCREEN_FAR, SCREEN_NEAR);
	if (!result)
	{

		MessageBox(hwnd, L"mDefferredBuffer Initialize failure", NULL, MB_OK);
		return false;
	}

	//第八，创建并初始化BitmapClass
	mBitmap = new BitmapClass();
	if (!mBitmap)
	{
		return false;
	}

	result = mBitmap->Initialize(mD3D->GetDevice(), ScreenWidth, ScreenHeight);
	if (!result)
	{

		MessageBox(hwnd, L"mBitmap Initialize failure", NULL, MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	//释放mLight
	if (mLight)
	{
		delete mLight;
		mLight = NULL;
	}



	//释放mCamera
	if (mCamera)
	{
		delete mCamera;
		mCamera = NULL;
	}

	//释放mModel
	if (mModel)
	{
		mModel->Shutdown();
		delete mModel;
		mModel = NULL;
	}

	//释放mLightShader
	if (mLightShader)
	{
		mLightShader->Shutdown();
		delete mLightShader;
		mLightShader = NULL;
	}

	//释放mDefferredBuffer
	if (mDefferredBuffer)
	{
		mDefferredBuffer->ShutDown();
		delete mDefferredBuffer;
		mDefferredBuffer = NULL;
	}

	//释放BitmapClass
	if (mBitmap)
	{
		mBitmap->Shutdown();
		delete mBitmap;
		mBitmap = NULL;
	}


	//释放mDefferredShader
	if (mDefferredShader)
	{
		mDefferredShader->Shutdown();
		delete mDefferredShader;
		mDefferredShader = NULL;
	}

	//释放mD3D
	if (mD3D)
	{
		mD3D->Shutdown();
		delete mD3D;
		mD3D = NULL;
	}
}



bool GraphicsClass::Frame()
{
	bool result;
	static float rotation = 0.0f;
	rotation += XM_PI*0.004f;
	if (rotation > 360.0f)
		rotation -= 360.0f;

	//设置相机的位置
	mCamera->SetPostion(0.0f, 0.0f, -10.0f);

	//进行渲染
	result = Render(rotation);
	if (!result)
		return false;


	return true;
}

bool GraphicsClass::Render(float rotation)
{
	bool result;
	
	//第一,将3D场景渲染到两张纹理上，一张为颜色纹理，一张为法线纹理(存储直接的法向量信息)
	result = RenderSceneToTexture(rotation);
	if (!result)
	{
		MessageBox(NULL, L"RenderSceneToTexture failure", NULL, MB_OK);
		return false;
	}

	//第二,将2D纹理打印到背后缓存
	result = RenderTextureToBack();
	if (!result)
	{
		MessageBox(NULL, L"RenderTextureToBack failure", NULL, MB_OK);
		return false;
	}

	return true;
}




bool GraphicsClass::RenderSceneToTexture(float rotation)
{
	//三个变换矩阵
	XMMATRIX WorldMatrix, ViewMatrix, ProjMatrix;
	bool result;
	//第一,清除缓存开始绘制场景
	mD3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//第一，将渲染目标变为纹理
	mDefferredBuffer->SetRenderTarget(mD3D->GetDeviceContext());

	//第二,清除纹理的缓存
	mDefferredBuffer->ClearRenderTarget(mD3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//第三,(更新)获取ViewMatrix(根据CameraClass的mPostion和mRotation来生成的)
	mCamera->Render();

	//第四,获取三个变换矩阵(WorldMatrix和ProjMatrix来自mD3D类,ViewMatrix来自CameraClass)

	WorldMatrix = mD3D->GetWorldMatrix();

	ProjMatrix = mD3D->GetProjMatrix();

	ViewMatrix = mCamera->GetViewMatrix();

	//顺时针旋转世界矩阵
	WorldMatrix = WorldMatrix*XMMatrixRotationY(rotation);

	//第五,将3D模型的顶点数据和索引数据放入3D渲染流水线
	mModel->Render(mD3D->GetDeviceContext());

	//第六,用DefferredShader进行绘制
	result = mDefferredShader->Render(mD3D->GetDeviceContext(), mModel->GetIndexCount(), WorldMatrix, ViewMatrix, ProjMatrix, mModel->GetTexture());
	if (!result)
	{
		MessageBox(NULL, L"mDefferredShaderRender failure", NULL, MB_OK);
		return false;
	}

	//第七,重置设置渲染目标为背后缓存
	mD3D->SetBackBufferRender();

	//第八，重新设置视口
	mD3D->SetViewPort();

	return true;
}
bool GraphicsClass::RenderTextureToBack()
{
     //三个变换矩阵
     XMMATRIX WorldMatrix, BaseViewMatrix, OrthoMatrix;
    bool result;

	//第一,清除缓存开始绘制场景
	mD3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//第二,生成基础相机变换矩阵
	mCamera->Render();

	//第三,获取三个变换矩阵(WorldMatrix和ProjMatrix来自mD3D类,ViewMatrix来自CameraClass)
	WorldMatrix = mD3D->GetWorldMatrix();

	OrthoMatrix = mD3D->GetOrthoMatrix();

	BaseViewMatrix = mCamera->GetViewMatrix();

	//第四，关闭Z缓存测试
	mD3D->TurnOffZBuffer();

	//第五,把2D图片的数据放入3D渲染流水线
	mBitmap->Render(mD3D->GetDeviceContext());

	//第六，用LightShader进行渲染
	result = mLightShader->Render(mD3D->GetDeviceContext(), mBitmap->GetIndexCount(), WorldMatrix, BaseViewMatrix, OrthoMatrix, mDefferredBuffer->GetShaderResourceView(0), mDefferredBuffer->GetShaderResourceView(1), mLight->GetDiffuseColor(), mLight->GetLightDirection());
	if (!result)
	{
		MessageBox(NULL, L"mLightShader failure", NULL, MB_OK);
		return false;
	}

	//第七，关闭Z缓存测试
	mD3D->TurnOnZBuffer();
    
	//把渲染的场景呈献给屏幕
	mD3D->EndScene();

	return true;
}