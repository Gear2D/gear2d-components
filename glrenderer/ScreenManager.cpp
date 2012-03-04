/*
 * ScreenManager.cpp
 *
 *  Created on: 10/09/2011
 *      Author: Aeradon
 */

#include "ScreenManager.h"

ScreenManager* ScreenManager::instance = NULL;

GLint ScreenManager::TEXTURE_SIZE = 128;
GLint ScreenManager::TEXTURE_DEPTH = 1;
GLint ScreenManager::MAX_TEXTURE_SIZE = 128;
GLint ScreenManager::MAX_TEXTURE_DEPTH = 1;
GLint ScreenManager::MAX_IMAGE_SIZE = 128;

ScreenManager* ScreenManager::GetInstance()
{ b
	if (!instance)
	{
		instance = new ScreenManager();
	}
	return instance;
}

ScreenManager::ScreenManager():
				zoom(5.0),
				camera(20,20)

{
	/* Limpeza nos buffers */
	memset(objectNormals,0,sizeof(GLfloat*)*MAX_OBJECTS *12);
	memset(objectVertices,0,sizeof(GLfloat*)*MAX_OBJECTS *12);
	memset(objectTextures,0,sizeof(GLfloat)*MAX_OBJECTS *12);
	memset(objectIndices,0,sizeof(GLubyte*)*MAX_OBJECTS *4);

	/* Flags to pass to SDL_SetVideoMode */
// 	int videoFlags;
// 
// 	/* this holds some info about our display */
// 	const SDL_VideoInfo *videoInfo;
// 
// 	/* initialize SDL */
// 	if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
// 	{
// 		fprintf( stderr, "Video initialization failed: %s\n", SDL_GetError() );
// 		exit( 1 );
// 	}

// 	/* Fetch the video info */
// 	videoInfo = SDL_GetVideoInfo( );
// 
// 	if ( !videoInfo )
// 	{
// 		fprintf( stderr, "Video query failed: %s\n", SDL_GetError( ) );
// 		exit( 1 );
// 	}
// 
// 	/* the flags to pass to SDL_SetVideoMode */
// 	videoFlags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
// 	videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
// 	videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */

	/* This checks to see if surfaces can be stored in memory */
// 	if ( videoInfo->hw_available )
// 		videoFlags |= SDL_HWSURFACE;
// 	else
// 		videoFlags |= SDL_SWSURFACE;
// 
// 	/* This checks if hardware blits can be done */
// 	if ( videoInfo->blit_hw )
// 		videoFlags |= SDL_HWACCEL;
// 
// 	/* Sets up OpenGL double buffering */
// 	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
// 
// 	/* get a SDL surface */
// 	srf_sreen = SDL_SetVideoMode( Opcoes::GetInstance()->GetResolution().resolution.x, Opcoes::GetInstance()->GetResolution().resolution.y, Opcoes::GetInstance()->GetResolution().bpp, videoFlags );
// 
// 	/* Verify there is a surface */
// 	if ( !srf_screen )
// 	{
// 		fprintf( stderr,  "Video mode set failed: %s\n", SDL_GetError( ) );
// 		exit( 1 );
// 	}
// 
// 	LOG("Screen Created.\n");

// 	/* initialize OpenGL */
// 	if (!(InitGL()) )
// 	{
// 		fprintf( stderr,  "Unable to Initialize OpenGL: %s\n", SDL_GetError( ) );
// 		exit( 1 );
// 	}
// }
// 
// /* general OpenGL initialization function */
// // int ScreenManager::InitGL()
// // {
// 	glewInit();
// 
// 	// Get max values
// 	int teste;
// 	glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &teste);
// 	ScreenManager::MAX_TEXTURE_SIZE = teste;
// 	ScreenManager::MAX_TEXTURE_DEPTH = teste;
// 	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &teste);
// 	ScreenManager::MAX_IMAGE_SIZE = teste;
// 	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &teste);
// 	if (teste < ScreenManager::TEXTURE_DEPTH)
// 		ScreenManager::MAX_TEXTURE_DEPTH = teste;
// 
// 	// Values Arbitrarily set
// 	ScreenManager::TEXTURE_SIZE = 128;
// 	ScreenManager::TEXTURE_DEPTH = MAX_TEXTURE_DEPTH;
// 
// #ifdef DEBUG
// 	PrintTextureInfo();
// #endif

	// Enable Texture Mapping
	//glEnable(GL_TEXTURE_2D);
	//glEnable(GL_TEXTURE_2D_ARRAY_EXT);
	//glEnable(GL_TEXTURE_2D_ARRAY);
	glEnable(GL_TEXTURE_3D);

	// shading mathod: GL_SMOOTH or GL_FLAT
	//glShadeModel( GL_FLAT );

	// background color
	glClearColor( 0.0, 0.0, 0.0, 0.0);

	/* Depth buffer setup */
	glClearDepth( 1.0 );
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Perspective Calculations: GL_NICEST or GL_FASTEST  */
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST );
	//glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

	// Enable Blending
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glEnable(GL_BLEND);
	glDisable(GL_BLEND);

	//TODO Enable Face Culling
	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	//glClearStencil(0);                          // clear stencil buffer
	glClearDepth(1.0f);                         // 0 is near, 1 is far

	// Lighting initialization
	// Disable Lightning
	glDisable(GL_LIGHTING);
	//initLights();

	// Set The Perpective view
	// Make sure we're chaning the projection
	glMatrixMode( GL_PROJECTION );
	// Reset The View
	glLoadIdentity( );
	// Set our perspective
	gluPerspective(50.0, (GLfloat)(Opcoes::GetInstance()->GetResolution().resolution.x  / Opcoes::GetInstance()->GetResolution().resolution.y), 0.1f, 100.0f );

	LOG("OpenGL Initialized.\n");

	return(true);
}


