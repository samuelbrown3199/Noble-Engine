#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>
#include <exception>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Resource.h"
#include "../Core/Logger.h"

/**
*Stores a shader source code file.
*/
struct Shader : public Resource
{
	std::shared_ptr<std::string> m_shaderCode;

	Shader()
	{
		m_resourceType = "Shader";
	}

	void OnLoad()override
	{
		std::ifstream shaderFile;
		shaderFile.open(m_sResourcePath);

		if (!shaderFile.is_open())
		{
			Logger::LogError(FormatString("Failed to load shader file %s", m_sResourcePath.c_str()), 2);
			throw std::exception();
		}
		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();

		m_shaderCode = std::make_shared<std::string>(shaderStream.str());
		m_bIsLoaded = true;
	}

	virtual void OnUnload()override
	{
		m_shaderCode.reset();
		m_bIsLoaded = false;
	};

	virtual void AddResource(std::string path)override;
	virtual std::vector<std::shared_ptr<Resource>> GetResourcesOfType()override;

	nlohmann::json AddToDatabase() override;
	std::shared_ptr<Resource> LoadFromJson(const std::string& path, const nlohmann::json& data) override;
	void SetDefaults(const nlohmann::json& data) override;
};

/**
*Stores a shader location GLint. It is more optimal to store these rather than to find them every frame.
*/
struct ShaderLocation
{
	std::string locationName;
	GLint locationID;

	static std::shared_ptr<ShaderLocation> CreateLocation(GLuint _shaderProgram, std::string _location)
	{
		std::shared_ptr<ShaderLocation> rtn = std::make_shared<ShaderLocation>();
		rtn->locationName = _location;
		rtn->locationID = glGetUniformLocation(_shaderProgram, _location.c_str());
		if (rtn->locationID == -1)
		{
			//std::cout << "Couldn't find location " << location << " in shader program " << shaderProgram << std::endl;
			//return a null ptr here and dont add to the locations list.
		}

		return rtn;
	}
};
/**
*Made up of several shader resources, these are used in rendering graphics within the engine. 
*/
struct ShaderProgram
{
	GLuint m_iProgramID;

	std::string m_shaderProgramID;

	std::weak_ptr<ShaderProgram> m_self;
	std::vector<std::shared_ptr<ShaderLocation>> m_vShaderLocations;

	std::shared_ptr<Shader> m_vertexShader;
	std::shared_ptr<Shader> m_fragmentShader;

public:
	ShaderProgram()
	{
		m_iProgramID = glCreateProgram();
	}
	/**
	*Gets a location from the list, or if it does not contain it creates and adds it to list.
	*/
	GLint GetLocation(std::string _location)
	{
		for (size_t i = 0; i < m_vShaderLocations.size(); i++)
		{
			if (m_vShaderLocations.at(i)->locationName == _location)
			{
				return m_vShaderLocations.at(i)->locationID;
			}
		}

		std::shared_ptr<ShaderLocation> rtn = ShaderLocation::CreateLocation(m_iProgramID, _location);
		m_vShaderLocations.push_back(rtn);
		return rtn->locationID;
	}
	/**
	*Binds a shader resource to the shader program.
	*/
	void BindShader(std::shared_ptr<Shader> _shader, GLenum _shaderType)
	{
		Logger::LogInformation(FormatString("Binding shader %s to shader program", _shader->m_sResourcePath.c_str()));

		const GLchar* shaderSource = _shader->m_shaderCode->c_str();
		GLuint shaderID = glCreateShader(_shaderType);
		glShaderSource(shaderID, 1, &shaderSource, NULL);
		glCompileShader(shaderID);
		GLint success = 0;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			Logger::LogError(FormatString("Failed to compile shader with path %s!", _shader->m_sResourcePath.c_str()), 2);
			GLint maxLength = 0;
			glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(shaderID, maxLength, &maxLength, &errorLog[0]);

			for (int i = 0; i < errorLog.size(); i++)
			{
				std::cout << errorLog[i];
			}

			glDeleteShader(shaderID);
			throw std::exception();
		}

		glAttachShader(m_iProgramID, shaderID);
	}
	/**
	*Binds a shader source code string to the shader program.
	*/
	void BindShader(const GLchar* _shaderSourceString, GLenum _shaderType)
	{
		Logger::LogInformation("Binding shader string to shader program");

		GLuint shaderID = glCreateShader(_shaderType);
		glShaderSource(shaderID, 1, &_shaderSourceString, NULL);
		glCompileShader(shaderID);
		GLint success = 0;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			Logger::LogError("Failed to compile shader with path passed by string!", 2);
			GLint maxLength = 0;
			glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(shaderID, maxLength, &maxLength, &errorLog[0]);

			for (int i = 0; i < errorLog.size(); i++)
			{
				std::cout << errorLog[i];
			}

			glDeleteShader(shaderID);
			throw std::exception();
		}

		glAttachShader(m_iProgramID, shaderID);
	}
	/**
	*This function binds an integer to a uniform int in the shader.
	*/
	void BindInt(std::string _location, int _value)
	{
		GLint intLocation = GetLocation(_location);
		glUniform1i(intLocation, _value);
	}
	/**
	*This function binds an float to a uniform int in the shader.
	*/
	void BindFloat(std::string _location, float _value)
	{
		GLint floatLocation = GetLocation(_location);
		glUniform1f(floatLocation, _value);
	}
	/**
	*This function binds an matrix4 to a uniform matrix4 in the shader.
	*/
	void BindMat4(std::string _location, glm::mat4 _matrix)
	{
		GLint matrixLocation = GetLocation(_location);
		glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, glm::value_ptr(_matrix));
	}
	/**
	*This function binds an vec3 to a uniform vec3 in the shader.
	*/
	void BindVector3(std::string _location, glm::vec3 _vector)
	{
		GLint vectorLocation = GetLocation(_location);
		glUniform3f(vectorLocation, _vector.x, _vector.y, _vector.z);
	}
	/**
	*This function binds an vec3 to a uniform vec3 in the shader.
	*/
	void BindVector4(std::string _location, glm::vec4 _vector)
	{
		GLint vectorLocation = GetLocation(_location);
		glUniform4f(vectorLocation, _vector.x, _vector.y, _vector.z, _vector.w);
	}
	/**
	*Links the program for use in the engine. Also responsible for setting up the uniform variables in the shader code. If this function isn't called then the shader program cannot be used.
	*/
	void LinkShaderProgram(std::shared_ptr<ShaderProgram> _selfPtr)
	{
		Logger::LogInformation("Linking shader program to program.");

		m_self = _selfPtr;
		GLint success = 0;
		glLinkProgram(m_iProgramID);
		glGetProgramiv(m_iProgramID, GL_LINK_STATUS, &success);

		if (success == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_iProgramID, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(m_iProgramID, maxLength, &maxLength, &errorLog[0]);

			for (int i = 0; i < errorLog.size(); i++)
			{
				std::cout << errorLog[i];
			}

			glDeleteProgram(m_iProgramID);
			throw std::exception();
		}
	}
	/**
	*Sets the currently used shader program to this.
	*/
	void UseProgram()
	{
		glUseProgram(m_iProgramID);
	}

	void ChangeShader(std::shared_ptr<Shader> shader, std::shared_ptr<Shader> targetShader)
	{
		if (shader == nullptr)
			return;

		if (targetShader != nullptr && shader->m_sLocalPath == targetShader->m_sLocalPath)
			return;
		
		targetShader = shader;
	}

	void DoShaderProgramInterface();
};