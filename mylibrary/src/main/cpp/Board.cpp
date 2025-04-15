#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <stddef.h>

#include "Board.h"

using namespace hiveVG;

CBoard* CBoard::m_pBoard = nullptr;
std::mutex   CBoard::m_Mutex;

CBoard* CBoard::getOrCreate()
{
    if (m_pBoard == nullptr)
    {
        std::lock_guard Lock(m_Mutex);
        if (m_pBoard == nullptr)
        {
            auto t = new CBoard;
            // C++11 内存屏障
            std::atomic_thread_fence(std::memory_order_acquire);
            m_pBoard = t;
        }
    }
    return m_pBoard;
}

void CBoard::destroy()
{
    if (m_pBoard != nullptr)
    {
        delete m_pBoard;
        m_pBoard = nullptr;
    }
}

CBoard::~CBoard()
{
    glDeleteBuffers(1, &m_VertexBufferHandle);
    m_VertexBufferHandle = 0;
    glDeleteBuffers(1, &m_IndexBufferHandle);
    m_IndexBufferHandle = 0;
    glDeleteVertexArrays(1, &m_VAOHandle);
    m_VAOHandle = 0;
}

void CBoard::bindAndDraw() const
{
    glBindVertexArray(m_VAOHandle);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}



#pragma pack(push,1)
struct SBoardAttributes{
    float m_Position[3];
    float m_TexCoord[2];
    float m_Normal[3];
    float m_Bitangent[3];
    float m_Tangent[3];
};
#pragma pack(pop)

static_assert(sizeof(SBoardAttributes) == sizeof(float)*14);



template<int IndexCount,int VertexCount>
void generateTangents(const int (&indices)[IndexCount],SBoardAttributes(&voAttributes)[VertexCount]){
    using namespace glm;

    auto ToVec3 = [](const float (&v)[3]) {
        return vec3(v[0], v[1], v[2]);
    };
    auto ToVec2 = [](const float (&v)[2]) {
        return vec2(v[0], v[1]);
    };

    auto ToFloat3 = [](const vec3& vVec,float (&vArr)[3]) {
        for (int i = 0; i < 3; i++) {
            vArr[i] = vVec[i];
        }
    };

    for(int i = 0; i < IndexCount; i += 3) {
        auto& P1 = voAttributes[indices[i]];
        auto& P2 = voAttributes[indices[i + 1]];
        auto& P3 = voAttributes[indices[i + 2]];
        vec3 Edge1 = ToVec3(P2.m_Position) - ToVec3(P1.m_Position);
        vec3 Edge2 = ToVec3(P3.m_Position) - ToVec3(P1.m_Position);
        vec2 DeltaUV1 = ToVec2(P2.m_TexCoord) - ToVec2(P1.m_TexCoord);
        vec2 DeltaUV2 = ToVec2(P3.m_TexCoord) - ToVec2(P1.m_TexCoord);
        auto F = 1.0f / (DeltaUV1.x * DeltaUV2.y - DeltaUV2.x * DeltaUV1.y);
        vec3 Tangent = (Edge1 * DeltaUV2.y - Edge2 * DeltaUV1.y) * F;
        vec3 Bitangent = (Edge2 * DeltaUV1.x - Edge1 * DeltaUV2.x) * F;

        ToFloat3(Tangent, P1.m_Tangent);
        ToFloat3(Bitangent, P1.m_Bitangent);
        ToFloat3(Tangent, P2.m_Tangent);
        ToFloat3(Bitangent, P2.m_Bitangent);
        ToFloat3(Tangent, P3.m_Tangent);
        ToFloat3(Bitangent, P3.m_Bitangent);

    }
}


CBoard::CBoard()
{
    SBoardAttributes VertexAttributes[4];

    VertexAttributes[0] = {
            .m_Position={-0.5f, -0.5f, 0.0f},
            .m_TexCoord={0.0f, 0.0f},
            .m_Normal={0.0f, 0.0f, 1.0f},
    };
    VertexAttributes[1] = {
            .m_Position={0.5f, -0.5f, 0},
            .m_TexCoord={1.0f, 0.0f},
            .m_Normal={0.0f, 0.0f, 1.0f},
    };
    VertexAttributes[2] = {
            .m_Position={0.5f, 0.5f, 0,},
            .m_TexCoord={1.0f, 1.0f},
            .m_Normal={0.0f, 0.0f, 1.0f},
    };
    VertexAttributes[3] = {
            .m_Position={-0.5f, 0.5f, 0},
            .m_TexCoord={0.0f, 1.0f},
            .m_Normal={0.0f, 0.0f, 1.0f},
    };

    auto CountOf = []<typename T,int ElemCount>(const T (&v)[ElemCount]) constexpr {
        return ElemCount;
    };

    bool invertV = false;
    if(invertV) {
        for (int i = 0; i < CountOf(VertexAttributes); i++) {
            auto& Attr = VertexAttributes[i];
           // Attr.m_TexCoord[0] = 1 - Attr.m_TexCoord[0];
            Attr.m_TexCoord[1] = 1 - Attr.m_TexCoord[1];
        }
    }

    int Indices[] = {
            0, 1, 2,
            2, 3, 0
    };

    generateTangents(Indices, VertexAttributes);

    glGenVertexArrays(1, &m_VAOHandle);
    glBindVertexArray(m_VAOHandle);

    glGenBuffers(1, &m_VertexBufferHandle);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttributes), VertexAttributes, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &m_IndexBufferHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);



    auto stride = sizeof(SBoardAttributes);

    glVertexAttribPointer(0, CountOf(VertexAttributes[0].m_Position),
                          GL_FLOAT,GL_FALSE, stride,
                          reinterpret_cast<void*>(offsetof(SBoardAttributes,m_Position)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, CountOf(VertexAttributes[0].m_TexCoord),
                          GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(offsetof(SBoardAttributes,m_TexCoord)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, CountOf(VertexAttributes[0].m_Normal),
                          GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(offsetof(SBoardAttributes,m_Normal)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, CountOf(VertexAttributes[0].m_Tangent),
                          GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(offsetof(SBoardAttributes,m_Tangent)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, CountOf(VertexAttributes[0].m_Bitangent),
                          GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<void*>(offsetof(SBoardAttributes,m_Bitangent)));
    glEnableVertexAttribArray(4);

}