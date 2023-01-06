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

		//--------------------------------
		//Matrices
		//--------------------------------

		m_pMatWorldViewProj = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();

		if (!m_pMatWorldViewProj->IsValid())
		{
			std::wcout << L"WorldViewProjMatrix is invalid.\n";
		}

		m_pMatWorld = m_pEffect->GetVariableByName("gWorld")->AsMatrix();

		if (!m_pMatWorld->IsValid())
		{
			std::wcout << L"m_pMatWorldVar is not valid!\n";
		}
		m_pMatViewInverse = m_pEffect->GetVariableByName("gViewInverse")->AsMatrix();

		if (!m_pMatViewInverse->IsValid())
		{
			std::wcout << L"m_pMatViewInverseVar is not valid!\n";
		}

		//--------------------------------
		// Texture2D
		//--------------------------------

		m_pDiffuseMap = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();

		if (!m_pDiffuseMap->IsValid())
		{
			std::wcout << L"DiffuseMap is invalid.\n";
		}

		m_pNormalMap = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();

		if (!m_pNormalMap->IsValid())
		{
			std::wcout << L"NormalMap is invalid.\n";
		}

		m_pSpecularMap = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();

		if (!m_pSpecularMap->IsValid())
		{
			std::wcout << L"SpecularMap is invalid.\n";
		}

		m_pGlossMap = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();

		if (!m_pGlossMap->IsValid())
		{
			std::wcout << L"GlossinessMap is invalid.\n";
		}
	}

	if (pErrorBlob)
	{
		std::cout << "Error occured in Effect.cpp.\n";

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
	m_pMatViewInverse->Release();
	m_pMatWorldViewProj->Release();
	m_pMatWorld->Release();
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

ID3DX11EffectMatrixVariable* Effect::GetWorldViewMatrix()
{
	return m_pMatWorldViewProj;
}

ID3DX11EffectMatrixVariable* Effect::GetWorldMatrix()
{
	return m_pMatWorld;
}

ID3DX11EffectMatrixVariable* Effect::GetViewInverseMatrix()
{
	return m_pMatViewInverse;
}

void Effect::SetDiffuseMap(dae::Texture* pDiffuseMap)
{
	if (m_pDiffuseMap)
	{
		m_pDiffuseMap->SetResource(pDiffuseMap->GetSRV());
	}
}

void Effect::SetNormalMap(dae::Texture* pNormalMap)
{

}

void Effect::SetSpecularMap(dae::Texture* pSpecularMap)
{
}

void Effect::SetGlossMap(dae::Texture* pGlossMap)
{
}
