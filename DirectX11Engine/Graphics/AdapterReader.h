#pragma once
#include "../ErrorLogger.h"
#include <d3d11.h>

#include <wrl/client.h>
#include <vector>

class AdapterData
{
public:
	AdapterData(IDXGIAdapter *adapter);
	IDXGIAdapter *pAdapter = nullptr;
	DXGI_ADAPTER_DESC adapterDesc;
};

class AdapterReader
{
public:
	static std::vector<AdapterData> GetAdapters();
	
private:
	static std::vector<AdapterData> adapters;
};