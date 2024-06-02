#include <dspch.hpp>
#include <DawnStar/Renderer/Shader.hpp>


#include <fstream>
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

#include <DawnStar/Renderer/Renderer.hpp>
#include <DawnStar/Core/Filesystem.hpp>

namespace DawnStar
{
	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")						return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")	return GL_FRAGMENT_SHADER;
		// add higher opengl version
		// if (type == "compute")						return GL_COMPUTE_SHADER;

		DS_CORE_ASSERT(false, "Unknown shader type");
		return 0;
	}

	Shader::Shader(const std::filesystem::path &filepath)
	{
		DS_PROFILE_SCOPE()

		const std::string source = Filesystem::ReadFileText(filepath);
		const auto shaderSources = Preprocess(source);
		Compile(shaderSources, filepath.filename().string());
		_name = filepath.filename().string();

	}

	Shader::~Shader()
	{
		DS_PROFILE_SCOPE()
		
		glDeleteProgram(_rendererID);
	}

	
	void Shader::Recompile(const std::filesystem::path &filepath)
    {
		DS_PROFILE_SCOPE()

		glDeleteProgram(_rendererID);

		const std::string source = Filesystem::ReadFileText(filepath);
		const auto shaderSources = Preprocess(source);
		Compile(shaderSources, filepath.filename().string());
    }

    void Shader::Bind() const
	{
		DS_PROFILE_SCOPE()

		glUseProgram(_rendererID);
	}

	void Shader::Unbind() const
	{
		DS_PROFILE_SCOPE()

		glUseProgram(0);
	}
	void Shader::SetInt(const std::string& name, int value)
	{
		DS_PROFILE_SCOPE()
		
		UploadUniformInt(name, value);
	}

	void Shader::SetIntArray(const std::string& name, const int* values, uint32_t count)
	{
		DS_PROFILE_SCOPE()
		
		UploadUniformIntArray(name, values, count);		
	}

	void Shader::SetFloat(const std::string& name, float value)
	{
		DS_PROFILE_SCOPE()
		
		UploadUniformFloat(name, value);
	}

	void Shader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		DS_PROFILE_SCOPE()
		
		UploadUniformFloat2(name, value);
	}

	void Shader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		DS_PROFILE_SCOPE()
		
		UploadUniformFloat3(name, value);
	}

	void Shader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		DS_PROFILE_SCOPE()
		
		UploadUniformFloat4(name, value);
	}

	void Shader::SetMat3(const std::string& name, const glm::mat3& value)
	{
		DS_PROFILE_SCOPE()
		
		UploadUniformMat3(name, value);
	}

	void Shader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		DS_PROFILE_SCOPE()
		
		UploadUniformMat4(name, value);
	}

	void Shader::SetUniformBlock(const std::string& name, uint32_t blockIndex)
	{
		DS_PROFILE_SCOPE()

		glUniformBlockBinding(_rendererID, glGetUniformBlockIndex(_rendererID, name.c_str()), blockIndex);
	}

	void Shader::UploadUniformInt(const std::string& name, int value)
	{
		glUniform1i(GetLocation(name), value);
	}

	void Shader::UploadUniformIntArray(const std::string& name, const int* values, uint32_t count)
	{
		glUniform1iv(GetLocation(name), static_cast<int>(count), values);
	}

	void Shader::UploadUniformFloat(const std::string& name, float value)
	{
		glUniform1f(GetLocation(name), value);
	}

	void Shader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
	{
		glUniform2f(GetLocation(name), values.x, values.y);
	}

	void Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
	{
		glUniform3f(GetLocation(name), values.x, values.y, values.z);
	}

	void Shader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
	{
		glUniform4f(GetLocation(name), values.x, values.y, values.z, values.w);
	}
	
	void Shader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		glUniformMatrix3fv(GetLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(GetLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
	}
	
	int Shader::GetLocation(const std::string& name)
	{
		DS_PROFILE_SCOPE()

		const auto& it = _uniformLocationCache.find(name);
		if (it != _uniformLocationCache.end())
			return it->second;

		int location = glGetUniformLocation(_rendererID, name.c_str());
		_uniformLocationCache.emplace(name, location);
		return location;
	}

	std::unordered_map<GLenum, std::string> Shader::Preprocess(const std::string& source)
	{
		DS_PROFILE_SCOPE()

		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);

		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			DS_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			DS_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = 
				source.substr(nextLinePos, 
							  pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));

		}
		return shaderSources;
	}

	void Shader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources, const std::string& name)
	{
		DS_PROFILE_SCOPE()

		GLuint program = glCreateProgram();
		DS_CORE_ASSERT(shaderSources.size() <= 2, "We only support 2 shaders for now");
		std::array<GLenum, 2> glShaderIDs;
		int glShaderIDIndex = 0;

		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;
			
			GLuint shader = glCreateShader(type);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				DS_CORE_ERROR("[Shader Compile Error][{0}] {1}", name, infoLog.data());
				DS_CORE_ASSERT(false, "Shader compilation failure!");
				break;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}		

		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);


			DS_CORE_ERROR("[Shader Link Error][{0}] {1}", name, infoLog.data());
			DS_CORE_ASSERT(false, "Shader link failure!");

			return;
		}

		_rendererID = program;

		for (auto id : glShaderIDs)
			glDetachShader(program, id);
	}
	

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		DS_CORE_ASSERT(!Exists(name), "Shader already exists!");
		_shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const std::filesystem::path &filepath)
	{
		auto shader = CreateRef<Shader>(filepath);
		Add(shader);
		_shaderPaths[shader->GetName()] = filepath.string();
		return shader;
	}
	
	void ShaderLibrary::ReloadAll()
	{
		DS_PROFILE_SCOPE()

		std::string shaderName;
		for (const auto& [name, shader] : _shaders)
		{
			const auto& it = _shaderPaths.find(name);
			if (it == _shaderPaths.end())
				continue;

			shader->Recompile(it->second);
		}
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		DS_CORE_ASSERT(Exists(name), "Shader not found!");
		return _shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return _shaders.contains(name);
	}
}