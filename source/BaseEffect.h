#pragma once
#include "Texture.h"

using namespace dae;

class BaseEffect
{
public:
	BaseEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
	virtual ~BaseEffect();
	ID3DX11Effect* GetEffect();
	ID3DX11EffectTechnique* GetTechnique();

	ID3DX11EffectMatrixVariable* GetWorldViewMatrix();
	virtual ID3DX11EffectMatrixVariable* GetWorldMatrix() { return nullptr; };
	virtual ID3DX11EffectMatrixVariable* GetViewInverseMatrix() { return nullptr; };

	virtual ID3D11InputLayout* CreateInputLayout(ID3D11Device* pDevice) = 0;
	void SetDiffuseMap(Texture* pDiffuseMap);

protected:
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMap;

	Texture* m_pTexture;

	ID3DX11EffectMatrixVariable* m_pMatWorldViewProj;
};