ScreenManager::~ScreenManager()
{
	// TODO Finalizar a janela
	SDL_Quit();
}

void ScreenManager::GenerateTexture3D()
{
	v_texture.push_back(0);
	glGenTextures(1, &(v_texture.back()));

	if (v_texture.back() < 0)
	{
		ERRO("Error: Couldn't generate initial textures.\n");
		v_texture.pop_back();
		return;
	}

	v_depth.push_back(0);
}

SDL_Surface * ScreenManager::LoadConvertResize(string path)
{
	SDL_Surface * srf_Loaded;

	srf_Loaded = IMG_Load(path.c_str());
	if (!srf_Loaded)
	{
		fprintf(stderr, "ERROR: Impossible to load image %s.", path.c_str());
		exit(1);
	}

	ResizeSurface2Texture(srf_Loaded);

	ConvertSurface2Screen(srf_Loaded);

	return srf_Loaded;
}

void ScreenManager::ResizeSurface2Texture(SDL_Surface * surface)
{
	SDL_Surface * srf_Resized;
	srf_Resized = SDL_Resize(surface, ScreenManager::TEXTURE_SIZE, ScreenManager::TEXTURE_SIZE);
	SDL_FreeSurface(surface);
	surface = srf_Resized;
}

// Sets SDL_RLEACCEL flag and converts image to screen format.
void ScreenManager::ConvertSurface2Screen(SDL_Surface * surface)
{
	SDL_Surface * srf_Converted;
	SDL_SetAlpha(surface,(SDL_RLEACCEL|SDL_SRCALPHA),255);
	srf_Converted = SDL_DisplayFormatAlpha(surface);
	SDL_FreeSurface(surface);
	surface = srf_Converted;
}

