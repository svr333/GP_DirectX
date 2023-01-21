#pragma once
#include "Math.h"
#include <vector>
#include "Texture.h"
#include "BaseEffect.h"

using namespace dae;

class Effect;

class Mesh
{
public:
	struct Vertex_In
	{
		Vector3 position;
		ColorRGB color;
		Vector2 uv;
		Vector3 normal;
		Vector3 tangent;
	};

	struct Vertex_Out
	{
		Vector4 position;
		ColorRGB color;
		Vector2 uv;
		Vector3 normal;
		Vector3 tangent;
	};

	Mesh(ID3D11Device* pDevice, BaseEffect* pEffect, const std::vector<Vertex_In>& vertices, const std::vector<uint32_t>& indices);
	void Render(ID3D11DeviceContext* pDeviceContext, const Matrix& worldViewProjMatrix, const Matrix& worldMatrix, const Matrix& invViewMatrix);
	void Render(ID3D11DeviceContext* pDeviceContext, const Matrix& worldViewProjMatrix);

	void UpdateSampleState(ID3D11SamplerState* pSampleState);
	~Mesh();

private:
	BaseEffect* m_pEffect;
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	int m_AmountIndices;
};
