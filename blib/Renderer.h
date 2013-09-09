#pragma once

#include <blib/Shader.h>
#include <blib/RenderState.h>
#include <blib/VBO.h>
#include <vector>

namespace blib
{


	class Renderer
	{
	protected:
		class Render
		{
		public:
			enum Command {
				Clear,
				DrawTriangles,
				SetVbo,
				DrawLines,
			} command;
			virtual ~Render() {};
			RenderState renderState;
			Shader::State state;

			virtual void setVertexAttributes(float* firstVertex) = 0;
			virtual int vertexCount() = 0;
			virtual void perform(float* firstVertex) {};
		};

		template <class T>
		class RenderBlock : public Render
		{
		public:
			int vertexStart;
			int count;
			virtual void setVertexAttributes(float* firstVertex)
			{
				if(renderState.activeVbo != NULL)
					T::setAttribPointers();
				else
					T::setAttribPointers(firstVertex + vertexStart);
			}
			virtual int vertexCount()
			{
				return count;
			}
			virtual ~RenderBlock() { };
		};


		class RenderClear : public Render
		{
		public:
			glm::vec4 color;
			bool clearColor;
			bool clearDepth;
			bool clearStencil;

			virtual void setVertexAttributes(float* firstVertex)
			{
			}

			virtual int vertexCount()
			{
				return 0;
			}
		};

		template <class T>
		class RenderSetVbo : public Render
		{
		public:
			VBO_<T>* vbo;
			int vertexStart;
			int count;

			virtual void setVertexAttributes(float* firstVertex)
			{
			}
			virtual int vertexCount()
			{
			//	return vertices.size();
				return 0;
			}
			virtual ~RenderSetVbo() { };
			virtual void perform(float* firstVertex)
			{
				vbo->setData(count, (T*)(firstVertex + vertexStart));
			}
		};



		int activeLayer;
		std::vector<Render*> toRender[2];
		float* vertices[2];
		int vertexIndex[2];

	public:
		enum ClearOptions
		{
			Color = 1,
			Depth = 2,
			Stencil = 4,
		};


		Renderer()
		{
			activeLayer = 0;
			vertices[0] = new float[1024*1024*10]; // 10M floats
			vertices[1] = new float[1024*1024*10]; // 10M floats
			vertexIndex[0] = 0;
			vertexIndex[1] = 0;
		}

		template<class T>
		void drawTriangles(const RenderState& renderState, std::vector<T> &vertices)
		{
			RenderBlock<T>* block = new RenderBlock<T>();
			block->command = Render::DrawTriangles;	//TODO : move to constructor
			block->renderState = renderState;
			block->state = renderState.activeShader->state;
			block->vertexStart = vertexIndex[activeLayer];
			block->count = vertices.size();
			memcpy(this->vertices[activeLayer]+vertexIndex[activeLayer], &vertices[0], sizeof(T) * vertices.size());
			vertexIndex[activeLayer] += (sizeof(T) / sizeof(float)) * vertices.size();
			toRender[activeLayer].push_back(block);
		}

		template<class T>
		void drawTriangles(const RenderState& renderState, int count)
		{
			RenderBlock<T>* block = new RenderBlock<T>();
			block->command = Render::DrawTriangles;	//TODO : move to constructor
			block->count = count;
			block->renderState = renderState;
			block->state = renderState.activeShader->state;
			toRender[activeLayer].push_back(block);
		}
		template<class T>
		void drawTriangles(const RenderState& renderState, T* first, int count)
		{
			RenderBlock<T>* block = new RenderBlock<T>();
			block->command = Render::DrawTriangles;	//TODO : move to constructor
			block->renderState = renderState;
			block->state = renderState.activeShader->state;
			block->vertexStart = vertexIndex[activeLayer];
			block->count = count;
			memcpy(this->vertices[activeLayer]+vertexIndex[activeLayer], first, sizeof(T) * count);
			vertexIndex[activeLayer] += (sizeof(T) / sizeof(float)) * count;
			toRender[activeLayer].push_back(block);
		}
		void clear(const RenderState& renderState, const glm::vec4 &color, int bits)
		{
			RenderClear* block = new RenderClear();
			block->command = Render::Clear;
			block->color = color;
			block->renderState = renderState;
			block->clearColor = (bits & Color) != 0;
			block->clearDepth = (bits & Depth) != 0;
			block->clearStencil = (bits & Stencil) != 0;
			toRender[activeLayer].push_back(block);
		}

		template<class T>
		void setVbo(VBO_<T>* vbo, const std::vector<T> &vertices)
		{
			RenderSetVbo<T>* block = new RenderSetVbo<T>();
			block->command = Render::SetVbo;	//TODO : move to constructor
			block->vertexStart = vertexIndex[activeLayer];
			block->count = vertices.size();
			memcpy(this->vertices[activeLayer]+vertexIndex[activeLayer], &vertices[0], sizeof(T) * vertices.size());
			vertexIndex[activeLayer] += (sizeof(T) / sizeof(float)) * vertices.size();
			block->vbo = vbo;
			toRender[activeLayer].push_back(block);
		}

		template<class T>
		void drawLines(const RenderState& renderState, T* first, int count)
		{
			RenderBlock<T>* block = new RenderBlock<T>();
			block->command = Render::DrawLines;	//TODO : move to constructor
			block->renderState = renderState;
			block->state = renderState.activeShader->state;
			block->vertexStart = vertexIndex[activeLayer];
			block->count = count;
			memcpy(this->vertices[activeLayer]+vertexIndex[activeLayer], first, sizeof(T) * count);
			vertexIndex[activeLayer] += (sizeof(T) / sizeof(float)) * count;
			toRender[activeLayer].push_back(block);
		}


		virtual void flush() = 0;
		void swap()
		{
			activeLayer = 1 - activeLayer;
			vertexIndex[activeLayer] = 0;
		}



	};
}