void ScreenManager::LoadTexture(std::string path, Image *image)
{
	SDL_Surface * sdlImage;
	/* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
	if ( (sdlImage = LoadConvertResize(path.c_str()) ) )
	{
		// Olha se tem espaco no vetor de texturas
		if (v_texture.empty() || (v_depth.back() == ScreenManager::TEXTURE_DEPTH) )
			GenerateTexture3D();

		// load images as 2d texture array
		glBindTexture( GL_TEXTURE_3D, v_texture.back());

		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Use GL_LINEAR to turn on interpolation
		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Use GL_NEAREST to turn off interpolation
		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexParameteri( GL_TEXTURE_3D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);      // 4-byte pixel alignment

		glTexImage3D(GL_TEXTURE_3D, 0, 4, ScreenManager::TEXTURE_SIZE, ScreenManager::TEXTURE_SIZE, ScreenManager::TEXTURE_DEPTH, 0, GL_ALPHA, GL_UNSIGNED_BYTE, NULL);
		glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, v_depth.back(),  ScreenManager::TEXTURE_SIZE, ScreenManager::TEXTURE_SIZE, 1, GL_ALPHA, GL_UNSIGNED_BYTE, sdlImage->pixels);

		//TODO Criar a textura, colocar no vetor da ScreenManager (junto com o caminho dela)
		//Retornar so o ponteiro pra ela
		image->texture = v_texture.back();
		image->depth = v_depth.back();
		image->z = 0;
		memset(image->position,0, 4*sizeof(GLpoint));

		++(v_depth.back());
		glBindTexture( GL_TEXTURE_3D, 0);
	}

	/* Free up any memory we may have used */
	if ( sdlImage )	SDL_FreeSurface( sdlImage);
}

void ScreenManager::LoadMultipleTextures(std::string path, int lines, int columns, Image *image)
{
	SDL_Surface * sdlImage;
	/* Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit */
	if ( (sdlImage = IMG_Load(path.c_str()) ) )
	{
		// Separar cada uma das imagens, Dar resize na imagem, Gerar a textura, Setar informaçoes
		int stepX = sdlImage->w/columns;
		int stepY = sdlImage->h/lines;
		SDL_Rect rect;
		rect.w = stepX;
		rect.h = stepY;

		SDL_Surface * sdlSnip = SDL_CreateRGBSurface(SDL_SWSURFACE, ScreenManager::TEXTURE_SIZE, ScreenManager::TEXTURE_SIZE,
					Opcoes::GetInstance()->GetResolution().bpp, 0, 0, 0, 0);

		// load images as 2d texture array
		glBindTexture( GL_TEXTURE_3D, v_texture.back());

		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Use GL_LINEAR to turn on interpolation
		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Use GL_NEAREST to turn off interpolation
		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexParameteri( GL_TEXTURE_3D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);      // 4-byte pixel alignment

		glTexImage3D(GL_TEXTURE_3D, 0, 4, ScreenManager::TEXTURE_SIZE, ScreenManager::TEXTURE_SIZE, ScreenManager::TEXTURE_DEPTH, 0, GL_ALPHA, GL_UNSIGNED_BYTE, NULL);

		int i = 0;
		for (int c = 0; c < sdlImage->w; c += stepX)
		{
			for (int l = 0; l < sdlImage->h; l += stepY)
			{
				// Olha se tem espaco no vetor de texturas
				if (v_texture.empty() || (v_depth.back() == ScreenManager::TEXTURE_DEPTH) )
					GenerateTexture3D();

				// Separar cada uma das imagens, Ajustar o Rect
				rect.x = c;
				rect.y = l;
				SDL_BlitSurface(sdlImage,&rect,sdlSnip,NULL);

				// Dar resize na imagem
				ResizeSurface2Texture(sdlSnip);
				ConvertSurface2Screen(sdlSnip);

				// Salvar a textura
				glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, v_depth.back(),  ScreenManager::TEXTURE_SIZE, ScreenManager::TEXTURE_SIZE, 1, GL_ALPHA, GL_UNSIGNED_BYTE, sdlSnip->pixels);

				//TODO Criar a textura, colocar no vetor da ScreenManager (junto com o caminho dela)
				//Retornar so o ponteiro pra ela
				// Setar informaçoes
				image[i].texture = v_texture.back();
				image[i].depth = v_depth.back();
				image[i].z = 0;
				memset(image[i].position,0, 4*sizeof(GLpoint));

				++(v_depth.back());
				++i;
			}
		}
		glBindTexture( GL_TEXTURE_3D, 0);

		// Free up any memory we may have used
		if ( sdlSnip )	SDL_FreeSurface( sdlSnip);
	}

	if ( sdlImage )	SDL_FreeSurface( sdlImage);

}

