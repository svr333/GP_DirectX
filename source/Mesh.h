#pragma once
#include "Math.h"
#include <vector>

using namespace dae;

class Effect;

class Mesh
{
public:
	struct Vertex_Input
	{
		Vertex_Input(Vector3 position, ColorRGB color)
		{
			m_Position = position;
			m_Color = color;
		};

		Vector3 m_Position;
		ColorRGB m_Color;
	};

	struct Vertex_Output
	{
		Vector4 position;
		ColorRGB color;
	};

	Mesh(ID3D11Device* pDevice, const std::vector<Vertex_Input>& vertices, const std::vector<uint32_t>& indices);
	void Render(ID3D11DeviceContext* pDeviceContext);
	~Mesh();

private:
	Effect* m_pEffect;
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	int m_AmountIndices;
};
