#include "list.h"

#include <blib/wm/wm.h>
#include <blib/Texture.h>
#include <blib/SpriteBatch.h>
#include <blib/math/Rectangle.h>
#include <blib/KeyListener.h>
#include <blib/Math.h>

//#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define NOMINMAX

#ifdef WIN32
#include <windows.h>
#endif

namespace blib
{
	namespace wm
	{
		namespace widgets
		{

			List::List( )
			{
				this->width = 100;
				this->height = 25;
				scrollPosition = 0;
				selectedItem = -1;
			}



			void List::draw( SpriteBatch &spriteBatch, glm::mat4 matrix)
			{
				Json::Value skin = WM::getInstance()->skin["list"];
				Texture* texture = WM::getInstance()->skinTexture;


				spriteBatch.drawStretchyRect(texture, glm::translate(matrix, glm::vec3(x,y,0)), skin, glm::vec2(width - skin["scroll"]["width"].asInt(), height));
				
				spriteBatch.draw(texture, glm::translate(matrix, glm::vec3(x + width - skin["scroll"]["width"].asInt(), y, 0)),
					glm::vec2(0, 0),
					blib::math::Rectangle(
					glm::vec2(skin["scroll"]["buttonup"]["left"].asInt() / (float)texture->originalWidth, skin["scroll"]["buttonup"]["top"].asInt() / (float)texture->originalWidth),
					skin["scroll"]["width"].asInt() / (float)texture->originalWidth, skin["scroll"]["buttonup"]["height"].asInt() / (float)texture->originalWidth));

				spriteBatch.draw(texture, glm::translate(matrix, glm::vec3(x + width - skin["scroll"]["width"].asInt(), y + height - skin["scroll"]["buttondown"]["height"].asInt(), 0)),
					glm::vec2(0, 0),
					blib::math::Rectangle(
					glm::vec2(skin["scroll"]["buttondown"]["left"].asInt() / (float)texture->originalWidth, skin["scroll"]["buttondown"]["top"].asInt() / (float)texture->originalWidth),
					skin["scroll"]["width"].asInt() / (float)texture->originalWidth, skin["scroll"]["buttondown"]["height"].asInt() / (float)texture->originalWidth));

				spriteBatch.draw(texture, glm::scale(glm::translate(matrix, glm::vec3(x + width - skin["scroll"]["width"].asInt(), y + skin["scroll"]["buttonup"]["height"].asInt(), 0)), glm::vec3(1,(height-skin["scroll"]["buttonup"]["height"].asInt() - skin["scroll"]["buttondown"]["height"].asInt()) / (float)skin["scroll"]["background"]["height"].asInt(),1)),
					glm::vec2(0, 0),
					blib::math::Rectangle(
					glm::vec2(skin["scroll"]["background"]["left"].asInt() / (float)texture->originalWidth, skin["scroll"]["background"]["top"].asInt() / (float)texture->originalWidth),
					skin["scroll"]["width"].asInt() / (float)texture->originalWidth, skin["scroll"]["background"]["height"].asInt() / (float)texture->originalWidth));


				/*
				glBindTexture(GL_TEXTURE_2D, WM::getInstance()->skinTexture->texid);
	


				GlHelper::drawStretchyRect(x, y, width - skin["scroll"]["width"].asInt(), height, skin);

				int textureWidth = WM::getInstance()->skinTexture->width;
				int textureHeight = WM::getInstance()->skinTexture->width;

				float tx = 1.0f/textureWidth;
				float ty = 1.0f/textureHeight;

				glBegin(GL_QUADS);
				glVertexAttrib3f(1, 1, 1, 1);	


				//button top
				GlHelper::drawRect(
					(skin["scroll"]["buttonup"]["left"].asInt()) * tx,
					(skin["scroll"]["buttonup"]["top"].asInt()) * ty,
					x+width-skin["scroll"]["width"].asInt(),
					y,
					(skin["scroll"]["buttonup"]["left"].asInt() + skin["scroll"]["width"].asInt()) * tx,
					(skin["scroll"]["buttonup"]["top"].asInt() + skin["scroll"]["buttonup"]["height"].asInt()) * ty,
					x + width,
					y + skin["scroll"]["buttonup"]["height"].asInt()
					);

				//background
				GlHelper::drawRect(
					(skin["scroll"]["background"]["left"].asInt()) * tx,
					(skin["scroll"]["background"]["top"].asInt()) * ty,
					x+width-skin["scroll"]["width"].asInt(),
					y + skin["scroll"]["buttonup"]["height"].asInt(),
					(skin["scroll"]["background"]["left"].asInt() + skin["scroll"]["width"].asInt()) * tx,
					(skin["scroll"]["background"]["top"].asInt() + skin["scroll"]["background"]["height"].asInt()) * ty,
					x + width,
					y + height - skin["scroll"]["buttondown"]["height"].asInt()
					);
				//button bottom
				GlHelper::drawRect(
					(skin["scroll"]["buttondown"]["left"].asInt()) * tx,
					(skin["scroll"]["buttondown"]["top"].asInt()) * ty,
					x+width-skin["scroll"]["width"].asInt(),
					y + height - skin["scroll"]["buttondown"]["height"].asInt(),
					(skin["scroll"]["buttondown"]["left"].asInt() + skin["scroll"]["width"].asInt()) * tx,
					(skin["scroll"]["buttondown"]["top"].asInt() + skin["scroll"]["buttondown"]["height"].asInt()) * ty,
					x + width,
					y + height
					);
				glEnd();


				glScissor((int)shader->matrix[3][0],shader->height-(int)shader->matrix[3][1]-height,width,height);
				glEnable(GL_SCISSOR_TEST);

*/
				if (selectedItem >= 0 && selectedItem < items.size())
					spriteBatch.drawStretchyRect(texture, glm::translate(matrix, glm::vec3(x + 2, y + 4 + 12 * selectedItem - scrollPosition, 0)), skin, glm::vec2(width - 4 - skin["scroll"]["width"].asInt(), 12), WM::getInstance()->convertHexColor4(skin["selectcolor"].asString()));

				for (int i = scrollPosition / 12; i < glm::min((int)items.size(), scrollPosition / 12 + (int)ceil(height / 12.0)); i++)
				{
					spriteBatch.draw(WM::getInstance()->font, items[i], 
						blib::math::easyMatrix(glm::vec2(x + 2, y + 12 * i - scrollPosition), matrix), 
						WM::getInstance()->convertHexColor4(i == selectedItem ? skin["selectfontcolor"].asString() : skin["fontcolor"].asString()));
				}
			}

			void List::mousewheel( int direction, int x, int y )
			{
				scrollPosition -= direction / abs(direction)*height/2;
				if(scrollPosition < 0)
					scrollPosition = 0;
				if(scrollPosition > 12*(int)items.size()-12)
					scrollPosition = 12*items.size()-12;
			}


			void List::mouseclick(int x, int y, int clickcount)
			{
				Widget::mouseclick(x, y, clickcount);
				Json::Value skin = WM::getInstance()->skin["list"];
				if(x-this->x < width - skin["scroll"]["width"].asInt())
				{
					selectedItem = ((y-this->y+scrollPosition)/12);
				}
			}

			void List::keyboardSpecial( int key )
			{
				Widget::keyboardSpecial(key);
				if(key == blib::KEY_DOWN)
					selectedItem = glm::min((int)items.size()-1, selectedItem+1);
				if(key == blib::KEY_UP)
					selectedItem = glm::max(0, selectedItem-1);

			}

		}
	}
}
