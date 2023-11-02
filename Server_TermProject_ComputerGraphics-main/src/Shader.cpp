#include "stdafx.h"
#include "Shader.h"
#include "File.h"

static GLint shaders[NUM_OF_SHADER];

GLint Make_ShaderProgram(const Shader& shader);
GLint Make_VertexShaders(const string& name);
GLint Make_FragmentShaders(const string& name);

GLvoid shd::Init()
{
	for (GLsizei i = 0; i < NUM_OF_SHADER; ++i)
	{
		const Shader shader = static_cast<Shader>(i);
		GLint* shaderProgram = &shaders[i];
		*shaderProgram = Make_ShaderProgram(shader);
	}
}
GLint shd::GetShaderProgram(const Shader& shader)
{
	return shaders[static_cast<GLint>(shader)];
}
GLvoid shd::Use(const Shader& shader)
{
	glUseProgram(GetShaderProgram(shader));
}
GLint Make_ShaderProgram(const Shader& shader)
{
	string vertexShaderName;
	string fragShaderName;
	switch (shader)
	{
	case Shader::Color:
		vertexShaderName = "color";
		fragShaderName = "color";
		break;
	case Shader::Light:
		vertexShaderName = "light";
		fragShaderName = "light";
		break;
	case Shader::Texture:
		vertexShaderName = "texture";
		fragShaderName = "texture";
		break;
	case Shader::Back:
		vertexShaderName = "back";
		fragShaderName = "back";
		break;
	default:
		assert(0);
		break;
	}

	vertexShaderName += "_vert.glsl";
	fragShaderName += "_frag.glsl";

	GLint vertexShader = Make_VertexShaders(vertexShaderName);
	GLint fragShader = Make_FragmentShaders(fragShaderName);

	GLint ShaderProgramID = glCreateProgram();

	glAttachShader(ShaderProgramID, vertexShader);
	glAttachShader(ShaderProgramID, fragShader);

	glLinkProgram(ShaderProgramID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

	GLint result;
	GLchar errorLog[512];
	glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);
		std::cerr << "[ ERROR ] make shader program\n";
		printf("[ error :: shader program ]%s\n", errorLog);
		assert(0);
	}

	return ShaderProgramID;
}

GLint Make_VertexShaders(const string& name)
{
	GLchar* vertexSource = nullptr;

	const string path = "shaders\\" + name;

	vertexSource = FileToBuffer(path.c_str());
	if (vertexSource == nullptr)
	{
		assert(0);
	}
	cout << "Load Vertex Shader: " << path << endl;

	GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, NULL);
	glCompileShader(vertexShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "[ ERROR ] make vertex shaders\n";
		printf("Vertex Error : %s\n", errorLog);
		printf("[ error :: vertex ]%s\n", errorLog);
		assert(0);
	}

	return vertexShader;
}
GLint Make_FragmentShaders(const string& name)
{
	GLchar* fragmentSource = nullptr;
	const string path = "shaders\\" + name;

	fragmentSource = FileToBuffer(path.c_str());
	if (fragmentSource == nullptr)
	{
		assert(0);
	}
	cout << "Load Fragment Shader : " << path << endl;

	GLint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, (const GLchar**)&fragmentSource, NULL);
	glCompileShader(fragShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragShader, 512, NULL, errorLog);
		std::cerr << "[ ERROR ] make fragment shaders\n";
		printf("Fragment Error : %s\n", errorLog);
		printf("[ error :: fragment ]%s\n", errorLog);
		assert(0);
	}

	return fragShader;
}







GLvoid shd::SetShader(const Shader& shader, const GLchar* name, const glm::mat4& transform)
{
	const GLint location = glGetUniformLocation(GetShaderProgram(shader), name);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
}

GLvoid shd::SetShader(const Shader& shader, const GLchar* name, const glm::vec3& value)
{
	unsigned int location = glGetUniformLocation(GetShaderProgram(shader), name);
	glUniform3f(location, value.x, value.y, value.z);
}
GLvoid shd::SetShader(const Shader& shader, const GLchar* name, const GLfloat& value)
{
	unsigned int location = glGetUniformLocation(GetShaderProgram(shader), name);
	glUniform1f(location, value);
}