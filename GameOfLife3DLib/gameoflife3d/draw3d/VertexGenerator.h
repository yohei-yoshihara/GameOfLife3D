#ifndef GAMEOFLIFE3D_DRAW3D_VERTEXGENERATOR_H_
#define GAMEOFLIFE3D_DRAW3D_VERTEXGENERATOR_H_

#include "gameOfLife3D/lifeSimulator/LifeData.h"

namespace graphics
{
class D3DInteropHelper;
}

namespace gameOfLife3D
{
namespace draw3d
{
#define NUMBER_OF_VERTICES_PER_LIFE 24
#define NUMBER_OF_INDEXES_PER_LIFE  36

struct VertexInfo {
    DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 color;
};
typedef UINT32 VERTEX_INDEX;

inline std::wostream& operator<< (std::wostream& os, gameOfLife3D::draw3d::VertexInfo const& v)
{
    os << L"VertexInfo[pos=(" << v.pos.x << L"," << v.pos.y << L"," << v.pos.z
       << L"),normal=(" << v.normal.x << L"," << v.normal.y << L"," << v.normal.z
       << L"),color=(" << v.color.x << L"," << v.color.y << L"," << v.color.z << L"," << v.color.w << L")]";
    return os;
}

class GenerationVertexData
{
public:
    GenerationVertexData() :
        vertexes(std::make_shared<std::vector<VertexInfo>>()),
        indexes(std::make_shared<std::vector<VERTEX_INDEX>>())
    {}
    std::shared_ptr<std::vector<VertexInfo>> vertexes;
    std::shared_ptr<std::vector<VERTEX_INDEX>> indexes;
};

class VertexGenerator
{
private:
    static const VertexInfo s_vertices[];
    static const VERTEX_INDEX s_indices[];

    bool m_attributeModified;
    UINT m_width;
    UINT m_height;
    UINT m_maxNumberOfGenerations;
    std::vector<GenerationVertexData> m_generationVertexData;
    float m_marginRate;

    // start point of 3D space
	DirectX::XMFLOAT3 m_p0;
    // end point of 3D space
	DirectX::XMFLOAT3 m_p1;

    ID3D11Buffer *m_pVertexBuffer;
    UINT m_numberOfVertexes;
    UINT m_maxNumberOfVertexes;
    ID3D11Buffer *m_pIndexBuffer;
    UINT m_numberOfIndexes;
    UINT m_maxNumberOfIndexes;

	DirectX::XMFLOAT3 m_cubeLength;
	DirectX::XMFLOAT3 m_cubeMargin;

protected:

public:
    VertexGenerator();
    virtual ~VertexGenerator();
    void SetWidth(UINT width) {
        m_width = width;
    }
    UINT GetWidth() const {
        return m_width;
    }
    void SetHeight(UINT height) {
        m_height = height;
    }
    UINT GetHeight() const {
        return m_height;
    }
    void SetMaxNumberOfGenerations(UINT maxNumberOfGenerations) {
        m_maxNumberOfGenerations = maxNumberOfGenerations;
    }
    UINT GetMaxNumberOfGenerations() const {
        return m_maxNumberOfGenerations;
    }
    UINT GetNumberOfGenerations() const {
        return static_cast<UINT>(m_generationVertexData.size());
    }
    void SetMarginRate(float marginRate) {
        m_marginRate = marginRate;
    }
    float GetMarginRate() const {
        return m_marginRate;
    }
	void SetP0(DirectX::XMFLOAT3 p0) {
        m_p0 = p0;
    }
	DirectX::XMFLOAT3 GetP0() const {
        return m_p0;
    }
	void SetP1(DirectX::XMFLOAT3 p1) {
        m_p1 = p1;
    }
	DirectX::XMFLOAT3 GetP1() const {
        return m_p1;
    }
    HRESULT Initialize(graphics::D3DInteropHelper *pD3DInteropHelper);
    HRESULT Add(const gameOfLife3D::lifeSimulator::LifeData *pLifeData);
    HRESULT Update(graphics::D3DInteropHelper *pD3DInteropHelper);
    void CopyVertexes(OUT VertexInfo *vertexes, OUT UINT *numberOfVertexes);
    void CopyVertexes(OUT std::vector<VertexInfo> &vertexes);
    void CopyIndexes(OUT VERTEX_INDEX *indexes, OUT UINT *numberOfIndexes);
    void CopyIndexes(OUT std::vector<VERTEX_INDEX> &indexes);

    ID3D11Buffer* GetVertexBuffer() const {
        return m_pVertexBuffer;
    }
    UINT GetNumberOfVertexes() const {
        return m_numberOfVertexes;
    }
    ID3D11Buffer* GetIndexBuffer() const {
        return m_pIndexBuffer;
    }
    UINT GetNumberOfIndexes() const {
        return m_numberOfIndexes;
    }

    void UpdateLengthAndMargin();
	DirectX::XMFLOAT3 GetCubeLength() {
        if (m_attributeModified) {
            UpdateLengthAndMargin();
        }
        return m_cubeLength;
    }
	DirectX::XMFLOAT3 GetCubeMargin() {
        if (m_attributeModified) {
            UpdateLengthAndMargin();
        }
        return m_cubeMargin;
    }
	DirectX::XMFLOAT3 CalculateCubeCenterPosition(
		const DirectX::XMFLOAT3 &cubeLength,
		const DirectX::XMFLOAT3 &cubeMargin,
        UINT x, UINT y, UINT generation);
    UINT GetMaxVertexes() const {
        return m_width * m_width * m_maxNumberOfGenerations * NUMBER_OF_VERTICES_PER_LIFE;
    }
    UINT GetMaxIndexes() const {
        return m_width * m_width * m_maxNumberOfGenerations * NUMBER_OF_INDEXES_PER_LIFE;
    }
    void Dump();
    void _MinMax(const float value, float &minValue, float &maxValue);
    void _AnalyzeVertices(
        const std::vector<gameOfLife3D::draw3d::VertexInfo> &vertices,
		OUT std::vector<DirectX::XMFLOAT3> &cubes);

    void CheckVertexAndIndex(
        IN const VertexInfo *vertexes,
        IN UINT numberOfVertexes,
        IN const VERTEX_INDEX *indexes,
        IN UINT numberOfIndexes);
};
}
}

#endif // GAMEOFLIFE3D_DRAW3D_VERTEXGENERATOR_H_
