#include "Graphics.h"

bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	this->windowWidth = width;
	this->windowHeight = height;

	this->fpsTimer.Start();

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

	//Init ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(this->device.Get(), this->context.Get());
	ImGui::StyleColorsDark();

	return true;
}

void Graphics::DrawLightControl(Light &light)
{
	switch (light.lightType)
	{
	case DIRECTIONAL_LIGHT:
	{
		ImGui::Begin("Directional light");
		ImGui::DragFloat("Direct light strength ", &light.lightStrength, 0.01f, 0.0f, 10.0f);
		ImGui::ColorEdit3("Direct light color", &light.lightColor.x);
		ImGui::DragFloat("Specular alpha", &light.specularAlpha, 1.0f, 0.0f, 128.0f);
		ImGui::End();
		break;
	}
	case POINT_LIGHT:
	{
		ImGui::Begin("Point light");
		ImGui::DragFloat("Point light strength ", &light.lightStrength, 0.01f, 0.0f, 10.0f);
		ImGui::ColorEdit3("Point light color", &light.lightColor.x);
		ImGui::DragFloat("Specular alpha", &light.specularAlpha, 1.0f, 0.0f, 128.0f);
		ImGui::DragFloat3("Attenuation const/linear/quad", &light.constantAttenuation, 0.1, 0.0f, 10.0f);
		ImGui::End();
		break;
	}
	case SPOT_LIGHT:
	{
		ImGui::Begin("Spot light");
		ImGui::DragFloat("Spot light cone cos(angle)", &light.spotAngle, 0.01f, -1.0f, 1.0f);
		ImGui::DragFloat("Spot light strength ", &light.lightStrength, 0.01f, 0.0f, 10.0f);
		ImGui::ColorEdit3("Spot light color", &light.lightColor.x);
		ImGui::DragFloat("Specular alpha", &light.specularAlpha, 1.0f, 0.0f, 128.0f);
		ImGui::DragFloat3("Attenuation const/linear/quad", &light.constantAttenuation, 0.1, 0.0f, 10.0f);
		ImGui::End();
		break;
	}
	}
}

