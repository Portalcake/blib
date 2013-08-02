#pragma once

#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <gl/glew.h>
#endif

#include <blib/gl/GlInitRegister.h>
#include <blib/gl/Texture.h>

namespace blib
{
	namespace gl
	{
		class FBO : public GlInitRegister, public Texture
		{
			bool depth;
			bool stencil;

			GLuint fbo;
			GLuint depthBuffer;
			GLuint stencilBuffer;
		public:
			FBO(int width, int height, bool depth = false, bool stencil = true);
			~FBO();

			void initGl();

			void bind();
			void unbind();

			int getHeight();
			int getWidth();

		};

	}
}
