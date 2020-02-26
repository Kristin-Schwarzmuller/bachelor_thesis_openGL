#pragma once


// Includes																																														 Includes
//	===========================================================================================================================================================================================		#####
#include <string>
//#include <gl_core_4_4.hpp>
#include <glad.h>
//	===========================================================================================================================================================================================		#####


namespace cgbv
{
	namespace textures
	{
		// Texture																																												  Texture
		//	===================================================================================================================================================================================		#####
		class Texture
		{
		protected:
			unsigned int textureID;

		public:
			Texture();
			~Texture();

			unsigned int getTextureID()
			{
				return textureID;
			}
		};




		// Texture2D																																											Texture2D
		//	===================================================================================================================================================================================		#####
		class Texture2D : public Texture
		{
		protected:
			int width;

			int height;

			const unsigned char *data;

			const GLubyte* LoadImageFile(const std::string& sourceFile, int &width, int &height, int &components, GLenum &format);

		public:
			Texture2D();
			~Texture2D();

			void Generate(std::string filepath, bool autoMipmap = false);
			void Generate(unsigned char* imgData, int imgWidth, int imgHeight, int imgComponents, GLenum imgFormat, bool autoMipmap = false);

			int getWidth()
			{
				return width;
			}

			int getHeight()
			{
				return height;
			}

			unsigned int getTextureID()
			{
				return textureID;
			}

			friend class Texture2DStorage;
		};




		// Texture2DRect																																									Texture2DRect
		//	===================================================================================================================================================================================		#####
		class Texture2DRect : public Texture2D
		{
		public:
			Texture2DRect();
			~Texture2DRect();

			void Generate(std::string filepath, bool autoMipmap = false);
			void Generate(unsigned char* imgData, int imgWidth, int imgHeight, int imgComponents, GLenum imgFormat, bool autoMipmap = false);
		};
		//	===================================================================================================================================================================================		#####




		// TextureStorage																																								   TextureStorage
		//	===================================================================================================================================================================================		#####
		class Texture2DStorage
		{
		public:
			static void Store(std::string path, GLubyte *data, int width, int height, int imgDepth);
			static void StoreGreyscale(std::string path, GLubyte *data, int width, int height, int imgDepth);
		};
		//	===================================================================================================================================================================================		#####
	}
}


