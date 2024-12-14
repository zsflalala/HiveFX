#include "pch.h"
#include "ShaderProgram.h"
#include "Common.h"

using namespace hiveVG;

CShaderProgram* CShaderProgram::createProgram(AAssetManager *vAssetManager, const std::string& vVertFilePath, const std::string& vFragFilePath)
{
    std::string VertCode, FragCode;
    if (!__dumpShaderCodeFromFile(vAssetManager, vVertFilePath, VertCode)) return nullptr;
    if (!__dumpShaderCodeFromFile(vAssetManager, vFragFilePath, FragCode)) return nullptr;
    GLuint VertHandle, FragHandle;
    if (!__compileShader(GL_VERTEX_SHADER, vVertFilePath, VertCode, VertHandle)) return nullptr;
    if (!__compileShader(GL_FRAGMENT_SHADER, vFragFilePath, FragCode, FragHandle)) return nullptr;
    const std::vector ShaderHandles{ VertHandle, FragHandle };
    GLuint ProgramHandle;
    if (!__linkProgram(ShaderHandles, ProgramHandle))
    {
        for (const auto& ShaderHandle : ShaderHandles)
        {
            glDeleteShader(ShaderHandle);
        }
        return nullptr;
    }
    return new CShaderProgram(ShaderHandles, ProgramHandle);
}

CShaderProgram::~CShaderProgram()
{
    glDeleteProgram(m_ProgramHandle);
    m_ProgramHandle = 0;
    for (const auto& ShaderHandle : m_ShaderHandles)
    {
        glDeleteShader(ShaderHandle);
    }
    m_ShaderHandles.clear();
}

void CShaderProgram::useProgram() const
{
    glUseProgram(m_ProgramHandle);
}

void CShaderProgram::setUniform(const std::string& vName, int vValue)
{
    glUniform1iv(__getOrCreateUniformId(vName), 1, &vValue);
}

void CShaderProgram::setUniform(const std::string& vName, float vValue)
{
    glUniform1fv(__getOrCreateUniformId(vName), 1, &vValue);
}

void CShaderProgram::setUniform(const std::string& vName, const glm::vec2& vValue)
{
    glUniform2fv(__getOrCreateUniformId(vName), 1, &vValue[0]);
}

void CShaderProgram::setUniform(const std::string& vName, const glm::vec3& vValue)
{
    glUniform3fv(__getOrCreateUniformId(vName), 1, &vValue[0]);
}

void CShaderProgram::setUniform(const std::string& vName, const glm::vec4& vValue)
{
    glUniform4fv(__getOrCreateUniformId(vName), 1, &vValue[0]);
}

void CShaderProgram::setUniform(const std::string& vName, const glm::mat3& vMat)
{
    glUniformMatrix3fv(__getOrCreateUniformId(vName), 1, GL_FALSE, &vMat[0][0]);
}

void CShaderProgram::setUniform(const std::string& vName, const glm::mat4& vMat)
{
    glUniformMatrix4fv(__getOrCreateUniformId(vName), 1, GL_FALSE, &vMat[0][0]);
}

bool CShaderProgram::__dumpShaderCodeFromFile(AAssetManager *vAssetManager, const std::string& vShaderPath, std::string& voShaderCode)
{
    if (!vAssetManager)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SHADER_PROGRAM_TAG, "AssetManager is null.");
        return false;
    }

    AAsset* pAsset = AAssetManager_open(vAssetManager, vShaderPath.c_str(), AASSET_MODE_BUFFER);
    if (!pAsset)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SHADER_PROGRAM_TAG, "Failed to open asset: ");
        return false;
    }

    size_t AssetSize = AAsset_getLength(pAsset);
    std::unique_ptr<char[]> pBuffer(new char[AssetSize + 1]);
    AAsset_read(pAsset, pBuffer.get(), AssetSize);
    AAsset_close(pAsset);

    pBuffer[AssetSize] = '\0';
    voShaderCode = std::string(pBuffer.get());
    return true;

    // TODO : if above code runs succeed, delete it below.
//    std::ifstream ShaderFile;
//    ShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//    try
//    {
//        ShaderFile.open(vShaderPath);
//        std::stringstream ShaderStream;
//        ShaderStream << ShaderFile.rdbuf();
//        ShaderFile.close();
//        voShaderCode = ShaderStream.str();
//        return true;
//    }
//    catch (std::ifstream::failure& Error)
//    {
//        LOG_ERROR(hiveVG::TAG_KEYWORD::SHADER_PROGRAM_TAG, "Failed to read shader file: %s; Reason: %s", vShaderPath.c_str(), Error.what());
//        return false;
//    }
}

bool CShaderProgram::__compileShader(GLenum vType, const std::string& vShaderPath, const std::string& vShaderCode, GLuint& voShaderHandle)
{
    voShaderHandle = glCreateShader(vType);
    if (voShaderHandle == 0) return false;

    const char* ShaderCodeCStr = vShaderCode.c_str();
    glShaderSource(voShaderHandle, 1, &ShaderCodeCStr, nullptr);
    glCompileShader(voShaderHandle);
    GLint CompileStatus = GL_FALSE;
    glGetShaderiv(voShaderHandle, GL_COMPILE_STATUS, &CompileStatus);
    if (CompileStatus == GL_FALSE)
    {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(voShaderHandle, 1024, nullptr, InfoLog);
        LOG_ERROR(hiveVG::TAG_KEYWORD::SHADER_PROGRAM_TAG, "Failed to compile shader: %s; Reason: %s", vShaderPath.c_str(), InfoLog);
        glDeleteShader(voShaderHandle);
        voShaderHandle = 0;
        return false;
    }
    return true;
}

bool CShaderProgram::__linkProgram(const std::vector<GLuint>& vShaderHandles, GLuint& voProgramHandle)
{
    voProgramHandle = glCreateProgram();
    if (voProgramHandle == 0) return false;

    for (const auto& ShaderHandle : vShaderHandles)
    {
        glAttachShader(voProgramHandle, ShaderHandle);
    }
    glLinkProgram(voProgramHandle);
    GLint LinkStatus = GL_FALSE;
    glGetProgramiv(voProgramHandle, GL_LINK_STATUS, &LinkStatus);
    if (LinkStatus == GL_FALSE)
    {
        GLchar InfoLog[1024];
        glGetProgramInfoLog(voProgramHandle, 1024, nullptr, InfoLog);
        LOG_ERROR(hiveVG::TAG_KEYWORD::SHADER_PROGRAM_TAG, "Failed to link shader program: %s", InfoLog);
        glDeleteProgram(voProgramHandle);
        voProgramHandle = 0;
        return false;
    }
    return true;
}

CShaderProgram::CShaderProgram(const std::vector<GLuint>& vShaderHandles, GLuint vProgramHandle)
        : m_ShaderHandles(vShaderHandles), m_ProgramHandle(vProgramHandle) {}

GLint CShaderProgram::__getOrCreateUniformId(const std::string& vUniformName)
{
    if (m_UniformIdMap.count(vUniformName) > 0) return m_UniformIdMap[vUniformName];
    const auto UniformId = glGetUniformLocation(m_ProgramHandle, vUniformName.c_str());
    if (UniformId == -1)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SHADER_PROGRAM_TAG, "Uniform %s does not exist or is not set correctly", vUniformName.c_str());
    }
    m_UniformIdMap[vUniformName] = UniformId;
    return UniformId;
}
