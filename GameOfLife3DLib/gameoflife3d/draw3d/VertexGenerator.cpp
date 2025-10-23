#include "stdafx.h"
#include "Common.h"
#include "graphics/D3DInteropHelper.h"
#include "gameOfLife3D/draw3d/Draw3DUtil.h"
#include "gameOfLife3D/draw3d/VertexGenerator.h"

using namespace DirectX;

#if defined(DEBUG) || defined(_DEBUG)
#define DEBUG_MODEL3D
#endif

const gameOfLife3D::draw3d::VertexInfo gameOfLife3D::draw3d::VertexGenerator::s_vertices[] = {
    {XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
    {XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
    {XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
    {XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},

    {XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)},
    {XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)},
    {XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
    {XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)},

    {XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
    {XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
    {XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
    {XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},

    {XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
    {XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
    {XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
    {XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},

    {XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
    {XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
    {XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
    {XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},

    {XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
    {XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
    {XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
    {XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
};

const gameOfLife3D::draw3d::VERTEX_INDEX gameOfLife3D::draw3d::VertexGenerator::s_indices[] = {3,  1,  0,  2,  1,  3,

                                                                                               6,  4,  5,  7,  4,  6,

                                                                                               11, 9,  8,  10, 9,  11,

                                                                                               14, 12, 13, 15, 12, 14,

                                                                                               19, 17, 16, 18, 17, 19,

                                                                                               22, 20, 21, 23, 20, 22};

gameOfLife3D::draw3d::VertexGenerator::VertexGenerator()
    : m_attributeModified(true)
    , m_width(32)
    , m_height(32)
    , m_maxNumberOfGenerations(32)
    , m_generationVertexData()
    , m_marginRate(0.2f)
    , m_p0(-1.0f, -1.0f, -1.0f)
    , m_p1(1.0f, 1.0f, 1.0f)
    , m_pVertexBuffer(nullptr)
    , m_numberOfVertexes(0)
    , m_pIndexBuffer(nullptr)
    , m_numberOfIndexes(0) {
}

gameOfLife3D::draw3d::VertexGenerator::~VertexGenerator() {
  SafeRelease(&m_pVertexBuffer);
  SafeRelease(&m_pIndexBuffer);
}

HRESULT gameOfLife3D::draw3d::VertexGenerator::Initialize(graphics::D3DInteropHelper *pD3DInteropHelper) {
  if (pD3DInteropHelper != nullptr) {
    ID3D11Device *pDevice = pD3DInteropHelper->GetD3D11Device();

    m_maxNumberOfVertexes = GetMaxVertexes();
    D3D11_BUFFER_DESC bufDesc;
    ZeroMemory(&bufDesc, sizeof(bufDesc));
    bufDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufDesc.ByteWidth = sizeof(VertexInfo) * m_maxNumberOfVertexes;
    bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    CHK_FATAL_HRESULT(pDevice->CreateBuffer(&bufDesc, nullptr, &m_pVertexBuffer));

    m_maxNumberOfIndexes = GetMaxIndexes();
    ZeroMemory(&bufDesc, sizeof(bufDesc));
    bufDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufDesc.ByteWidth = sizeof(VERTEX_INDEX) * m_maxNumberOfIndexes;
    bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    CHK_FATAL_HRESULT(pDevice->CreateBuffer(&bufDesc, nullptr, &m_pIndexBuffer));
  }

  UpdateLengthAndMargin();

  for (UINT i = 0; i < m_maxNumberOfGenerations; ++i) {
    gameOfLife3D::draw3d::GenerationVertexData gen;
    m_generationVertexData.push_back(gen);
  }

  return S_OK;
}

void gameOfLife3D::draw3d::VertexGenerator::_MinMax(const float value, float &minValue, float &maxValue) {
  if (value < minValue) {
    minValue = value;
  }
  if (value > maxValue) {
    maxValue = value;
  }
}

void gameOfLife3D::draw3d::VertexGenerator::_AnalyzeVertices(
    const std::vector<gameOfLife3D::draw3d::VertexInfo> &vertices, OUT std::vector<XMFLOAT3> &cubes) {
  for (size_t i = 0; i < vertices.size(); i += 24) {
    XMFLOAT3 minPos = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
    XMFLOAT3 maxPos = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    for (size_t j = 0; j < 24; ++j) {
      const gameOfLife3D::draw3d::VertexInfo &v = vertices[i + j];
      _MinMax(v.pos.x, minPos.x, maxPos.x);
      _MinMax(v.pos.y, minPos.y, maxPos.y);
      _MinMax(v.pos.z, minPos.z, maxPos.z);
    }
    XMFLOAT3 centerPos;
    centerPos.x = (maxPos.x - minPos.x) / 2.0f + minPos.x;
    centerPos.y = (maxPos.y - minPos.y) / 2.0f + minPos.y;
    centerPos.z = (maxPos.z - minPos.z) / 2.0f + minPos.z;
    cubes.push_back(centerPos);
  }
}

void gameOfLife3D::draw3d::VertexGenerator::Dump() {
  for (UINT genId = 0; genId < m_generationVertexData.size(); ++genId) {
    const GenerationVertexData &gen = m_generationVertexData[genId];
    if (gen.vertexes->size() % NUMBER_OF_VERTICES_PER_LIFE != 0) {
      throw std::runtime_error("corrupted vertexes");
    }
    std::vector<XMFLOAT3> cubes;
    _AnalyzeVertices(*gen.vertexes.get(), cubes);
    LOG(SEVERITY_LEVEL_DEBUG) << "Generation = " << genId << L", number of cubes = " << cubes.size();
    for (UINT i = 0; i < cubes.size(); ++i) {
      LOG(SEVERITY_LEVEL_DEBUG) << L"  cube(" << cubes[i].x << L"," << cubes[i].y << L"," << cubes[i].z << L")";
    }
  }
}

HRESULT gameOfLife3D::draw3d::VertexGenerator::Add(const gameOfLife3D::lifeSimulator::LifeData *pLifeData) {
  while (m_generationVertexData.size() >= m_maxNumberOfGenerations) {
    m_generationVertexData.erase(m_generationVertexData.begin(), m_generationVertexData.begin() + 1);
  }

  GenerationVertexData gen;
  UINT generation = static_cast<UINT>(m_generationVertexData.size());

  XMFLOAT3 cubeLength = GetCubeLength();
  XMFLOAT3 cubeMargin = GetCubeMargin();
  XMMATRIX scaleMat = XMMatrixScaling(cubeLength.x / 2.0f, cubeLength.y / 2.0f, cubeLength.z / 2.0f);

  for (UINT index = 0; index < m_width * m_height; ++index) {
    const gameOfLife3D::lifeSimulator::LifeData &lifeData = pLifeData[index];
    if (lifeData.alive != 0) {
      VERTEX_INDEX startVertexIndex = static_cast<VERTEX_INDEX>(gen.vertexes->size());
      UINT x = index % m_width;
      UINT y = index / m_width;

      XMFLOAT3 centerPos = CalculateCubeCenterPosition(cubeLength, cubeMargin, x, y, generation);
      XMMATRIX moveMat = XMMatrixTranslation(centerPos.x, centerPos.y, 0.0f /*centerPos.z */);

      for (UINT vidx = 0; vidx < NUMBER_OF_VERTICES_PER_LIFE; ++vidx) {
        const gameOfLife3D::draw3d::VertexInfo &src = s_vertices[vidx];
        VertexInfo destVertex;

        XMVECTOR pos = XMLoadFloat3(&src.pos);
        pos = XMVector3Transform(pos, scaleMat);
        pos = XMVector3Transform(pos, moveMat);
        XMStoreFloat3(&destVertex.pos, pos);
        destVertex.normal = src.normal;
        destVertex.color = XMFLOAT4(lifeData.color.x, lifeData.color.y, lifeData.color.z, 1.0f);
        //                 LOG(SEVERITY_LEVEL_DEBUG) << L"color: " << destVertex.color.x << L"," << destVertex.color.y
        //                     << L"," << destVertex.color.z << L"," << destVertex.color.w;

        gen.vertexes->push_back(destVertex);
      }

      for (UINT iidx = 0; iidx < NUMBER_OF_INDEXES_PER_LIFE; ++iidx) {
        gen.indexes->push_back(s_indices[iidx] + startVertexIndex);
      }
    }
  }
  m_generationVertexData.push_back(gen);
  return S_OK;
}

HRESULT gameOfLife3D::draw3d::VertexGenerator::Update(graphics::D3DInteropHelper *pD3DInteropHelper) {
  ID3D11DeviceContext *pImmediateContext = pD3DInteropHelper->GetD3D11ImmediateContext();

  // Vertexes
  D3D11_MAPPED_SUBRESOURCE mappedResource;
  pImmediateContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  VertexInfo *vertexes = (VertexInfo *)mappedResource.pData;
  CopyVertexes(vertexes, &m_numberOfVertexes);

  // Indexes
  pImmediateContext->Map(m_pIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  VERTEX_INDEX *indexes = (VERTEX_INDEX *)mappedResource.pData;
  CopyIndexes(indexes, &m_numberOfIndexes);

#ifdef DEBUG_MODEL3D
  CheckVertexAndIndex(vertexes, m_numberOfVertexes, indexes, m_numberOfIndexes);
#endif

  pImmediateContext->Unmap(m_pVertexBuffer, 0);
  pImmediateContext->Unmap(m_pIndexBuffer, 0);

  return S_OK;
}

void gameOfLife3D::draw3d::VertexGenerator::UpdateLengthAndMargin() {
  if (m_width < 1) {
    throw std::invalid_argument("width must be more than or equal to 1");
  }
  if (m_height < 1) {
    throw std::invalid_argument("height must be more than or equal to 1");
  }
  if (m_maxNumberOfGenerations < 1) {
    throw std::invalid_argument("max number of generations must be more than or equal to 1");
  }

  if (m_attributeModified) {
    Draw3DUtil::CalculateLengthAndMargin(m_width, m_height, m_maxNumberOfGenerations, m_p0, m_p1, m_marginRate,
                                         m_cubeLength, m_cubeMargin);
  }
  m_attributeModified = false;
}

XMFLOAT3 gameOfLife3D::draw3d::VertexGenerator::CalculateCubeCenterPosition(const XMFLOAT3 &cubeLength,
                                                                            const XMFLOAT3 &cubeMargin, UINT x, UINT y,
                                                                            UINT generation) {
  XMFLOAT3 pos;
  pos.x = m_p0.x + (cubeLength.x + cubeMargin.x) * x + cubeLength.x / 2.0f;
  pos.y = m_p0.y + (cubeLength.y + cubeMargin.y) * y + cubeLength.y / 2.0f;
  pos.z = m_p0.z + (cubeLength.z + cubeMargin.z) * generation + cubeLength.z / 2.0f;
  return pos;
}

void gameOfLife3D::draw3d::VertexGenerator::CopyVertexes(VertexInfo *vertexes, OUT UINT *numberOfVertexes) {
  UINT vIndex = 0;
  XMFLOAT3 cubeLength = GetCubeLength();
  XMFLOAT3 cubeMargin = GetCubeMargin();
  for (UINT generation = 0; generation < m_generationVertexData.size(); ++generation) {
    GenerationVertexData &gen = m_generationVertexData[generation];
    XMFLOAT3 pos = CalculateCubeCenterPosition(cubeLength, cubeMargin, 0, 0, generation);
    for (UINT i = 0; i < gen.vertexes->size(); ++i) {
      if (vIndex >= m_maxNumberOfVertexes) {
        LOG(SEVERITY_LEVEL_ERROR) << L"vertex index over max";
        throw std::runtime_error("vertex index over max");
      }
      vertexes[vIndex] = gen.vertexes->at(i);
      vertexes[vIndex].pos.z += pos.z;
      float rate = generation / static_cast<float>(m_maxNumberOfGenerations);
      vertexes[vIndex].color.x *= rate;
      vertexes[vIndex].color.y *= rate;
      vertexes[vIndex].color.z *= rate;
      vertexes[vIndex].color.w *= rate;
      ++vIndex;
    }
  }
  *numberOfVertexes = vIndex;
}

void gameOfLife3D::draw3d::VertexGenerator::CopyVertexes(OUT std::vector<VertexInfo> &vertexes) {
  vertexes.resize(GetMaxVertexes());
  UINT n;
  CopyVertexes(vertexes.data(), &n);
  vertexes.resize(n);
}

void gameOfLife3D::draw3d::VertexGenerator::CopyIndexes(VERTEX_INDEX *indexes, OUT UINT *numberOfIndexes) {
  VERTEX_INDEX indexSum = 0;
  UINT iIndex = 0;
  for (UINT generation = 0; generation < m_generationVertexData.size(); ++generation) {
    GenerationVertexData &gen = m_generationVertexData[generation];
    for (UINT i = 0; i < gen.indexes->size(); ++i) {
      if (iIndex >= m_maxNumberOfIndexes) {
        LOG(SEVERITY_LEVEL_ERROR) << L"index index over max";
        throw std::runtime_error("index index over max");
      }
      indexes[iIndex] = indexSum + gen.indexes->at(i);
      ++iIndex;
    }
    indexSum += static_cast<VERTEX_INDEX>(gen.vertexes->size());
  }
  *numberOfIndexes = iIndex;
}

void gameOfLife3D::draw3d::VertexGenerator::CopyIndexes(OUT std::vector<VERTEX_INDEX> &indexes) {
  indexes.resize(GetMaxIndexes());
  UINT n;
  CopyIndexes(indexes.data(), &n);
  indexes.resize(n);
}

void gameOfLife3D::draw3d::VertexGenerator::CheckVertexAndIndex(IN const VertexInfo *vertexes, IN UINT numberOfVertexes,
                                                                IN const VERTEX_INDEX *indexes,
                                                                IN UINT numberOfIndexes) {
  UNREFERENCED_PARAMETER(vertexes);
  if (numberOfVertexes % NUMBER_OF_VERTICES_PER_LIFE != 0) {
    LOG(SEVERITY_LEVEL_ERROR) << L"invalid number of vertexes" << numberOfVertexes;
  }
  if (numberOfIndexes % NUMBER_OF_INDEXES_PER_LIFE != 0) {
    LOG(SEVERITY_LEVEL_ERROR) << L"invalid number of indexes" << numberOfIndexes;
  }
  UINT nCubes_v = numberOfVertexes / NUMBER_OF_VERTICES_PER_LIFE;
  UINT nCubes_i = numberOfIndexes / NUMBER_OF_INDEXES_PER_LIFE;
  if (nCubes_v != nCubes_i) {
    LOG(SEVERITY_LEVEL_ERROR) << L"invalid number of cubes, nCubes_v = " << nCubes_v << L", nCubes_i = " << nCubes_i;
  }
  for (UINT i = 0; i < nCubes_v; ++i) {
    UINT startIndex = i * NUMBER_OF_VERTICES_PER_LIFE;
    UINT endIndex = (i + 1) * NUMBER_OF_VERTICES_PER_LIFE - 1;
    for (UINT j = 0; j < NUMBER_OF_INDEXES_PER_LIFE; ++j) {
      VERTEX_INDEX index = indexes[i * NUMBER_OF_INDEXES_PER_LIFE + j];
      if (index < startIndex || endIndex < index) {
        LOG(SEVERITY_LEVEL_ERROR) << L"index is out of range, index = " << index << L", startIndex = " << startIndex
                                  << L", endIndex = " << endIndex;
      }
    }
  }
}
