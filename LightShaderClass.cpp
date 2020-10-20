#include"LightShaderClass.h"

LightShaderClass::LightShaderClass()
{
	 md3dVertexShader=NULL;
	 md3dPixelShader=NULL;
	 md3dInputLayout=NULL;
	 mCBMatrixBuffer=NULL;
	 mCBLightBuffer = NULL;
}


LightShaderClass::LightShaderClass(const LightShaderClass& other)
{

}

LightShaderClass::~LightShaderClass()
{

}


bool LightShaderClass::Initialize(ID3D11Device* d3dDevice, HWND hwnd)
{
	bool result;
	result = InitializeShader(d3dDevice, hwnd, (WCHAR*)L"MyShader/LightShader.fx", (WCHAR*)L"MyShader/LightShader.fx");
	if (!result)
		return false;

	return true;
}

void LightShaderClass::Shutdown()
{
	ShutdownShader();
}

bool LightShaderClass::Render(ID3D11DeviceContext* d3dDeviceContext, int indexCount, CXMMATRIX worldMatrix, CXMMATRIX viewMatrix, CXMMATRIX ProjMatrix, ID3D11ShaderResourceView* ColorTexture, ID3D11ShaderResourceView* NormalTexture, FXMVECTOR diffuseColor, FXMVECTOR LightDirection)
{
	bool result;

	//����Shader���������������Դ
	result = SetShaderParameter(d3dDeviceContext, worldMatrix, viewMatrix, ProjMatrix, ColorTexture,NormalTexture,diffuseColor,LightDirection);
	if (!result)
		return false;

	//����VertexShader PixelShader InputLayout SamplerState
	RenderShader(d3dDeviceContext, indexCount);
	
	return true;
}

bool LightShaderClass::InitializeShader(ID3D11Device* d3dDevice, HWND hwnd, WCHAR* VSFileName, WCHAR* PSFileName)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* VertexShaderBuffer;  
	ID3D10Blob* PixelShaderBuffer;
	
	//��һ,��ʼ������
	errorMessage = NULL;
    VertexShaderBuffer=NULL;
	PixelShaderBuffer=NULL;

	//�ڶ�,����VertexShader����,������VertexShader
	result = D3DX11CompileFromFile(VSFileName, NULL, NULL, "VS", "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, NULL, &VertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		//���ڴ�����Ϣ
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, VSFileName);
		}
		//�����ڴ�����Ϣ,Ҳ����û���ҵ�Shader�ļ�
		else
		{
			MessageBox(hwnd, L"can not find VS file", L"error", MB_OK);
		}
	}

	HR(d3dDevice->CreateVertexShader(VertexShaderBuffer->GetBufferPointer(),VertexShaderBuffer->GetBufferSize(),NULL,&md3dVertexShader));


	//����,����PixelShader,������PixelShader
	result = D3DX11CompileFromFile(PSFileName, NULL, NULL, "PS", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, NULL, &PixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		//���ڴ�����Ϣ
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, PSFileName);
		}
		//�����ڴ�����Ϣ,Ҳ����û���ҵ�Shader�ļ�
		else
		{
			MessageBox(hwnd, L"can not find PS file", L"error", MB_OK);
		}
	}

	HR(d3dDevice->CreatePixelShader(PixelShaderBuffer->GetBufferPointer(), PixelShaderBuffer->GetBufferSize(), NULL, &md3dPixelShader));

	//����,������벼�����ݽṹ��,�������벼��
	D3D11_INPUT_ELEMENT_DESC VertexInputLayout[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 }, // 96λ��12���ֽ�
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,20,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	unsigned int numElements = sizeof(VertexInputLayout) / sizeof(VertexInputLayout[0]);         //��������
	
	HR(d3dDevice->CreateInputLayout(VertexInputLayout, numElements, VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), &md3dInputLayout));

	//����,�ͷ�VertexShaderBuffer��PixelShaderBuffer
	VertexShaderBuffer->Release();
	VertexShaderBuffer = NULL;
	PixelShaderBuffer->Release();
	PixelShaderBuffer = NULL;

	//����,����(�任������)�������ݽṹ��,����������������
	D3D11_BUFFER_DESC matrixBufferDesc;
	ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));
	matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	matrixBufferDesc.ByteWidth = sizeof(CBMatrix);   //�ṹ���С,����Ϊ16�ֽڱ���
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = 0;

	HR(d3dDevice->CreateBuffer(&matrixBufferDesc, NULL, &mCBMatrixBuffer));
	

	//����,���ã��ƹⳣ�����棩�������ݽṹ�壬�������ƹⳣ������,����ճ����ʱ����LightBufferDesc��matrixBufferDesc�����
	D3D11_BUFFER_DESC LightBufferDesc;
	ZeroMemory(&LightBufferDesc, sizeof(LightBufferDesc));
	LightBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	LightBufferDesc.ByteWidth = sizeof(CBLight);   //�ṹ���С,����Ϊ16�ֽڱ���
	LightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	LightBufferDesc.CPUAccessFlags = 0;

	HR(d3dDevice->CreateBuffer(&LightBufferDesc,NULL,&mCBLightBuffer));


	//�ڰ�,���������ݽṹ��,���Ҵ�������״̬
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;  //�������Բ�ֵ(���ַ�ʽ,�����,���Թ���,���Թ���)
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HR(d3dDevice->CreateSamplerState(&samplerDesc, &mSamplerState));

    return true;
}

