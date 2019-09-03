#include "Graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	this->windowWidth = width;
	this->windowHeight = height;

	if (!InitializeDirectX(hwnd))
	{
		return false;
	}

	if (!InitializeShaders())
	{
		return false;
	}

	if (!InitializeScene())
	{
		return false;
	}

	return true;
}

void Graphics::RenderFrame()
{
	float bgcolor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	this->context->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);
	this->context->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->context->IASetInputLayout(this->vertexShader.GetInputLayout());
	this->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	this->context->VSSetShader(vertexShader.GetShader(), NULL, 0);
	this->context->PSSetShader(pixelShader.GetShader(), NULL, 0);

	this->context->OMSetDepthStencilState(this->depthStencilState.Get(), 0);

	this->context->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());

	this->context->PSSetShaderResources(0, 1, this->myTexture.GetAddressOf());

	//world matrix
	XMMATRIX world = XMMatrixIdentity();

	this->camera.AdjustPosition(0.01f, 0.0f, 0.0f);

	XMFLOAT3 lookAtPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->camera.SetLookAtPosition(lookAtPos);

	this->constantBuffer.data.mat = world * this->camera.GetViewMatrix() * this->camera.GetProjectionMatrix(); // DirectX::XMMatrices are in row major layout
	this->constantBuffer.data.mat = DirectX::XMMatrixTranspose(this->constantBuffer.data.mat); // in shaders matrices by default are in column major layout
	if (!this->constantBuffer.ApplyChanges())
		return;

	this->context->VSSetConstantBuffers(0, 1, this->constantBuffer.GetAddressOf());

	UINT offset = 0;
	this->context->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), this->vertexBuffer.StridePtr(), &offset);
	this->context->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	this->context->DrawIndexed(this->indexBuffer.BufferSize(), 0, 0);

	spriteBatch->Begin();
	spriteFont->DrawString(this->spriteBatch.get(), L"HELLO WORLD DX11", DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();

	this->swapChain->Present(1, NULL);
}

bool Graphics::InitializeDirectX(HWND hwnd)
{
	std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

	if (adapters.size() < 1)
	{
		ErrorLogger::Log("No IDXG adapters found!\n");
		return false;
	}

	HRESULT hr = S_OK;
	D3D_DRIVER_TYPE driverTypes[] = { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE };
	UINT numDriverTypes = ARRAYSIZE(driverTypes);
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	D3D_FEATURE_LEVEL featureLevel;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferDesc.Width = this->windowWidth;
	swapChainDesc.BufferDesc.Height = this->windowHeight;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	UINT driver = 0;
	for (unsigned int i = 0; i < numDriverTypes; i++)
	{
		hr = D3D11CreateDeviceAndSwapChain(0/*adapters[0].pAdapter*/, driverTypes[driver],
			NULL, 0, &featureLevels[i], numFeatureLevels, D3D11_SDK_VERSION, &swapChainDesc,
			this->swapChain.GetAddressOf(), this->device.GetAddressOf(), &featureLevel, this->context.GetAddressOf());
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
	{
		ErrorLogger::Log("Failed to create swap chain and device!\n");
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(backBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		ErrorLogger::Log("Failed to get back buffer!\n");
		return false;
	}

	hr = device->CreateRenderTargetView(backBuffer.Get(), NULL, renderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log("Failed to create Render Target view!\n");
		return false;
	}

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));
	depthStencilDesc.Width = this->windowWidth;
	depthStencilDesc.Height = this->windowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log("Failed to create Depth Stencil Buffer!\n");
		return false;
	}

	hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log("Failed to create Depth Stencil View!\n");
		return false;
	}
	
	//Set the render target and depth/stencil views
	this->context->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());


	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilStateDesc.DepthEnable = true;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	hr = this->device->CreateDepthStencilState(&depthStencilStateDesc, this->depthStencilState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log("Failed to create Depth Stencil State!\n");
		return false;
	}
	

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(this->windowWidth);
	viewport.Height = static_cast<float>(this->windowHeight);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//Set the Viewport
	this->context->RSSetViewports(1, &viewport);

	D3D11_RASTERIZER_DESC rasterizerStateDesc;
	ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerStateDesc.CullMode = D3D11_CULL_BACK;
	rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;

	hr = this->device->CreateRasterizerState(&rasterizerStateDesc, rasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log("Failed to create Rasterizer State!\n");
		return false;
	}
	this->context->RSSetState(this->rasterizerState.Get());

	this->spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->context.Get());
	this->spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data/Fonts/comic_sans_ms_16.spritefont");


	//Create SamplerState
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	// Linear interpolation for Mipmapping (take 2 mipmaps - bigger and smaller,
	//independently interpolate them and take final linear interpolation between 2 interpolated mipmaps values),
	// linear interp for minification and magnification
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	hr = this->device->CreateSamplerState(&samplerDesc, this->samplerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log("Failed to create Sampler State!\n");
		return false;
	}


	return true;
}

bool Graphics::InitializeShaders()
{
	std::wstring shaderFolder;
#pragma region DetermineShaderPath
	//if (IsDebuggerPresent() == TRUE)
	//{
#ifdef _DEBUG
	#ifdef _WIN64
		shaderFolder = L"../x64/Debug/";
	#else
		shaderFolder = L"../Debug/";
	#endif
#else
	#ifdef _WIN64
		shaderFolder = L"../x64/Release/";
	#else
		shaderFolder = L"../Release/";
	#endif
#endif
	//}
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numLayoutElements = ARRAYSIZE(layout);

	if (!vertexShader.Initialize(this->device, shaderFolder + L"vertexShader.cso", layout, numLayoutElements))
	{
		return false;
	}

	if (!pixelShader.Initialize(this->device, shaderFolder + L"pixelShader.cso"))
	{
		return false;
	}

	return true;;
}

bool Graphics::InitializeScene()
{
	Vertex vertices[] =
	{
		Vertex(-0.75f, -0.75f, 0.0f, 0.0f, 1.0f),
		Vertex(-0.75f, 0.75f, 0.0f, 0.0f, 0.0f),
		Vertex(0.75f, 0.75f, 0.0f, 1.0f, 0.0f),
		Vertex(0.75f, -0.75f, 0.0f, 1.0f, 1.0f),
	};

	HRESULT hr = this->vertexBuffer.Initialize(this->device.Get(), vertices, ARRAYSIZE(vertices));
	if (FAILED(hr))
	{
		ErrorLogger::Log("Failed to create Vertex Buffer!\n");
		return false;
	}


	DWORD indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	hr = this->indexBuffer.Initialize(this->device.Get(), indices, ARRAYSIZE(indices));
	if (FAILED(hr))
	{
		ErrorLogger::Log("Failed to create Index Buffer!\n");
		return false;
	}

	hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/myTexture.JPG", nullptr, this->myTexture.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log("Failed to load texture from file and create Shader Resource View!\n");
		return false;
	}

	hr = this->constantBuffer.Initialize(this->device.Get(), this->context.Get());
	if (FAILED(hr))
	{
		ErrorLogger::Log("Failed to create Constant Buffer!\n");
		return false;
	}

	this->camera.SetPosition(0.0f, 0.0f, -2.0f); // set eye position
	this->camera.SetProjectionValues(90.0f, static_cast<float>(this->windowWidth) / static_cast<float>(this->windowHeight), 0.1f, 1000.0f);

	return true;
}
