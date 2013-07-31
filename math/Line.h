#pragma once
#include <glm/glm.hpp>
#include <blib/gl/LineBatch.h>

namespace blib
{
	namespace math
	{

		class Line : public blib::gl::ILineDrawable
		{
		public:
			glm::vec2 p1;
			glm::vec2 p2;

			Line();
			Line(glm::vec2 p1, glm::vec2 p2);


			bool intersects(const Line &other) const;
			bool intersects(const Line &other, glm::vec2 &point) const;
			glm::vec2 project(const glm::vec2& point) const;

			glm::vec2 normal();
			void buildLines();

		};

	}
}