void ScreenManager::LoadSingleTexture(std::string path, Image *image)
{
	// Ordenacao do vetor
	//std::sort(v_render.begin(), indice_ultimoElementoInserido);
/*

Esse metodo deve carregar texturas normais, 2D, que devem ser guardadas em camadas solitarias de blit.
Acredito que essas imagens devam ser blitadas antes das texturas padrão.

1. Carregar imagem
2. Arredondar para a proxima potencia de 2
3. Testar o máximo
4. Fazer as otimizações
5. Criar a textura
6. Salvar o valor da textura
7. Retornar o valor da textura
	SINGLE!

//Round to the next power of 2
val = ... // Get input
val--;
val = (val >> 1) | val;
val = (val >> 2) | val;
val = (val >> 4) | val;
val = (val >> 8) | val;
val = (val >> 16) | val;
val++; // Val is now the next highest power of 2.

 */
}

/* set camera position and lookat direction */
void ScreenManager::Update()
{
	/*
		// TODO Remover este codigo daqui
		int n;
		n = 0;
		for (int x = 0; x< MAX_LINES; x++)
		{
			for (int y = 0; y > -MAX_COLUMNS; y--)
			{
				objectVertices[n++] = x;		objectVertices[n++] =-y;		objectVertices[n++] = 0;
				objectVertices[n++] = x+1;		objectVertices[n++] = -y;		objectVertices[n++] = 0;
				objectVertices[n++] = x+1;		objectVertices[n++] = -y-1;		objectVertices[n++] = 0;
				objectVertices[n++] = x;		objectVertices[n++] = -y-1;		objectVertices[n++] = 0;
			}
		}

		n = 0;
		for (int x = 0; x< MAX_LINES; x++)
		{
			for (int y = 0; y< MAX_COLUMNS; y++)
			{
				objectNormals[n++] = 0;		objectNormals[n++] = 0;		objectNormals[n++] = 1;
				objectNormals[n++] = 0;		objectNormals[n++] = 0;		objectNormals[n++] = 1;
				objectNormals[n++] = 0;		objectNormals[n++] = 0;		objectNormals[n++] = 1;
				objectNormals[n++] = 0;		objectNormals[n++] = 0;		objectNormals[n++] = 1;
			}
		}

		n = 0;
		for (int x = 0; x< MAX_LINES; x++)
		{
			for (int y = 0; y< MAX_COLUMNS; y++)
			{
				float color = (float)(rand() % MAX_TEXTURES);
				color = (float)((color + 0.5) / MAX_TEXTURES);

				objectTextures[n++] = 0;		objectTextures[n++] = 0;		objectTextures[n++] = color;
				objectTextures[n++] = 1;		objectTextures[n++] = 0;		objectTextures[n++] = color;
				objectTextures[n++] = 1;		objectTextures[n++] = 1;		objectTextures[n++] = color;
				objectTextures[n++] = 0;		objectTextures[n++] = 1;		objectTextures[n++] = color;
			}
		}

		for (int x = 0; x< MAX_LINES*MAX_COLUMNS*12; x++)
		{
			objectIndices[x] = x;
		}

		// END
		 */

	// Make sure we're chaning the model view and not the projection
	glMatrixMode( GL_MODELVIEW );

	// Reset The View
	glLoadIdentity( );

	// Set the camera
	gluLookAt(camera.x, camera.y, zoom,camera.x, camera.y, 0, 0, 1, 0);
}

