#pragma once

struct ID3DX11Effect;

class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	~Effect();
	ID3DX11Effect* GetEffect();
	ID3DX11EffectTechnique* GetTechnique();
	ID3DX11EffectMatrixVariable* GetMatrix();

private:
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProj;
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMap;
};
