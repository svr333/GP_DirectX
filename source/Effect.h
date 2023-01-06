#pragma once
#include "Texture.h"

class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	~Effect();
	ID3DX11Effect* GetEffect();
	ID3DX11EffectTechnique* GetTechnique();

	ID3DX11EffectMatrixVariable* GetWorldViewMatrix();
	ID3DX11EffectMatrixVariable* GetWorldMatrix();
	ID3DX11EffectMatrixVariable* GetViewInverseMatrix();

	void SetDiffuseMap(dae::Texture* pDiffuseMap);
	void SetNormalMap(dae::Texture* pNormalMap);
	void SetSpecularMap(dae::Texture* pSpecularMap);
	void SetGlossMap(dae::Texture* pGlossMap);

private:
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;

	ID3DX11EffectMatrixVariable* m_pMatWorldViewProj;
	ID3DX11EffectMatrixVariable* m_pMatWorld;
	ID3DX11EffectMatrixVariable* m_pMatViewInverse;

	ID3DX11EffectShaderResourceVariable* m_pDiffuseMap;
	ID3DX11EffectShaderResourceVariable* m_pNormalMap;
	ID3DX11EffectShaderResourceVariable* m_pSpecularMap;
	ID3DX11EffectShaderResourceVariable* m_pGlossMap;
};