/* Here goes our drawing code */
int ScreenManager::Render()
{
	/* These are to calculate our fps */
	static GLint T0     = 0;
	static GLint Frames = 0;

	// clear framebuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// TODO DRAW SOMETHING
	glBindTexture(GL_TEXTURE_3D, texture[0]);

	//glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_INDEX_ARRAY);

	//glNormalPointer(GL_FLOAT, 0, objectNormals);
	glTexCoordPointer(3,GL_FLOAT,0,objectTextures);
	glVertexPointer(3, GL_FLOAT, 0, objectVertices);
	//glIndexPointer(GL_UNSIGNED_BYTE, 0, objectIndices);

	//glPushMatrix();
	//glDrawElements(GL_QUADS,MAX_OBJECTS*12,GL_UNSIGNED_BYTE,objectIndices);
	//glDrawRangeElements(GL_QUADS, 0, 11, 12, GL_UNSIGNED_BYTE, objectIndices);
	//glDrawRangeElements(GL_QUADS, 12, 23, 12, GL_UNSIGNED_BYTE, objectIndices + 12);
	glDrawArrays(GL_QUADS,0,MAX_OBJECTS * 4);
	//glPopMatrix();

	//glDisableClientState(GL_INDEX_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//glDisableClientState(GL_NORMAL_ARRAY);


	glBindTexture(GL_TEXTURE_3D, 0);

	//draw info messages
	//showInfo();
	//showFPS();

	/* Draw it to the screen */
	SDL_GL_SwapBuffers( );
	//RENDERIZACAO END

	/* Gather our frames per second */
	Frames++;
	{
		GLint t = SDL_GetTicks();
		if (t - T0 >= 5000) {
			GLfloat seconds = (t - T0) / 1000.0;
			GLfloat fps = Frames / seconds;
			printf("%d frames in %g seconds = %g FPS\n", Frames, seconds, fps);
			fflush(stdout);
			T0 = t;
			Frames = 0;
		}
	}

	return( true );
}

/*
 * check FBO completeness
 */
bool ScreenManager::checkFramebufferStatus()
{
	// check FBO status
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	switch(status)
	{
	case GL_FRAMEBUFFER_COMPLETE_EXT:
		printf("Framebuffer complete.\n");
		return true;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
		printf("[ERROR] Framebuffer incomplete: Attachment is NOT complete.\n");
		return false;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
		printf("[ERROR] Framebuffer incomplete: No image is attached to FBO.\n");
		return false;

	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		printf("[ERROR] Framebuffer incomplete: Attached images have different dimensions.\n");
		return false;

	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
		printf("[ERROR] Framebuffer incomplete: Color attached images have different internal formats.\n");
		return false;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
		printf("[ERROR] Framebuffer incomplete: Draw buffer.\n");
		return false;

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
		printf("[ERROR] Framebuffer incomplete: Read buffer.\n");
		return false;

	case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
		printf("[ERROR] Unsupported by FBO implementation.\n");
		return false;

	default:
		printf("[ERROR] Unknow error.\n");
		return false;
	}
}

/*
 * print out the FBO info
 */
