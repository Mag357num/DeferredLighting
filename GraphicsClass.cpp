#include"GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	//��ʼ���ĸ����ָ��
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
	
	//��һ,����D3DClass�ಢ�ҳ�ʼ��,D3DClassӦ���ǵ�һ�����������ҳ�ʼ������,��Ϊ����ĵ�ColroShaderClass,ModelClass����Ҫd3dDevice��d3dDeviceContext
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


	//�ڶ�,����CameraClass��
	mCamera = new CameraClass();
	if (!mCamera)
	{
		return false;
	}
	//��ʼ�������λ��
	mCamera->SetPostion(0.0f, 0.0f, -5.0f); 

	//����,����ModelClass���ҳ�ʼ��
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

	//����,����DefferredShader,���ҽ��г�ʼ��
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

	//����,����mLightShader,���ҽ��г�ʼ��
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

	//����,�����ƹ���
	mLight = new LightClass();
	if (!mLight)
	{
		return false;
	}
	mLight->SetDiffuseColor(XMVectorSet(1.0f, 1.0f, 1.0f,1.0f)); //ȡ�ĸ���
	mLight->SetLightDirection(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)); //��ȡ������  

	//����,����DefferredBuffer(����ʵ����RTT��,��Ⱦ��������)
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

	//�ڰˣ���������ʼ��BitmapClass
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
	//�ͷ�mLight
	if (mLight)
	{
		delete mLight;
		mLight = NULL;
	}



	//�ͷ�mCamera
	if (mCamera)
	{
		delete mCamera;
		mCamera = NULL;
	}

	//�ͷ�mModel
	if (mModel)
	{
		mModel->Shutdown();
		delete mModel;
		mModel = NULL;
	}

	//�ͷ�mLightShader
	if (mLightShader)
	{
		mLightShader->Shutdown();
		delete mLightShader;
		mLightShader = NULL;
	}

	//�ͷ�mDefferredBuffer
	if (mDefferredBuffer)
	{
		mDefferredBuffer->ShutDown();
		delete mDefferredBuffer;
		mDefferredBuffer = NULL;
	}

	//�ͷ�BitmapClass
	if (mBitmap)
	{
		mBitmap->Shutdown();
		delete mBitmap;
		mBitmap = NULL;
	}


	//�ͷ�mDefferredShader
	if (mDefferredShader)
	{
		mDefferredShader->Shutdown();
		delete mDefferredShader;
		mDefferredShader = NULL;
	}

	//�ͷ�mD3D
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

	//���������λ��
	mCamera->SetPostion(0.0f, 0.0f, -10.0f);

	//������Ⱦ
	result = Render(rotation);
	if (!result)
		return false;


	return true;
}

bool GraphicsClass::Render(float rotation)
{
	bool result;
	
	//��һ,��3D������Ⱦ�����������ϣ�һ��Ϊ��ɫ����һ��Ϊ��������(�洢ֱ�ӵķ�������Ϣ)
	result = RenderSceneToTexture(rotation);
	if (!result)
	{
		MessageBox(NULL, L"RenderSceneToTexture failure", NULL, MB_OK);
		return false;
	}

	//�ڶ�,��2D�����ӡ�����󻺴�
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
	//�����任����
	XMMATRIX WorldMatrix, ViewMatrix, ProjMatrix;
	bool result;
	//��һ,������濪ʼ���Ƴ���
	mD3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//��һ������ȾĿ���Ϊ����
	mDefferredBuffer->SetRenderTarget(mD3D->GetDeviceContext());

	//�ڶ�,�������Ļ���
	mDefferredBuffer->ClearRenderTarget(mD3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	//����,(����)��ȡViewMatrix(����CameraClass��mPostion��mRotation�����ɵ�)
	mCamera->Render();

	//����,��ȡ�����任����(WorldMatrix��ProjMatrix����mD3D��,ViewMatrix����CameraClass)

	WorldMatrix = mD3D->GetWorldMatrix();

	ProjMatrix = mD3D->GetProjMatrix();

	ViewMatrix = mCamera->GetViewMatrix();

	//˳ʱ����ת�������
	WorldMatrix = WorldMatrix*XMMatrixRotationY(rotation);

	//����,��3Dģ�͵Ķ������ݺ��������ݷ���3D��Ⱦ��ˮ��
	mModel->Render(mD3D->GetDeviceContext());

	//����,��DefferredShader���л���
	result = mDefferredShader->Render(mD3D->GetDeviceContext(), mModel->GetIndexCount(), WorldMatrix, ViewMatrix, ProjMatrix, mModel->GetTexture());
	if (!result)
	{
		MessageBox(NULL, L"mDefferredShaderRender failure", NULL, MB_OK);
		return false;
	}

	//����,����������ȾĿ��Ϊ���󻺴�
	mD3D->SetBackBufferRender();

	//�ڰˣ����������ӿ�
	mD3D->SetViewPort();

	return true;
}
bool GraphicsClass::RenderTextureToBack()
{
     //�����任����
     XMMATRIX WorldMatrix, BaseViewMatrix, OrthoMatrix;
    bool result;

	//��һ,������濪ʼ���Ƴ���
	mD3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//�ڶ�,���ɻ�������任����
	mCamera->Render();

	//����,��ȡ�����任����(WorldMatrix��ProjMatrix����mD3D��,ViewMatrix����CameraClass)
	WorldMatrix = mD3D->GetWorldMatrix();

	OrthoMatrix = mD3D->GetOrthoMatrix();

	BaseViewMatrix = mCamera->GetViewMatrix();

	//���ģ��ر�Z�������
	mD3D->TurnOffZBuffer();

	//����,��2DͼƬ�����ݷ���3D��Ⱦ��ˮ��
	mBitmap->Render(mD3D->GetDeviceContext());

	//��������LightShader������Ⱦ
	result = mLightShader->Render(mD3D->GetDeviceContext(), mBitmap->GetIndexCount(), WorldMatrix, BaseViewMatrix, OrthoMatrix, mDefferredBuffer->GetShaderResourceView(0), mDefferredBuffer->GetShaderResourceView(1), mLight->GetDiffuseColor(), mLight->GetLightDirection());
	if (!result)
	{
		MessageBox(NULL, L"mLightShader failure", NULL, MB_OK);
		return false;
	}

	//���ߣ��ر�Z�������
	mD3D->TurnOnZBuffer();
    
	//����Ⱦ�ĳ������׸���Ļ
	mD3D->EndScene();

	return true;
}