#include "Shader.h"

Shader::Shader(const std::string& filepath)
    :m_FilePath(filepath), m_RendererId(0)
{
    ShaderProgramSource source = Parseshader();
    m_RendererId = CreateShader(source);
}

Shader::~Shader()
{
    glDeleteShader(m_RendererId);
}

void Shader::Bind() const
{
    glUseProgram(m_RendererId);
}

void Shader::UnBind() const
{
    glUseProgram(0);
}


ShaderProgramSource Shader::Parseshader()
{
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1, GEOMETRY = 2, TessCont = 3, TessEval = 4
    };
    std::ifstream stream(m_FilePath);
    std::string line;
    std::stringstream ss[5];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
            else if (line.find("geometry") != std::string::npos)
                type = ShaderType::GEOMETRY;
            else if (line.find("tess_cont") != std::string::npos)
                type = ShaderType::TessCont;
            else if (line.find("tess_eval") != std::string::npos)
                type = ShaderType::TessEval;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str(), ss[2].str(), ss[3].str(), ss[4].str() };
}
unsigned int Shader::CompileShader(unsigned int type,
    const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();//&source[0]
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        std::cout << "failed." << std::endl;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)_malloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "failed to compile shader!" << std::endl;
        std::string erro_message;
        switch (type)
        {
        case GL_VERTEX_SHADER: erro_message = "vertex ";
            break;
        case GL_FRAGMENT_SHADER:erro_message = "fragment ";
            break;
        case GL_GEOMETRY_SHADER:erro_message = "geometry ";
            break;
        case GL_TESS_CONTROL_SHADER:erro_message = "tess_cont ";
            break;
        case GL_TESS_EVALUATION_SHADER:erro_message = "tess_eval ";
            break;
        }
        std::cout << erro_message << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}
unsigned int Shader::CreateShader(const ShaderProgramSource& source)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = 0;
    if (!source.VertexSource.empty())
    {
        vs = CompileShader(GL_VERTEX_SHADER, source.VertexSource);
        glAttachShader(program, vs);
    }
    unsigned int fs = 0;
    if (!source.FragmentSource.empty())
    {
        fs = CompileShader(GL_FRAGMENT_SHADER, source.FragmentSource);
        glAttachShader(program, fs);
    }
    unsigned int gs = 0;
    if (!source.GeometrySource.empty())
    {
        gs = CompileShader(GL_GEOMETRY_SHADER, source.GeometrySource);
        glAttachShader(program, gs);
    }
    unsigned int tcs = 0;
    if (!source.TessContSource.empty())
    {
        tcs = CompileShader(GL_TESS_CONTROL_SHADER, source.TessContSource);
        glAttachShader(program, tcs);
    }
    unsigned int tes = 0;
    if (!source.TessEvalSource.empty())
    {
        tes = CompileShader(GL_TESS_EVALUATION_SHADER, source.TessEvalSource);
        glAttachShader(program, tes);
    }
    glLinkProgram(program);
    glValidateProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
    glDeleteShader(gs);
    glDeleteShader(tcs);
    glDeleteShader(tes);
    return program;
}
int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        return m_UniformLocationCache[name];
    }
    int location = glGetUniformLocation(m_RendererId, name.c_str());
    if (location == -1)
        std::cout << "warning: uniform '" << name << "' doesn't exist!" << std::endl;
    m_UniformLocationCache[name] = location;
    return location;

}
void Shader::SetUniform1i(const std::string& name, int value)
{
    glUniform1i(GetUniformLocation(name), value);
}
void Shader::SetUniform1b(const std::string& name, bool value)
{
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string& name, float value)
{
    glUniform1f(GetUniformLocation(name), value);
}
void Shader::SetUniform2f(const std::string& name, float value1, float value2)
{
    glUniform2f(GetUniformLocation(name), value1, value2);
}
void Shader::SetUniform2f(const std::string& name, glm::vec2 value)
{
    glUniform2fv(GetUniformLocation(name), 1, &value[0]);
}
void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}
void Shader::SetUniform4f(const std::string& name, glm::vec4 value)
{
    glUniform4fv(GetUniformLocation(name), 1, &value[0]);
}

void Shader::SetUniformmatri4fv(const std::string& name, const glm::mat4& ptr)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &ptr[0][0]);
}

void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
    glUniform3f(GetUniformLocation(name), v0, v1, v2);
}
void Shader::SetUniform3f(const std::string& name, glm::vec3 vc)
{
    glUniform3fv(GetUniformLocation(name), 1, &vc[0]);
}