bool LightShaderClass::ShutdownShader()
{
	HR(mCBMatrixBuffer);
	HR(mCBLightBuffer);
	HR(md3dInputLayout);
	HR(md3dPixelShader);
	HR(md3dVertexShader);
	return true;
}

void LightShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// ��ȡָ�������Ϣ�ı���ָ��
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// ��ȡ������Ϣ�ı��ĳ���
	bufferSize = errorMessage->GetBufferSize();

	// ����һ��txt,����д�������Ϣ
	fout.open("shader-error.txt");

	//��txt�ļ�д�������Ϣ
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// �ر��ļ�
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

    //�������ѵ�С����
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

}


bool LightShaderClass::SetShaderParameter(ID3D11DeviceContext* d3dDeviceContext, CXMMATRIX worldMatrix, CXMMATRIX viewMatrix, CXMMATRIX ProjMatrix, ID3D11ShaderResourceView* ColorTexture, ID3D11ShaderResourceView* NormalTexture, FXMVECTOR diffuseColor, FXMVECTOR LightDirection)
{

	//��һ�����±任�����������ֵ
	//������ת��,�ڴ��볣������ǰ����ת��,��ΪGPU�Ծ������ݻ��Զ�����һ��ת��
	CBMatrix cb;
	XMMATRIX worldMa = XMMatrixTranspose(worldMatrix);
	XMMATRIX viewMa = XMMatrixTranspose(viewMatrix);
	XMMATRIX ProjMa = XMMatrixTranspose(ProjMatrix);
	cb.mWorldMatrix = worldMa;
	cb.mViewMatrix = viewMa;
	cb.mProjMatrix = ProjMa;
	d3dDeviceContext->UpdateSubresource(mCBMatrixBuffer, 0, NULL, &cb, 0, 0);


	//�������㻺��  
	d3dDeviceContext->Unmap(mCBMatrixBuffer, 0);
	
	//�ڶ������µƹⳣ�������ֵ
	//�����ڶ��㻺���г��������λ��,ע���
	CBLight cbLight;
	XMStoreFloat4(&cbLight.diffuseColor, diffuseColor);
	XMStoreFloat3(&cbLight.LightDireciton, LightDirection);
	cbLight.pad = 0.0f;
	d3dDeviceContext->UpdateSubresource(mCBLightBuffer, 0, NULL, &cbLight, 0, 0);


	//����,������VertexShader�ĳ��������ֵ(���Ÿ��µ�ֵ)
	d3dDeviceContext->VSSetConstantBuffers(0, 1, &mCBMatrixBuffer); 

	//����,������PixelShader�ĳ��������ֵ
	d3dDeviceContext->PSSetConstantBuffers(1, 1, &mCBLightBuffer);

	//����,,������PixelShader��������Դ
	d3dDeviceContext->PSSetShaderResources(0, 1, &ColorTexture);
	d3dDeviceContext->PSSetShaderResources(1, 1, &NormalTexture);

	return true;
}

void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//���ö������벼��
	deviceContext->IASetInputLayout(md3dInputLayout);

	//����VertexShader��PixelShader
	deviceContext->VSSetShader(md3dVertexShader, NULL, 0);
	deviceContext->PSSetShader(md3dPixelShader, NULL, 0);

	//���ò���״̬
	deviceContext->PSSetSamplers(0, 1, &mSamplerState);  //S0ע�� ��Ӧ0

	//��Ⱦ������
	deviceContext->DrawIndexed(indexCount, 0, 0);
}


//����һ�������������ת��
XMMATRIX LightShaderClass::GetInvenseTranspose(CXMMATRIX ma)
{
	XMMATRIX A = ma;

	//������A�ĵ���������,��Ϊ�������޷�����ƽ�Ƶ�
	A.r[3] = { 0.0f,0.0f,0.0f,1.0f };

	XMVECTOR det = XMMatrixDeterminant(A);
	XMMATRIX MaInvense = XMMatrixInverse(&det, A);
	return XMMatrixTranspose(MaInvense);
}