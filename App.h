#pragma once

#include <blib/KeyListener.h>

namespace blib
{
	class Window;
	class MouseListener;

	class App
	{
	protected:
		Window* window;
		double time;

		struct AppSetup
		{
			int width, height;
			bool border;
		} appSetup;


		struct KeyState
		{
			bool pressedKeys[255];
			inline bool isPressed(int index)
			{
				return pressedKeys[index];
			}
		} keyState;
		struct MouseState
		{
			union
			{
				bool buttons[3];
				struct 
				{
					bool leftButton;
					bool middleButton;
					bool rightButton;
				};
			};
			int x;
			int y;
		} mouseState;

		bool running;


	public:
		void run();

		virtual void init() = 0;
		virtual void update(double elapsedTime) = 0;
		virtual void draw() = 0;
	protected:
		void addListener(KeyListener* keyListener);
		void addListener(MouseListener* mouseListener);

	};


}