void ScreenManager::printFramebufferInfo()
{
	printf("\n***** FBO STATUS *****\n");

	// print max # of colorbuffers supported by FBO
	int colorBufferCount = 0;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &colorBufferCount);
	printf("Max Number of Color Buffer Attachment Points: %d,\n",colorBufferCount);

	int objectType;
	int objectId;

	// print info of the colorbuffer attachable image
	for(int i = 0; i < colorBufferCount; ++i)
	{
		glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT,
				GL_COLOR_ATTACHMENT0_EXT+i,
				GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT,
				&objectType);
		if(objectType != GL_NONE)
		{
			glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT,
					GL_COLOR_ATTACHMENT0_EXT+i,
					GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT,
					&objectId);

			std::string formatName;

			printf("Color Attachment %d: ",i);
			if(objectType == GL_TEXTURE)
				printf("GL_TEXTURE, %s \n",getTextureParameters(objectId).c_str());
			else if(objectType == GL_RENDERBUFFER_EXT)
				printf("GL_RENDERBUFFER_EXT, %s\n",getRenderbufferParameters(objectId).c_str());
		}
	}

	// print info of the depthbuffer attachable image
	glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT,
			GL_DEPTH_ATTACHMENT_EXT,
			GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT,
			&objectType);
	if(objectType != GL_NONE)
	{
		glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT,
				GL_DEPTH_ATTACHMENT_EXT,
				GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT,
				&objectId);

		printf("Depth Attachment: ");
		switch(objectType)
		{
		case GL_TEXTURE:
			printf("GL_TEXTURE, %s\n",getTextureParameters(objectId).c_str());
			break;
		case GL_RENDERBUFFER_EXT:
			printf("GL_RENDERBUFFER_EXT, %s\n",getRenderbufferParameters(objectId).c_str());
			break;
		}
	}

	// print info of the stencilbuffer attachable image
	glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT,
			GL_STENCIL_ATTACHMENT_EXT,
			GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT,
			&objectType);
	if(objectType != GL_NONE)
	{
		glGetFramebufferAttachmentParameterivEXT(GL_FRAMEBUFFER_EXT,
				GL_STENCIL_ATTACHMENT_EXT,
				GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT,
				&objectId);

		printf("Stencil Attachment: ");
		switch(objectType)
		{
		case GL_TEXTURE:
			printf("GL_TEXTURE, %s\n", getTextureParameters(objectId).c_str());
			break;
		case GL_RENDERBUFFER_EXT:
			printf("GL_RENDERBUFFER_EXT, %s\n", getRenderbufferParameters(objectId).c_str());
			break;
		}
	}

	printf("\n");
}

void ScreenManager::PrintTextureInfo()
{
	int teste;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &teste);
	printf("\n\nGL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = %d\n\n", teste);
	glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &teste);
	printf("\n\nGL_MAX_3D_TEXTURE_SIZE = %d\n\n", teste);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &teste);
	printf("\n\nGL_MAX_TEXTURE_IMAGE_UNITS = %d\n\n", teste);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &teste);
	printf("\n\nGL_MAX_TEXTURE_SIZE = %d\n\n", teste);
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &teste);
	printf("\n\nGL_MAX_TEXTURE_UNITS = %d\n\n", teste);
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &teste);
	printf("\n\nGL_MAX_VERTEX_TEXTURE_IMAGE_UNITS = %d\n\n", teste);
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &teste);
	printf("\n\nGL_MAX_VERTEX_UNIFORM_COMPONENTS = %d\n\n", teste);
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &teste);
	printf("\n\nGL_MAX_ARRAY_TEXTURE_LAYERS = %d\n\n", teste);
	fflush(stdout);
}

/*
 * return texture parameters as string using glGetTexLevelParameteriv()
 */
std::string ScreenManager::getTextureParameters(GLuint id)
{
	if(glIsTexture(id) == GL_FALSE)
		return "Not texture object";

	int width, height, format;
	std::string formatName;
	glBindTexture(GL_TEXTURE_2D, id);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);            // get texture width
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);          // get texture height
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format); // get texture internal format
	glBindTexture(GL_TEXTURE_2D, 0);

	formatName = convertInternalFormatToString(format);

	std::stringstream ss;
	ss << width << "x" << height << ", " << formatName;
	return ss.str();
}

/*
 * return renderbuffer parameters as string using glGetRenderbufferParameterivEXT
 */
std::string ScreenManager::getRenderbufferParameters(GLuint id)
{
	if(glIsRenderbufferEXT(id) == GL_FALSE)
		return "Not Renderbuffer object";

	int width, height, format;
	std::string formatName;
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, id);
	glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_WIDTH_EXT, &width);    // get renderbuffer width
	glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_HEIGHT_EXT, &height);  // get renderbuffer height
	glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_INTERNAL_FORMAT_EXT, &format); // get renderbuffer internal format
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	formatName = convertInternalFormatToString(format);

	std::stringstream ss;
	ss << width << "x" << height << ", " << formatName;
	return ss.str();
}

/*
 * convert OpenGL internal format enum to string
 */
