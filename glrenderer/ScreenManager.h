/*
 * ScreenManager.h
 *
 *  Created on: 10/09/2011
 *      Author: Aeradon
 */

#ifndef SCREENMANAGER_H_
#define SCREENMANAGER_H_

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <sstream>
#include "../INC/PFR/point.hpp"
#include "../INC/Opcoes.h"
#include "../INC/Utils.hpp"
#include "resize++.h"

using std::string;

/**
 * Structure used to represent an OpenGL point.
 */
	struct GLpoint
	{
		GLfloat x;
		GLfloat y;
	};

/**
 * Structure used to represent an Image.
 */
class Image
{
public:
	GLuint texture;
	GLint depth;
	GLpoint position[4];
	GLfloat z;

	Image():		texture(-1),depth(-1),z(0)
	{}

	~Image()
	{}

	bool operator==(const Image &other) const
	{
		return ( (this->texture == other.texture) && (this->depth == other.depth));
	}

	bool operator!=(const Image &other) const
	{
		return (!(*this == other));
	}

	bool operator>(const Image &other) const
	{
		return (this->texture > other.texture);
	}

	bool operator<(const Image &other) const
	{
		return (this->texture < other.texture);
	}

	bool operator<=(const Image &other) const
	{
		return (this->texture <= other.texture);
	}

	bool operator>=(const Image &other) const
	{
		return (this->texture >= other.texture);
	}
};

class ScreenManager
{
public:

	// Number of stars
	// TODO Should be Static Const
	#define MAX_LINES 110
	#define MAX_COLUMNS 50
	#define MAX_OBJECTS (MAX_LINES * MAX_COLUMNS)
	#define MAX_TEXTURES 1900

	GLfloat zoom; /* Viewing Distance Away From Stars */

	Point camera;


	GLfloat objectVertices[MAX_OBJECTS *12];
	GLfloat objectNormals[MAX_OBJECTS *12];
	GLubyte objectIndices[MAX_OBJECTS *4];
	GLfloat objectTextures[MAX_OBJECTS * 12];

private:
	static ScreenManager* instance;
	static GLint TEXTURE_SIZE;
	static GLint TEXTURE_DEPTH;
	static GLint MAX_TEXTURE_SIZE;
	static GLint MAX_TEXTURE_DEPTH;
	static GLint MAX_IMAGE_SIZE;

public:
	static ScreenManager* GetInstance();
	virtual ~ScreenManager();

	int  InitGL();
	void Update();
	int  Render();

	void LoadTexture(string path, Image *image);
	void LoadMultipleTextures(std::string path, int lines, int columns, Image *image);
	void LoadSingleTexture(string path, Image *image);

private:
	ScreenManager();

	SDL_Surface * srf_screen;

	GLint iViewport[4];
	GLint g_iTextureWidth;
	GLint g_iTextureHeight;
	GLuint texture[1];					// Storage For One Texture ( NEW )

	std::vector<GLuint> v_texture;
	std::vector<GLint> v_depth;
	std::vector<Image> v_image;
	std::vector<Image> v_render;

	void GenerateTexture3D();

	void ConvertSurface2Screen(SDL_Surface * surface);
	void ResizeSurface2Texture(SDL_Surface * surface);
	SDL_Surface * LoadConvertResize(string path);
	void printFramebufferInfo();
	bool checkFramebufferStatus();
	void PrintTextureInfo();

	std::string getTextureParameters(GLuint id);
	std::string getRenderbufferParameters(GLuint id);
	std::string convertInternalFormatToString(GLenum format);
};

#endif /* SCREENMANAGER_H_ */
