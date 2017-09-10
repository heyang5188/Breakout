#include "ResourceManager.h"
#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include <iostream>
#include <sstream>
#include <fstream>



Shader ResourceManager::LoadShader(const GLchar * vShaderFile, const GLchar * fShaderFile, const GLchar * gShaderFile, std::string name)
{
	Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	return Shaders[name];
	
}

Shader ResourceManager::GetShader(std::string name)
{
	return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const GLchar * file, GLboolean alpha, std::string name)
{
	Textures[name] = loadTextureFromFile(file, alpha);
	return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
	return Textures[name];
}

void ResourceManager::Clear()
{
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);
	for (auto iter : Textures)
		glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const GLchar * vShaderFile, const GLchar * fShaderFile, const GLchar * gShaderFile)
{
	std::string vertexCode; //定点着色器代码
	std::string fragmentCode; //片原着色器代码
	std::string geometryCode; //几何着色器代码
	try
	{
		// 打开文件
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);
		std::stringstream vShaderStream, fShaderStream;
		// 读取文件流到string流
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();
		// 关闭文件流
		vertexShaderFile.close();
		fragmentShaderFile.close();
		// 转换成为C风格的数组
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// 判断几何着色器的存在
		if (gShaderFile != nullptr)
		{
			std::ifstream geometryShaderFile(gShaderFile);
			std::stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::exception e)
	{
		std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
	}
	const GLchar *vShaderCode = vertexCode.c_str();
	const GLchar *fShaderCode = fragmentCode.c_str();
	const GLchar *gShaderCode = geometryCode.c_str();
	// 从路径获取了着色器的代码，直接编译--
	Shader shader;
	shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
	return shader;

}

Texture2D ResourceManager::loadTextureFromFile(const GLchar * file ,GLboolean ahpa)
{
	Texture2D texture;
	
	//读取图片
	int width, height, nrComponents;
	unsigned char* image = stbi_load(file,&width,&height,&nrComponents,0);
	if (image)
	{
		if (nrComponents == 1)
			texture.Image_Format = GL_RED;
		else if (nrComponents == 3)
			texture.Image_Format = GL_RGB;
		else if (nrComponents == 4)
			texture.Image_Format = GL_RGBA;
	}
	texture.Generate(width, height,image);

	stbi_image_free(image);

	return texture;
}
