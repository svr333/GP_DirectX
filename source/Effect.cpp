#include "pch.h"
#include "Effect.h"

#include <sstream>

Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
	: m_pEffect(), m_pTechnique()
{
	HRESULT result = S_OK;
	ID3D10Blob* pErrorBlob = nullptr;
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(assetFile.c_str(), nullptr, nullptr, shaderFlags, 0, pDevice, &pEffect, &pErrorBlob);

	if (!FAILED(result))
	{
		m_pEffect = pEffect;

		m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");

		if (!m_pTechnique->IsValid())
		{
			std::wcout << "Technique is invalid\n";
		}

		m_pMatWorldViewProj = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();

		if (!m_pMatWorldViewProj->IsValid())
		{
			std::wcout << L"WorldViewProjMatrix is invalid.\n";
		}

		m_pDiffuseMap = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();

		if (!m_pDiffuseMap->IsValid())
		{
			std::wcout << L"DiffuseMap is invalid.\n";
		}

		return;
	}

	std::cout << "Error occured in Effect.cpp.\n";

	if (pErrorBlob)
	{
		char* pErrors = (char*)pErrorBlob->GetBufferPointer();

		std::wstringstream ss;
		for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); ++i)
		{
			ss << pErrors[i];
		}

		OutputDebugStringW(ss.str().c_str());
		std::wcout << ss.str() << "\n";

		pErrorBlob->Release();
	}
}

Effect::~Effect()
{
	m_pDiffuseMap->Release();
	m_pMatWorldViewProj->Release();
	m_pTechnique->Release();
	m_pEffect->Release();
}

ID3DX11Effect* Effect::GetEffect()
{
	return m_pEffect;
}

ID3DX11EffectTechnique* Effect::GetTechnique()
{
	return m_pTechnique;
}