void Graphics::RenderFrame()
{
	int nLight = 0;
	for (auto &light : this->lights)
	{
		this->cb_ps_light.data.lights[nLight].specularAlpha = light.specularAlpha;
		this->cb_ps_light.data.lights[nLight].lightColor = light.lightColor;
		this->cb_ps_light.data.lights[nLight].lightStrength = light.lightStrength;
		this->cb_ps_light.data.lights[nLight].lightType = light.lightType;
		this->cb_ps_light.data.lights[nLight].isEnabled = light.isEnabled ? 1 : 0;
		XMVECTOR LightDirection = XMVector3Normalize(light.GetForwardVector()); //used for directional/spot lights
		XMStoreFloat4(&cb_ps_light.data.lights[nLight].lightDirection, LightDirection);

		this->cb_ps_light.data.lights[nLight].constantAttenuation = light.constantAttenuation;
		this->cb_ps_light.data.lights[nLight].linearAttenuation = light.linearAttenuation;
		this->cb_ps_light.data.lights[nLight].quadraticAttenuation = light.quadraticAttenuation;
		XMStoreFloat4(&this->cb_ps_light.data.lights[nLight].lightPosition, light.GetPositionVector());

		this->cb_ps_light.data.lights[nLight].spotAngle = light.spotAngle;

		nLight++;
	}
	XMStoreFloat4(&this->cb_ps_light.data.worldEyePos, this->camera.GetPositionVector());
	

	this->cb_ps_light.ApplyChanges();
	this->context->PSSetConstantBuffers(0, 1, this->cb_ps_light.GetAddressOf());

	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	this->context->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);
	this->context->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->context->IASetInputLayout(this->vertexShader.GetInputLayout());
	this->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	this->context->VSSetShader(vertexShader.GetShader(), NULL, 0);

	this->context->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	//this->context->OMSetBlendState(this->blendState.Get(), NULL, 0xFFFFFFFF);
	this->context->OMSetBlendState(NULL, NULL, 0xFFFFFFFF);
	this->context->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	this->context->PSSetShader(pixelShader.GetShader(), NULL, 0);

	XMMATRIX viewProjMatrix = this->camera.GetViewMatrix() * this->camera.GetProjectionMatrix();
	{
		this->gameObject[0].Draw(viewProjMatrix);
		this->gameObject[1].Draw(viewProjMatrix);
	}
	{
		this->context->PSSetShader(pixelShader_no_light.GetShader(), NULL, 0);
		this->lights[0].Draw(viewProjMatrix);
		this->context->PSSetShader(pixelShader_no_light.GetShader(), NULL, 0);
		this->lights[1].Draw(viewProjMatrix);
		this->context->PSSetShader(pixelShader_no_light.GetShader(), NULL, 0);
		this->lights[2].Draw(viewProjMatrix);
	}

	static int fpsCounter = 0;
	static std::wstring fpsString = L"\nFPS: 0";
	fpsCounter += 1;
	if (this->fpsTimer.GetMillisecondsElapsed() > 1000.0)
	{
		fpsString = L"\nFPS: " + std::to_wstring(fpsCounter);
		fpsCounter = 0;
		this->fpsTimer.Restart();
	}

	std::wstring text = L"Camera position:\nX = ";
	text += std::to_wstring(this->camera.GetPositionFloat3().x);
	text += L" Y = ";
	text += std::to_wstring(this->camera.GetPositionFloat3().y);
	text += L" Z = ";
	text += std::to_wstring(this->camera.GetPositionFloat3().z);
	text += L"\nCamera rotation:\nX = ";
	text += std::to_wstring(this->camera.GetRotationFloat3().x);
	text += L" Y = ";
	text += std::to_wstring(this->camera.GetRotationFloat3().y);
	text += L" Z = ";
	text += std::to_wstring(this->camera.GetRotationFloat3().z);

	text += fpsString;

	spriteBatch->Begin();
	spriteFont->DrawString(this->spriteBatch.get(), text.c_str(), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();

	//start the ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//Create ImGui window
	ImGui::Begin("Ambient light");
	ImGui::ColorEdit3("Ambient color ", &this->cb_ps_light.data.globalAmbientColor.x);
	ImGui::DragFloat("Ambient strength ", &this->cb_ps_light.data.globalAmbientStrength, 0.01f, 0.0f, 10.0f);
	ImGui::End();

	ImGui::Begin("Enabled lights");
	for (auto & light : this->lights)
	{
		if (light.lightType == DIRECTIONAL_LIGHT)
			ImGui::Checkbox("Direct", &light.isEnabled);
		if (light.lightType == POINT_LIGHT)
			ImGui::Checkbox("Point", &light.isEnabled);
		if (light.lightType == SPOT_LIGHT)
			ImGui::Checkbox("SPOT", &light.isEnabled);
	}
	ImGui::End();

	for (auto & light : this->lights)
	{
		if (light.isEnabled)
			this->DrawLightControl(light);
	}
	
	//assemble draw data
	ImGui::Render();
	//render draw data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	this->swapChain->Present(0, NULL);
}

