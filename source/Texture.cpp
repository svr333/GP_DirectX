#include "pch.h"
#include "Texture.h"
#include "Vector2.h"
#include <SDL_image.h>
#include <d3d11.h>

namespace dae
{
	Texture::Texture()
	{
	}

	Texture::~Texture()
	{
		m_pResource->Release();
		m_pSRV->Release();
	}

	Texture* Texture::LoadFromFile(ID3D11Device* device, const std::string& path)
	{
		auto pSurface = IMG_Load(path.c_str());
		Texture* texture = new Texture();

		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		D3D11_TEXTURE2D_DESC desc{};
		desc.Width = pSurface->w;
		desc.Height = pSurface->h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData{};
		initData.pSysMem = pSurface->pixels;
		initData.SysMemPitch = static_cast<UINT>(pSurface->pitch);
		initData.SysMemSlicePitch = static_cast<UINT>(pSurface->h * pSurface->pitch);

		HRESULT result = device->CreateTexture2D(&desc, &initData, &texture->m_pResource);

		if (FAILED(result))
		{
			delete texture;
			throw;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
		SRVDesc.Format = format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		result = device->CreateShaderResourceView(texture->m_pResource, &SRVDesc, &texture->m_pSRV);

		if (FAILED(result))
		{
			delete texture;
			throw;
		}

		SDL_FreeSurface(pSurface);
		return texture;
	}

	ID3D11ShaderResourceView* Texture::GetSRV()
	{
		return m_pSRV;
	}
}