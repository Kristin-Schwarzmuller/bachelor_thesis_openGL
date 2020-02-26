// Includes																																														 Includes
//	===========================================================================================================================================================================================		#####
#include <Texture.h>
#include <iostream>
#include <fstream>
#include <FreeImage.h>
//	===========================================================================================================================================================================================		#####


namespace cgbv
{
	namespace textures
	{
        Texture::Texture(void)
        {
            glGenTextures(1, &textureID);
        }

        Texture::~Texture(void)
        {
            glDeleteTextures(1, &textureID);
        }



		Texture2D::Texture2D(void) : Texture()
		{
            data = nullptr;
            height = -1;
            width = -1;
		}


		Texture2D::~Texture2D(void)
		{

		}

		void Texture2D::Generate(std::string filepath, bool autoMipmap)
		{
			int components;
			GLenum format;

			data = LoadImageFile(filepath.c_str(), width, height, components, format);

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, components, width, height, 0, format, GL_UNSIGNED_BYTE, data);

			if(autoMipmap)
				glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void Texture2D::Generate(unsigned char* imgData, int imgWidth, int imgHeight, int imgComponents, GLenum imgFormat, bool autoMipmap)
		{
			data = imgData;
			width = imgWidth;
			height = imgHeight;
			
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, imgComponents, width, height, 0, imgFormat, GL_UNSIGNED_BYTE, data);
			
			if(autoMipmap)
				glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);
		}

        const GLubyte* Texture2D::LoadImageFile(const std::string& sourceFile, int &width, int &height, int &components, GLenum &format)
        {
            std::ifstream file(sourceFile, std::fstream::in);

            if(!file.is_open())
            {
                std::cout << "Datei " << sourceFile << " existiert nicht!" << std::endl;
                return nullptr;
            }

            file.close();

            GLubyte *image_bits = nullptr;

            FREE_IMAGE_FORMAT image_format = FIF_UNKNOWN;

            image_format = FreeImage_GetFileType(sourceFile.c_str(), 0);

            if(image_format == FIF_UNKNOWN)
                image_format = FreeImage_GetFIFFromFilename(sourceFile.c_str());

            if(image_format == FIF_UNKNOWN)
            {
                std::cout << "Fehler beim Lesen der Datei: " << sourceFile << std::endl << "Datei-Format unbekannt" << std::endl;
                return nullptr;
            }

            if(FreeImage_FIFSupportsReading(image_format))
            {
                FIBITMAP *bitmap = FreeImage_Load(image_format, sourceFile.c_str(), 0);

                int bpp = FreeImage_GetBPP(bitmap);
                int depth = bpp / 8;

                width = FreeImage_GetWidth(bitmap);

                height = FreeImage_GetHeight(bitmap);

                image_bits = FreeImage_GetBits(bitmap);

                switch(depth)
                {
                    case 4:
                        format = GL_BGRA;
                        components = GL_RGBA;
                        break;

                    case 3:
                        format = GL_BGR;
                        components = GL_RGB;
                        break;

                        //case 1:
                        //	format = GL_LUMINANCE;
                        //	components = GL_LUMINANCE;
                        //	break;
                    default:
                        format = GL_BGR;
                        components = GL_RGB;
                        break;
                }
            }

            return image_bits;
        }




		void Texture2DStorage::Store(std::string path, GLubyte *data, int width, int height, int imgDepth)
		{
			auto datatype = FreeImage_GetFIFFromFilename(path.c_str());
			FIBITMAP *image = FreeImage_ConvertFromRawBits(data, width, height, 3 * width, 24, 0x0000FF, 0xFF0000, 0x00FF00, false);
			if (!FreeImage_Save(datatype, image, path.c_str()))
				std::cout << "Writing Image " << path << " failed" << std::endl;

			FreeImage_Unload(image);
		}

        void Texture2DStorage::StoreGreyscale(std::string path, GLubyte* data, int width, int height, int imgDepth)
        {
            auto datatype = FreeImage_GetFIFFromFilename(path.c_str());
            FIBITMAP *image = FreeImage_ConvertFromRawBits(data, width, height, width, 8, 0x0000FF, 0xFF0000, 0x00FF00, false);
            if (!FreeImage_Save(datatype, image, path.c_str()))
                std::cout << "Writing Image " << path << " failed" << std::endl;

            FreeImage_Unload(image);
        }




        Texture2DRect::Texture2DRect() : Texture2D()
        {

        }

        Texture2DRect::~Texture2DRect()
        {
            glDeleteTextures(1, &textureID);
        }

        void Texture2DRect::Generate(std::string filepath, bool autoMipmap)
        {
            int components;
            GLenum format;

            data = LoadImageFile(filepath.c_str(), width, height, components, format);

            glBindTexture(GL_TEXTURE_RECTANGLE, textureID);
            glTexImage2D(GL_TEXTURE_RECTANGLE, 0, components, width, height, 0, format, GL_UNSIGNED_BYTE, data);

            glBindTexture(GL_TEXTURE_RECTANGLE, 0);
        }

        void Texture2DRect::Generate(unsigned char* imgData, int imgWidth, int imgHeight, int imgComponents, GLenum imgFormat, bool autoMipmap)
        {
            data = imgData;
            width = imgWidth;
            height = imgHeight;

            glBindTexture(GL_TEXTURE_RECTANGLE, textureID);
            glTexImage2D(GL_TEXTURE_RECTANGLE, 0, imgComponents, width, height, 0, imgFormat, GL_UNSIGNED_BYTE, data);


            glBindTexture(GL_TEXTURE_RECTANGLE, 0);
        }
	}
}