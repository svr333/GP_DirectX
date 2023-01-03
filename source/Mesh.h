#pragma once
#include "Math.h"
#include <vector>
#include "Texture.h"

using namespace dae;

class Effect;

class Mesh
{
public:
	struct Vertex_Input
	{
		Vector3 m_Position;
		ColorRGB m_Color;
		Vector2 uv;
	};

	struct Vertex_Output
	{
		Vector4 position;
		ColorRGB color;
		Vector2 uv;
	};

	Mesh(ID3D11Device* pDevice, const std::vector<Vertex_Input>& vertices, const std::vector<uint32_t>& indices);
	void Render(ID3D11DeviceContext* pDeviceContext, const Matrix& worldViewProjMatrix);

	void SetTexture(Texture* texture);
	~Mesh();

private:
	Effect* m_pEffect;
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	int m_AmountIndices;
};
