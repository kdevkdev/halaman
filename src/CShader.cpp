#include "CShader.h"

CShader::CShader(const char * vertex_shader_path, const char * fragment_shader_path)
{
    GLuint vertex_shader = load_shader(vertex_shader_path,GL_VERTEX_SHADER);
    GLuint fragment_shader = load_shader(fragment_shader_path,GL_FRAGMENT_SHADER);

    m_shader_programm = glCreateProgram();
    glAttachShader(m_shader_programm, vertex_shader);
    glAttachShader(m_shader_programm, fragment_shader);

    // linking shaders
    glLinkProgram(m_shader_programm);
    // print linking errors if any
    int success;
    char infoLog[512];
    glGetProgramiv(m_shader_programm, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(m_shader_programm, 512, NULL, infoLog);
        printf("linknig shaders failed: %s", infoLog);
    }

    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
}
CShader::~CShader()
{
    //dtor
}
void CShader::use()
{
    glUseProgram(m_shader_programm);
}
GLuint CShader::load_shader(const char * path, GLenum type)
{
    // code from https://learnopengl.com/#!Getting-started/Shaders
    std::string shader_code;
    std::ifstream shader_file;
    unsigned int shader;
    int success;
    char infoLog[512];

    shader_file.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // open files
        shader_file.open(path);
        std::stringstream shader_stream;

        // read file's buffer contents into streams
        shader_stream<< shader_file.rdbuf();

        // close file handlers
        shader_file.close();

        // convert stream into string
        shader_code = shader_stream.str();

    }
    catch(std::ifstream::failure e)
    {
        printf("failure reading shader: %s\n", path);

        return false;
    }
    const  char * cshader_code = shader_code.c_str();

    // create shader and compile
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &cshader_code, NULL);
    glCompileShader(shader);

    // print compile errors if any
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("%s: compilation failed: %s \n", path, infoLog);
    };

    return shader;
}
void CShader::set_bool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_shader_programm, name.c_str()), (int)value);
}
void CShader::set_int(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(m_shader_programm, name.c_str()), value);
}
void CShader::set_float(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(m_shader_programm, name.c_str()), value);
}
void CShader::set_vec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(m_shader_programm, name.c_str()), 1, &value[0]);
}
void CShader::set_vec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(m_shader_programm, name.c_str()), x, y);
}
void CShader::set_vec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(m_shader_programm, name.c_str()), 1, &value[0]);
}
void CShader::set_vec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(m_shader_programm, name.c_str()), x, y, z);
}
void CShader::set_vec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(m_shader_programm, name.c_str()), 1, &value[0]);
}
void CShader::set_vec4(const std::string &name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(m_shader_programm, name.c_str()), x, y, z, w);
}
void CShader::set_mat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(m_shader_programm, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void CShader::set_mat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_shader_programm, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void CShader::set_mat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_shader_programm, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