Graphics::~Graphics()
{
	//Releases COM references that ImGui was given on setup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool Graphics::InitializeDirectX(HWND hwnd)
{
	try
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

		DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
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
			hr = D3D11CreateDeviceAndSwapChain(0/*adapters[1].pAdapter*/, driverTypes[driver],
				NULL, 0, &featureLevels[i], numFeatureLevels, D3D11_SDK_VERSION, &swapChainDesc,
				this->swapChain.GetAddressOf(), this->device.GetAddressOf(), &featureLevel, this->context.GetAddressOf());
			if (SUCCEEDED(hr))
				break;
		}
		COM_ERROR_IF_FAILED(hr, "Failed to create swap chain and device!");

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(backBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "Failed to get back buffer!");

		hr = device->CreateRenderTargetView(backBuffer.Get(), NULL, renderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create Render Target view!");

		CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowWidth, this->windowHeight);
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create Depth Stencil Buffer!");

		hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create Depth Stencil View!");
		//Set the render target and depth/stencil views
		this->context->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

		CD3D11_DEPTH_STENCIL_DESC depthStencilStateDesc(D3D11_DEFAULT);
		depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = this->device->CreateDepthStencilState(&depthStencilStateDesc, this->depthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create Depth Stencil State!");

		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));
		//Set the Viewport
		this->context->RSSetViewports(1, &viewport);

		D3D11_RASTERIZER_DESC rasterizerStateDesc = { 0 };
		//ZeroMemory(&rasterizerStateDesc, sizeof(D3D11_RASTERIZER_DESC));
		rasterizerStateDesc.CullMode = D3D11_CULL_BACK;
		rasterizerStateDesc.FillMode = D3D11_FILL_SOLID;

		hr = this->device->CreateRasterizerState(&rasterizerStateDesc, rasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create Rasterizer State!");
		this->context->RSSetState(this->rasterizerState.Get());


		D3D11_BLEND_DESC blendStateDesc = { 0 };
		//ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
		D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 };
		//ZeroMemory(&rtbd, sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rtbd.BlendOp = D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		blendStateDesc.RenderTarget[0] = rtbd;
		hr = this->device->CreateBlendState(&blendStateDesc, this->blendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create Blend State!");
		this->context->RSSetState(this->rasterizerState.Get());

		this->spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->context.Get());
		this->spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data/Fonts/comic_sans_ms_16.spritefont");

		//Create SamplerState
		D3D11_SAMPLER_DESC samplerDesc = { 0 };
		//ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
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
		COM_ERROR_IF_FAILED(hr, "Failed to create Sampler State!");
	}
	catch (COMException &exception)
	{
		ErrorLogger::Log(exception);
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
	    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

	if (!pixelShader_no_light.Initialize(this->device, shaderFolder + L"pixelShader_no_light.cso"))
	{
		return false;
	}

	return true;;
}

bool Graphics::InitializeScene()
{
	try
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/seamless_grass.JPG", nullptr, this->grassTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to load texture from file and create Shader Resource View!");

		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/pinksquare.JPG", nullptr, this->pinkTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to load texture from file and create Shader Resource View!");

		hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/seamless_pavement.JPG", nullptr, this->pavementTexture.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to load texture from file and create Shader Resource View!");

		hr = this->cb_vs_vertexShader.Initialize(this->device.Get(), this->context.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to create Constant Buffer for VS!");

		hr = this->cb_ps_light.Initialize(this->device.Get(), this->context.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to create Constant Buffer for PS!");

		gameObject.push_back(RenderableGameObject());
		if (this->gameObject.back().Initialize("data\\Objects\\Nanosuit\\Nanosuit.obj", this->device.Get(), this->context.Get(), this->cb_vs_vertexShader) == false)
			return false;

		gameObject.push_back(RenderableGameObject());
		if (this->gameObject.back().Initialize("", this->device.Get(), this->context.Get(), this->cb_vs_vertexShader) == false)
			return false;

		this->lights.push_back(Light(DIRECTIONAL_LIGHT));
		this->lights.push_back(Light(POINT_LIGHT));
		this->lights.push_back(Light(SPOT_LIGHT));
		for (auto &light : this->lights)
		{
			if (light.Initialize(this->device.Get(), this->context.Get(), this->cb_vs_vertexShader) == false)
				return false;
		}
		
		this->camera.SetPosition(0.0f, 0.0f, -2.0f); // set eye position
		this->camera.SetProjectionValues(90.0f, static_cast<float>(this->windowWidth) / static_cast<float>(this->windowHeight), 0.1f, 3000.0f);
	}
	catch (COMException &exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}

	return true;
}
