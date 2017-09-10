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
	std::string vertexCode; //������ɫ������
	std::string fragmentCode; //Ƭԭ��ɫ������
	std::string geometryCode; //������ɫ������
	try
	{
		// ���ļ�
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);
		std::stringstream vShaderStream, fShaderStream;
		// ��ȡ�ļ�����string��
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();
		// �ر��ļ���
		vertexShaderFile.close();
		fragmentShaderFile.close();
		// ת����ΪC��������
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// �жϼ�����ɫ���Ĵ���
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
	// ��·����ȡ����ɫ���Ĵ��룬ֱ�ӱ���--
	Shader shader;
	shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
	return shader;

}

Texture2D ResourceManager::loadTextureFromFile(const GLchar * file ,GLboolean ahpa)
{
	Texture2D texture;
	
	//��ȡͼƬ
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
