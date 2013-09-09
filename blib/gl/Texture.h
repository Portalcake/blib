#pragma once

#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>
#include <blib/Texture.h>

namespace blib
{

	namespace gl
	{
		class Texture : public blib::Texture
		{
		public: //TODO: make protected
			Texture();
			void fromData(unsigned char* data, int width, int height);
			void fromFile(std::string fileName, int loadOptions);

			Texture(std::string fileName, int loadOptions = 0);
			~Texture(void);
		public:
			enum LoadOptions
			{
				NoResize = 1,
				KeepData = 2,
				LoadLater = 4,
				TextureWrap = 8,
			};




			unsigned char* data;

			GLuint texid;
			Texture(unsigned char* data, int width, int height);
			
			void use();

		};
		

		class TextureMap : public blib::Texture
		{
		public:
			class TexInfo
			{
			public:
				TexInfo(TextureMap* texMap);
				TextureMap* texMap;
				glm::vec2 t1;
				glm::vec2 t2;


				int x,y,width,height;
			};
			GLuint texid;
			std::map<std::string, TexInfo*> info;


			bool* taken;
			inline bool &isTaken(int x, int y);

			void save(std::string filename);

			TextureMap();
			~TextureMap();
			TexInfo* addTexture(std::string filename);
			void use();
		};


		class MultiTextureMap : public blib::Texture
		{
		public:
			int maxCount;
			int count;
			int width, height;

			class TexInfo
			{
			public:
			//	TexInfo();
				MultiTextureMap* texMap;
				glm::vec2 t1;
				glm::vec2 t2;
				int x,y,width,height;

				int depth;
				std::string filename;

				unsigned char* data;
			};

			void use();
			GLuint texid;
			std::map<std::string, TexInfo*> info;

			MultiTextureMap(int width, int height);
			~MultiTextureMap();
			TexInfo* addTexture(std::string filename);


		};


	}
}

