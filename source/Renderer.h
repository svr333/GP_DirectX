#pragma once
#include "Camera.h"
#include "Texture.h"
#include "Mesh.h"

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Update(const Timer* pTimer);
		void Render();

		void CycleSampleState();

		enum class SampleState
		{
			Point,
			Linear,
			Antisotropic,
			End
		};

	private:
		SDL_Window* m_pWindow{};
		Camera m_Camera{};

		Mesh* m_Mesh;

		int m_Width{};
		int m_Height{};

		bool m_IsInitialized = false;
		SampleState m_SampleState = SampleState::Point;

		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;

		IDXGIFactory* m_pDXGIFactory;
		IDXGISwapChain* m_pSwapChain;

		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11Texture2D* m_pRenderTargetBuffer;
		ID3D11RenderTargetView* m_pRenderTargetView;

		ID3D11SamplerState* m_pSamplerState;

		HRESULT InitializeDirectX();
	};
}
