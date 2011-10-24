#ifndef LUKA_H
#define LUKA_H

//Libraries
//Cross-compatable
#include <string>
#include <vector>
#include <cmath>
#include <sstream>

#define PI 3.141592654f
#define FONT 1

//Windows
#if defined(WIN32) || defined(_MSC_VER)
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#if FONT
#include <ftgl.h>
#endif

#else
//Unix
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>

#if FONT
#include <FTGL/ftgl.h>
#endif

#endif

//Framework
namespace frm
{
	//Structures/classees
	struct Rect;
	struct Point;
	struct RotRect;
	struct Image;

	//Color structure
	struct Color
	{
		float r,g,b,a;

		Color();
		Color(float r, float g, float b, float a = 255);

		void set(float r, float g, float b, float a = 255);
	};

	//Point structure
	struct Point
	{
		float x,y; //x and y

		Point();
		Point(float x, float y);

		void set(float x, float y);
	};

	//Rect structure
	struct Rect
	{
		float x,y,w,h,a; //x,y,width,height,angle
		Image * image; //Binded image to rect

		//Default constructor
		Rect();

		//Normal constructor
		Rect(float x, float y,float w, float h,float a = 0, Image* img = NULL);

		//Post constructor set
		void set(float x, float y, float w, float h, float a = 0, Image* img = NULL);

		void bindImage(Image* img);

		//Rotate
		void rotate(float angle);

		//Increase, decrease in px
		void increase(float px, float py);
		void decrease(float px, float py);

	};

	//Rotatable rectangle
	struct RotRect
	{
		//Edges, order: upper-left,upper-right,down-left,down-right
		std::vector<Point*> edge;
		float angle; //current angle
		Rect* rect; //A normal rectangle that is used to store non rotated coordinates of the rectangle for calculations

		//Default constructor
		RotRect();

		//Constructor from a Rectangle
		RotRect(Rect* rect);

		//Destructor
		~RotRect();

		//Move on x and y axis
		void move(float xVel, float yVel);

		//Rotate rectangle to a certain angle
		void rotate(float ang);

		//Set the rectangle to a other rectangle
		void setRect(Rect rec);

		//Get rect
		Rect* getRect();
	};

	//Image struct
	struct Image
	{
		//Basic atributes
		int width;
		int height;
		int frameWidth;
		int frameHeight;
		unsigned int texture;

		//Default constructor
		Image();

		//Constructor that loads an image
		Image(const std::string &file);

		//Load image
		void loadImage(const std::string &file);
	};

	//Event object
	extern SDL_Event event;

	//Global FPS timer
	extern float FPS_A;

	//Screen size
	extern int SCREEN_WIDTH;
	extern int SCREEN_HEIGHT;

	//initialize SDL, OpenGL, Fmod
	bool initialize(std::string caption, int width, int height, bool fullScreen);

	//Check collision between 2 Rect
	bool checkCollisionQuads(Rect* A, Rect* B);

	//Collision check for rotated rects
	bool checkCollisionRotated(RotRect* A, RotRect* B);

	//Set alpha timer - before main loop, after logic part
	void setFPS();

	//Get alpha timer
	float getFPS();

	//Delay function
	void delayFPS();

	//Calculate angle between 2 boxes depending on x axis
	float calculateAngle(Rect* A, Rect* B);

	//Calculate x and y speed for angle
	void calculateSpeed(float angle, float xSpeed, float* x, float* y);

	//Calculate length in px
	float calculateLength(Rect* A, Rect* B);

	//Load image into texture
	GLuint loadTexture(const std::string &filename);

	//Render rectangle
	void renderRect(const Rect* rect, const Color* color);
	void renderRect(Rect rect, Color color);

	//Render rectangle border
	void renderBorderRect(const Rect* rect, const Color* color, float lineWidth = 1);
	void renderBorderRect(Rect rect, Color color, float lineWidth = 1);

	//Render rotRect, and border
	void renderRotRect(const RotRect* rect, const Color* color);
	void renderBorderRotRect(const RotRect* rect, const Color* color, float lineWidth = 1);

	//Render point
	void renderPoint(const Point* point, const Color* color, float pointSize = 1);
	void renderPoint(Point point, Color color, float pointSize = 1);

	//Start/stop rendering
	void startRendering();
	void stopRendering();

	//Rotate/scale object
	void rotateScreen(float angle);
	void scaleScreen(float factor, int type = 0);

	void rotateRect(const Rect* rect, float angle);

	//Render sprite->frame in box
	void renderSprite(Rect* box, unsigned int texture, int texW, int spriteW, int frame);

#if FONT
	//Render text at x, y position
	void renderText(const std::string &text, FTGLTextureFont* font, float size, float x, float y, Color color);

#endif
}

#endif
