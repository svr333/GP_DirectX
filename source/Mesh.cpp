#include "pch.h"
#include "Mesh.h"
#include "Effect.h"

Mesh::Mesh(ID3D11Device* pDevice, const std::vector<Vertex_In>& vertices, const std::vector<uint32_t>& indices)
{
	m_pEffect = new Effect( pDevice, L"Resources/PosCol3D.fx");

	// create vertex layout
	static const uint32_t numElements{ 5 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "TEXCOORD";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 24;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "NORMAL";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 32;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[4].SemanticName = "TANGENT";
	vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[4].AlignedByteOffset = 44;
	vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	// create input layout
	D3DX11_PASS_DESC passDesc;
	m_pEffect->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
	HRESULT result = pDevice->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &m_pVertexLayout);

	if (FAILED(result))
	{
		throw;
	}

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex_In) * (uint32_t)vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = vertices.data();
	
	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);

	if (FAILED(result))
	{
		throw;
	}

	// create index buffer
	bd = {};
	m_AmountIndices = static_cast<uint32_t>(indices.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_AmountIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	
	initData = { 0 };
	initData.pSysMem = indices.data();
	
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);

	if (FAILED(result))
	{
		throw;
	}
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext, const Matrix& worldViewProjMatrix)
{
	// Set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set input layout
	pDeviceContext->IASetInputLayout(m_pVertexLayout);

	// set vertex buffer
	UINT stride = sizeof(Vertex_In);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// set index buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// set input layout
	pDeviceContext->IASetInputLayout(m_pVertexLayout);

	// set matrix
	m_pEffect->GetMatrix()->SetMatrix((float*)(&worldViewProjMatrix));

	// render a triangle
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pEffect->GetTechnique()->GetDesc(&techDesc);

	for (UINT i = 0; i < techDesc.Passes; i++)
	{
		m_pEffect->GetTechnique()->GetPassByIndex(0)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_AmountIndices, 0, 0);
	}
}

void Mesh::SetTexture(Texture* texture)
{
	m_pEffect->SetDiffuseMap(texture);
}

void Mesh::UpdateSampleState(ID3D11SamplerState* pSampleState)
{
	HRESULT result = m_pEffect->GetEffect()->GetVariableByName("samPoint")->AsSampler()->SetSampler(0, pSampleState);

	if (FAILED(result))
	{
		std::wcout << L"Couldn't update sampler state\n";
		throw;
	}
}

Mesh::~Mesh()
{
	m_pVertexLayout->Release();
	m_pIndexBuffer->Release();
	m_pVertexBuffer->Release();
	delete m_pEffect;
}