std::string ScreenManager::convertInternalFormatToString(GLenum format)
{
	std::string formatName;

	switch(format)
	{
	case GL_STENCIL_INDEX:
		formatName = "GL_STENCIL_INDEX";
		break;
	case GL_DEPTH_COMPONENT:
		formatName = "GL_DEPTH_COMPONENT";
		break;
	case GL_ALPHA:
		formatName = "GL_ALPHA";
		break;
	case GL_RGB:
		formatName = "GL_RGB";
		break;
	case GL_RGBA:
		formatName = "GL_RGBA";
		break;
	case GL_LUMINANCE:
		formatName = "GL_LUMINANCE";
		break;
	case GL_LUMINANCE_ALPHA:
		formatName = "GL_LUMINANCE_ALPHA";
		break;
	case GL_ALPHA4:
		formatName = "GL_ALPHA4";
		break;
	case GL_ALPHA8:
		formatName = "GL_ALPHA8";
		break;
	case GL_ALPHA12:
		formatName = "GL_ALPHA12";
		break;
	case GL_ALPHA16:
		formatName = "GL_ALPHA16";
		break;
	case GL_LUMINANCE4:
		formatName = "GL_LUMINANCE4";
		break;
	case GL_LUMINANCE8:
		formatName = "GL_LUMINANCE8";
		break;
	case GL_LUMINANCE12:
		formatName = "GL_LUMINANCE12";
		break;
	case GL_LUMINANCE16:
		formatName = "GL_LUMINANCE16";
		break;
	case GL_LUMINANCE4_ALPHA4:
		formatName = "GL_LUMINANCE4_ALPHA4";
		break;
	case GL_LUMINANCE6_ALPHA2:
		formatName = "GL_LUMINANCE6_ALPHA2";
		break;
	case GL_LUMINANCE8_ALPHA8:
		formatName = "GL_LUMINANCE8_ALPHA8";
		break;
	case GL_LUMINANCE12_ALPHA4:
		formatName = "GL_LUMINANCE12_ALPHA4";
		break;
	case GL_LUMINANCE12_ALPHA12:
		formatName = "GL_LUMINANCE12_ALPHA12";
		break;
	case GL_LUMINANCE16_ALPHA16:
		formatName = "GL_LUMINANCE16_ALPHA16";
		break;
	case GL_INTENSITY:
		formatName = "GL_INTENSITY";
		break;
	case GL_INTENSITY4:
		formatName = "GL_INTENSITY4";
		break;
	case GL_INTENSITY8:
		formatName = "GL_INTENSITY8";
		break;
	case GL_INTENSITY12:
		formatName = "GL_INTENSITY12";
		break;
	case GL_INTENSITY16:
		formatName = "GL_INTENSITY16";
		break;
	case GL_R3_G3_B2:
		formatName = "GL_R3_G3_B2";
		break;
	case GL_RGB4:
		formatName = "GL_RGB4";
		break;
	case GL_RGB5:
		formatName = "GL_RGB4";
		break;
	case GL_RGB8:
		formatName = "GL_RGB8";
		break;
	case GL_RGB10:
		formatName = "GL_RGB10";
		break;
	case GL_RGB12:
		formatName = "GL_RGB12";
		break;
	case GL_RGB16:
		formatName = "GL_RGB16";
		break;
	case GL_RGBA2:
		formatName = "GL_RGBA2";
		break;
	case GL_RGBA4:
		formatName = "GL_RGBA4";
		break;
	case GL_RGB5_A1:
		formatName = "GL_RGB5_A1";
		break;
	case GL_RGBA8:
		formatName = "GL_RGBA8";
		break;
	case GL_RGB10_A2:
		formatName = "GL_RGB10_A2";
		break;
	case GL_RGBA12:
		formatName = "GL_RGBA12";
		break;
	case GL_RGBA16:
		formatName = "GL_RGBA16";
		break;
	default:
		formatName = "Unknown Format";
		break;
	}

	return formatName;
}


