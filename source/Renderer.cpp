#include "pch.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Utils.h"

namespace dae {

	Renderer::Renderer(SDL_Window* pWindow) :
		m_pWindow(pWindow)
	{
		//Initialize
		SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

		//Initialize DirectX pipeline
		const HRESULT result = InitializeDirectX();
		if (result == S_OK)
		{
			m_IsInitialized = true;
			std::cout << "DirectX is initialized and ready!\n";
		}
		else
		{
			std::cout << "DirectX initialization failed!\n";
		}

		auto aspectRatio = static_cast<float>(m_Width) / m_Height;
		m_Camera.Initialize(aspectRatio, 45.0f, { 0.0f, 0.0f, -50.0f });

		std::vector<Mesh::Vertex_In> vertices;
		std::vector<uint32_t> indices;

		Utils::ParseOBJ("Resources/vehicle.obj", vertices, indices);
		m_Mesh = new Mesh{ m_pDevice, vertices, indices };
	}

	Renderer::~Renderer()
	{
		if (m_pSamplerState)
		{
			m_pSamplerState->Release();
		}

		m_pRenderTargetView->Release();
		m_pRenderTargetBuffer->Release();
		m_pDepthStencilView->Release();
		m_pDepthStencilBuffer->Release();
		m_pSwapChain->Release();

		if (m_pDeviceContext)
		{
			m_pDeviceContext->ClearState();
			m_pDeviceContext->Flush();
			m_pDeviceContext->Release();
		}

		m_pDevice->Release();
		m_pDXGIFactory->Release();

		delete m_Mesh;
	}

	void Renderer::Update(const Timer* pTimer)
	{
		m_Camera.Update(pTimer);
	}


	void Renderer::Render()
	{
		if (!m_IsInitialized)
		{
			return;
		}

		ColorRGB clearColor = ColorRGB(0.0f, 0.0f, 0.3f);
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		m_Mesh->Render(m_pDeviceContext, m_Camera.viewMatrix * m_Camera.projectionMatrix, Matrix{}, m_Camera.invViewMatrix);

		m_pSwapChain->Present(0, 0);
	}

	void Renderer::CycleSampleState()
	{
		m_SampleState = SampleState(((int)m_SampleState + 1) % (int)SampleState::End);

		// release previous one so we can create a new one
		if (m_pSamplerState)
		{
			m_pSamplerState->Release();
		}

		D3D11_SAMPLER_DESC sampleDesc{};
		sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		switch (m_SampleState)
		{
			case SampleState::Point:
				sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
				break;
			case SampleState::Linear:
				sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				break;
			case SampleState::Antisotropic:
				sampleDesc.Filter = D3D11_FILTER_ANISOTROPIC;
				break;
			default:
				break;
		}

		std::wcout << L"Sampler state: " << (int)m_SampleState << "\n";
		HRESULT result = m_pDevice->CreateSamplerState(&sampleDesc, &m_pSamplerState);

		if (FAILED(result))
		{
			std::wcout << L"Failed to update Sampler State!\n";
		}

		m_Mesh->UpdateSampleState(m_pSamplerState);
	}

	HRESULT Renderer::InitializeDirectX()
	{
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
		uint32_t deviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, deviceFlags, &featureLevel, 1,
			D3D11_SDK_VERSION, &m_pDevice, nullptr, &m_pDeviceContext);

		if (FAILED(result))
		{
			return result;
		}

		result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_pDXGIFactory));

		if (FAILED(result))
		{
			return result;
		}

		// get window info
		SDL_SysWMinfo sysWMInfo{};
		SDL_VERSION(&sysWMInfo.version);
		SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);

		// initialize swap chain settings
		DXGI_SWAP_CHAIN_DESC swapChainDesc{};
		swapChainDesc.BufferDesc.Width = m_Width;
		swapChainDesc.BufferDesc.Height = m_Height;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;
		swapChainDesc.OutputWindow = sysWMInfo.info.win.window;

		result = m_pDXGIFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);

		if (FAILED(result))
		{
			return result;
		}

		// depth buffer init
		D3D11_TEXTURE2D_DESC depthStencilDesc{};
		depthStencilDesc.Width = m_Width;
		depthStencilDesc.Height = m_Height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = depthStencilDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);

		if (FAILED(result))
		{
			return result;
		}

		result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);

		if (FAILED(result))
		{
			return result;
		}

		result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));

		if (FAILED(result))
		{
			return result;
		}

		result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, 0, &m_pRenderTargetView);

		if (FAILED(result))
		{
			return result;
		}

		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		D3D11_VIEWPORT viewPort{};
		viewPort.Width = static_cast<float>(m_Width);
		viewPort.Height = static_cast<float>(m_Height);
		viewPort.TopLeftX = 0.0f;
		viewPort.TopLeftY = 0.0f;
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;

		m_pDeviceContext->RSSetViewports(1, &viewPort);

		return result;
	}
}
