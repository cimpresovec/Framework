#include "Luka.h"

#include <iostream>

namespace frm
{
	SDL_Event event;

	float FPS_A = 0;
	int SCREEN_WIDTH = 0;
	int SCREEN_HEIGHT = 0;

	Color::Color()
	{
		r = 255; g = 255; b = 255; a = 255;
	}

	Color::Color(float r, float g, float b, float a)
	{
		this->r = r; this->g = g; this->b = b; this->a = a;
	}

	void Color::set(float r, float g, float b, float a)
	{
		this->r = r; this->g = g; this->b = b; this->a = a;
	}

	Point::Point()
	{
		x = 0; y = 0;
	}

	Point::Point(float x, float y)
	{
		this->x = x; this->y = y;
	}

	void Point::set(float x, float y)
	{
		this->x = x; this->y = y;
	}

	Rect::Rect()
	{
		x = 0; y = 0; w = 1; h = 1; a = 0; image = NULL;
	}

	Rect::Rect(float x, float y,float w, float h,float a, Image* img)
	{
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
		this->a = a;
		image = img;
	}

	void Rect::set(float x, float y, float w, float h, float a, Image* img)
	{
		this->x = x; this->y = y; this->w = w; this->h = h; this->a = a; image = img;
	}

	void Rect::bindImage(Image* img)
	{
		image = img;
	}

	void Rect::rotate(float angle)
	{
		a += angle;
		a = fmod(a, 360);
	}

	void Rect::increase(float px, float py)
	{
		x -= px/2; w += px/2;
		y -= px/2; h += px/2;
	}

	void Rect::decrease(float px, float py)
	{
		x += px/2; w -= px/2;
		y += px/2; h -= px/2;
	}

	RotRect::RotRect()
	{
		edge.push_back(new Point());
		edge.push_back(new Point());
		edge.push_back(new Point());
		edge.push_back(new Point());
		rect = new Rect();
		angle = 0;
	}

	RotRect::RotRect(Rect* rect)
	{
		edge.push_back(new Point(rect->x,rect->y));
		edge.push_back(new Point(rect->x+rect->w,rect->y));
		edge.push_back(new Point(rect->x,rect->y+rect->h));
		edge.push_back(new Point(rect->x+rect->w,rect->y+rect->h));
		this->rect = new Rect(); *this->rect = *rect;
		angle = 0;
	}

	RotRect::~RotRect()
	{
		for ( int n = 0; n < (signed)edge.size(); n++ )
		{
			delete edge[n];
		}

		edge.clear();

		delete rect;
	}

	void RotRect::move(float xVel, float yVel)
	{
		for ( int n = 0; n < edge.size(); n++ )
		{
			edge[n]->x += xVel;
			edge[n]->y += yVel;
		}

		rect->x += xVel;
		rect->y += yVel;
	}

	void RotRect::rotate(float ang)
	{
		angle = ang;//Set current angle

		angle = fmod(angle,360); //Normalize

		float x = rect->x, y = rect->y; //Store current x and y position

		//Move normal rectangle so it's centered with the matrix origin
		rect->x -= rect->x + rect->w/2;
		rect->y -= rect->y + rect->h/2;

		//Rotate the rectangle
		edge[0]->x = rect->x * cos(-angle * PI/180) + rect->y * sin(-angle * PI/180);
		edge[0]->y = -(rect->x) * sin(-angle * PI/180) + rect->y * cos(-angle * PI/180);

		edge[1]->x = (rect->x + rect->w) * cos(-angle * PI/180) + rect->y * sin(-angle * PI/180);
		edge[1]->y = -(rect->x + rect->w) * sin(-angle * PI/180) + rect->y * cos(-angle * PI/180);

		edge[2]->x = rect->x * cos(-angle * PI/180) + (rect->y + rect->h)  * sin(-angle * PI/180);
		edge[2]->y = -(rect->x) * sin(-angle * PI/180) + (rect->y + rect->h) * cos(-angle * PI/180);

		edge[3]->x = (rect->x + rect->w) * cos(-angle * PI/180) + (rect->y + rect->h) * sin(-angle * PI/180);
		edge[3]->y = -(rect->x + rect->w) * sin(-angle * PI/180) + (rect->y + rect->h) * cos(-angle *PI/180);

		//Move rotated rectangle back to initial position
		for ( int n = 0; n < 4; n++ )
		{
			edge[n]->x += x + rect->w/2;
			edge[n]->y += y + rect->h/2;	
		}

		//Set normal's rectangle x and y position back
		rect->x = x;
		rect->y = y;
		rect->a = angle;
	}

	void RotRect::setRect(Rect rec)
	{
		*rect = rec;
		angle = rec.a;
		rotate(rec.a);
	}

	Rect* RotRect::getRect()
	{
		return rect;
	}

	Image::Image()
	{
		width = 0;
		height = 0;
		frameWidth = 0;
		frameHeight = 0;
		texture = 0;
	}

	Image::Image(const std::string &file)
	{
		//Load surface
		SDL_Surface *image = IMG_Load( file.c_str() );

		if ( image ){

			//Set display format
			SDL_DisplayFormatAlpha(image);

			//Create texture
			unsigned object(0);

			glGenTextures(1, &object);

			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, object);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

			glDisable(GL_TEXTURE_2D);

			width = image->w;
			height = image->h;
			frameWidth = width;
			frameHeight = height;

			//Free surface
			SDL_FreeSurface(image);

			texture = object;
		}
		else
		{
			std::cout << "Problem reading image " << file.c_str() << "!\n";
			texture = 0;
		}
	}

	void Image::loadImage(const std::string &file)
	{
		SDL_Surface *image = IMG_Load( file.c_str() );

		if ( image ){

			//Set display format
			SDL_DisplayFormatAlpha(image);

			//Create texture
			unsigned object(0);

			glGenTextures(1, &object);

			glEnable(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, object);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

			glDisable(GL_TEXTURE_2D);

			width = image->w;
			height = image->h;
			frameWidth = width;
			frameHeight = height;

			//Free surface
			SDL_FreeSurface(image);

			texture = object;
		}
		else
		{
			std::cout << "Problem reading image " << file.c_str() << "!\n";
			texture = 0;
		}
	}

	bool initialize(std::string caption, int width, int height, bool fullScreen )
	{

		//initialize SDL
		if ( SDL_Init(SDL_INIT_EVERYTHING) )
		{
			std::cout << "SDL Initialization failed!\n" << SDL_GetError() << std::endl;
			return false;
		}

		//Set memory usage
		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute( SDL_GL_BUFFER_SIZE, 32);
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

		//Set caption
		SDL_WM_SetCaption( caption.c_str() , NULL );

		//Video mode
		if ( fullScreen ){
			SDL_SetVideoMode( width, height, 32, SDL_OPENGL | SDL_FULLSCREEN ); 
		}
		else{
			SDL_SetVideoMode( width, height, 32, SDL_OPENGL );
		}

		//Clear color : black
		glClearColor(0,0,0,1);

		//Shader model
		glShadeModel( GL_SMOOTH );

		//Viewport
		glViewport( 0, 0, width, height );

		//Matrix modes
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();

		//3D
		/*
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		*/

		//glEnable( GL_LINE_SMOOTH );

		//Blending
		glEnable ( GL_BLEND );
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
		glEnable(GL_LINE_SMOOTH);

		//GL Enable-Disable
		//glEnable (GL_DEPTH_TEST );
		glDisable (GL_DEPTH_TEST);
		//glEnable (GL_LIGHTING);
		glDisable (GL_LIGHTING);

		SCREEN_WIDTH = width;
		SCREEN_HEIGHT = height;

		return true;
	}

	float getFPS()
	{
		return ( ( SDL_GetTicks() - FPS_A ) / 1000.f );
	}

	void setFPS()
	{
		FPS_A = SDL_GetTicks();
	}

	void delayFPS()
	{
		if ( (SDL_GetTicks() - FPS_A) < 1000.00/60.00 )
		{
			SDL_Delay( ( 1000.f/60.f ) - (SDL_GetTicks() - FPS_A ) );
		}
	}

	float calculateAngle( Rect* A, Rect* B )
	{
		//Centers of boxes
		float Ax,Ay,Bx,By;

		//Sides for calculations and angle
		double sideA, sideB, angle = 1;

		//Calculate centers
		Ax = A->x + A->w/2;
		Ay = A->y + A->h/2;
		Bx = B->x + B->w/2;
		By = B->y + B->h/2;

		//Check if boxes align on x or y axis
		if ( By == Ay && Bx > Ax ) angle = 0;
		else if ( By == Ay && Bx < Ax ) angle = 180;
		else if ( Bx == Ax && By > Ay ) angle = 90;
		else if ( Bx == Ax && By < Ay ) angle = 270;

		//If boxes are not alignes on x or y axis then calculate angle
		if ( !(angle == 1) ) return angle;

		//Calculate sides
		sideA = Bx - Ax;
		sideB = By - Ay;

		//Check negatives
		if ( sideA < 0 ) { sideA = -sideA; }
		if ( sideB < 0 ) { sideB = -sideB; }

		//Calculate angle in degrees
		angle = atan2(sideB,sideA) * 180/PI;

		//Add angle based on box positions
		if ( By > Ay && Bx < Ax ) angle = 180 - angle;
		else if ( By < Ay && Bx < Ax ) angle += 180;
		else if ( By < Ay && Bx > Ax ) angle = 360 - angle;

		return angle;
	}

	void calculateSpeed( float angle, float xSpeed, float* x, float* y )
	{
		//calculate x and y speed and multiply with xSpeed
		*y = (float)(sin((angle * (PI/180.f))) * xSpeed);
		*x = (float)(cos((angle * (PI/180.f))) * xSpeed);
	}

	float calculateLength(Rect* A, Rect* B)
	{
		//Center of boxes;
		float Ax,Ay,Bx,By;

		Ax = A->x + A->w/2;
		Ay = A->y + A->h/2;
		Bx = B->x + B->w/2;
		By = B->y + B->h/2;

		return sqrt( ((Ax-Bx) * (Ax-Bx)) + ((Ay-By) * (Ay-By)));
	}

	bool checkCollisionQuads( Rect* A, Rect* B )
	{
		if ( A->x + A->w <= B->x ) return false;
		if ( A->y + A->h <= B->y ) return false;
		if ( B->x + B->w <= A->x ) return false;
		if ( B->y + B->h <= A->y ) return false;

		return true;
	}

	bool checkCollisionRotated(RotRect* A, RotRect* B)
	{
		//Create axis
		Point axis[4];

		axis[0].x = A->edge[1]->x - A->edge[0]->x;
		axis[0].y = A->edge[1]->y - A->edge[0]->y;

		axis[1].x = A->edge[1]->x - A->edge[3]->x;
		axis[1].y = A->edge[1]->y - A->edge[3]->y;

		axis[2].x = B->edge[0]->x - B->edge[2]->x;
		axis[2].y = B->edge[0]->y - B->edge[2]->y;

		axis[3].x = B->edge[0]->x - B->edge[1]->x;
		axis[3].y = B->edge[0]->y - B->edge[1]->y;

		bool collision = true;

		//Copies
		Point a[4], b[4];

		for ( int n = 0; n < 4; n++ )
		{
			a[n].x = A->edge[n]->x;
			a[n].y = A->edge[n]->y;

			b[n].x = B->edge[n]->x;
			b[n].y = B->edge[n]->y;
		}

		//Collision checking
		for ( int n = 0; n < 4; n++ )
		{
			//Transform point to axis
			for ( int g = 0; g < 4; g++ )
			{
				A->edge[g]->x = ( (a[g].x * axis[n].x + a[g].y * axis[n].y) / ( (axis[n].x * axis[n].x) + (axis[n].y * axis[n].y) ) ) * axis[n].x;	
				A->edge[g]->y = ( (a[g].x * axis[n].x + a[g].y * axis[n].y) / ( (axis[n].x * axis[n].x) + (axis[n].y * axis[n].y) ) ) * axis[n].y;

				B->edge[g]->x = ( (b[g].x * axis[n].x + b[g].y * axis[n].y) / ( (axis[n].x * axis[n].x) + (axis[n].y * axis[n].y) ) ) * axis[n].x;	
				B->edge[g]->y = ( (b[g].x * axis[n].x + b[g].y * axis[n].y) / ( (axis[n].x * axis[n].x) + (axis[n].y * axis[n].y) ) ) * axis[n].y;
			}

			//Find min, max numbers
			float minA, minB, maxA, maxB;

			minA = A->edge[0]->x * axis[n].x + A->edge[0]->y * axis[n].y;
			maxA = minA;

			minB = B->edge[0]->x * axis[n].x + B->edge[0]->y * axis[n].y;
			maxB = minB;

			float cur;
			for ( int g = 0; g < 3; g++ )
			{
				cur = A->edge[g+1]->x * axis[n].x + A->edge[g+1]->y * axis[n].y;
				if ( cur > maxA )
					maxA = cur;
				if ( cur < minA )
					minA = cur;

				cur = B->edge[g+1]->x * axis[n].x + B->edge[g+1]->y * axis[n].y;
				if ( cur > maxB )
					maxB = cur;
				if ( cur < minB )
					minB = cur;
			}

			//Check for collision
			if ( !((minB <= maxA) && (maxB >= minA)) )
			{
				collision = false;
				break;
			}
		}

		//Return rectangles back to start
		for ( int n = 0; n < 4; n++ )
		{
			A->edge[n]->x = a[n].x;
			A->edge[n]->y = a[n].y;

			B->edge[n]->x = b[n].x; 
			B->edge[n]->y = b[n].y;
		}

		return collision;
	}

	GLuint loadTexture( const std::string &fileName )
	{
		//Load surface
		SDL_Surface *image = IMG_Load( fileName.c_str() );

		if ( !image ){
			std::cout << "Problem reading image " << fileName.c_str() << "!\n";
			return 0;
		}

		//Set display format
		SDL_DisplayFormatAlpha(image);

		//Create texture
		unsigned object(0);

		glGenTextures(1, &object);

		glBindTexture(GL_TEXTURE_2D, object);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);

		//Free surface
		SDL_FreeSurface(image);

		return object;
	}

	void renderRect(const Rect* rect, const Color* color)
	{
		glColor4ub(color->r,color->g,color->b,color->a);

		if ( rect->a != 0 )
		{
			glPushMatrix();
			rotateRect(rect,rect->a);
		}

		if ( rect->image != NULL )
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, rect->image->texture);

			glBegin(GL_QUADS);
			glTexCoord2d(0,0); glVertex2f(rect->x,rect->y);
			glTexCoord2d(1,0); glVertex2f(rect->x+rect->w,rect->y);
			glTexCoord2d(1,1); glVertex2f(rect->x+rect->w,rect->y+rect->h);
			glTexCoord2d(0,1); glVertex2f(rect->x,rect->y+rect->h);
			glEnd();

			glDisable(GL_TEXTURE_2D);
			if ( rect->a != 0 )
			{
				glPopMatrix();
			}

			return;
		}

		glBegin(GL_QUADS);
		glVertex2f(rect->x,rect->y);
		glVertex2f(rect->x+rect->w,rect->y);
		glVertex2f(rect->x+rect->w,rect->y+rect->h);
		glVertex2f(rect->x,rect->y+rect->h);
		glEnd();

		if ( rect->a != 0 )
		{
			glPopMatrix();
		}
	} 

	void renderRect(Rect rect, Color color)
	{
		glColor4ub(color.r,color.g,color.b,color.a);

		if ( rect.a != 0 )
		{
			glPushMatrix();
			rotateRect(&rect,rect.a);
		}

		if ( rect.image != NULL )
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, rect.image->texture);

			glBegin(GL_QUADS);
			glTexCoord2d(0,0); glVertex2f(rect.x,rect.y);
			glTexCoord2d(1,0); glVertex2f(rect.x+rect.w,rect.y);
			glTexCoord2d(1,1); glVertex2f(rect.x+rect.w,rect.y+rect.h);
			glTexCoord2d(0,1); glVertex2f(rect.x,rect.y+rect.h);
			glEnd();

			glDisable(GL_TEXTURE_2D);

			if ( rect.a != 0 )
			{
				glPushMatrix();
				rotateRect(&rect,rect.a);
			}
			return;
		}

		glBegin(GL_QUADS);
		glVertex2f(rect.x,rect.y);
		glVertex2f(rect.x+rect.w,rect.y);
		glVertex2f(rect.x+rect.w,rect.y+rect.h);
		glVertex2f(rect.x,rect.y+rect.h);
		glEnd();

		if ( rect.a != 0 )
		{
			glPopMatrix();
		}
	}

	void renderBorderRect(const Rect* rect, const Color* color, float lineWidth)
	{
		glColor4ub(color->r,color->g,color->b,color->a);
		glLineWidth(lineWidth);

		if ( rect->a != 0 )
		{
			glPushMatrix();
			rotateRect(rect,rect->a);
		}

		glBegin(GL_LINE_LOOP);
		glVertex2f(rect->x,rect->y);
		glVertex2f(rect->x+rect->w,rect->y);
		glVertex2f(rect->x+rect->w,rect->y+rect->h);
		glVertex2f(rect->x,rect->y+rect->h);
		glEnd();

		if ( rect->a != 0 )
		{
			glPopMatrix();
		}
	}

	void renderBorderRect(Rect rect, Color color, float lineWidth)
	{
		glColor4ub(color.r,color.g,color.b,color.a);
		glLineWidth(lineWidth);

		if ( rect.a != 0 )
		{
			glPushMatrix();
			rotateRect(&rect,rect.a);
		}

		glBegin(GL_LINE_LOOP);
		glVertex2f(rect.x,rect.y);
		glVertex2f(rect.x+rect.w,rect.y);
		glVertex2f(rect.x+rect.w,rect.y+rect.h);
		glVertex2f(rect.x,rect.y+rect.h);
		glEnd();

		if ( rect.a != 0 )
		{
			glPopMatrix();
		}
	}

	void renderRotRect(const RotRect* rect, const Color* color)
	{
		glColor4ub(color->r,color->g,color->b,color->a);

		glBegin(GL_QUADS);
		glVertex2f(rect->edge[0]->x,rect->edge[0]->y);
		glVertex2f(rect->edge[1]->x,rect->edge[1]->y);
		glVertex2f(rect->edge[3]->x,rect->edge[3]->y);
		glVertex2f(rect->edge[2]->x,rect->edge[2]->y);
		glEnd();
	}

	void renderBorderRotRect(const RotRect* rect, const Color* color, float lineWidth)
	{
		glColor4ub(color->r,color->g,color->b,color->a);
		glLineWidth(lineWidth);

		glBegin(GL_LINE_LOOP);
		glVertex2f(rect->edge[0]->x,rect->edge[0]->y);
		glVertex2f(rect->edge[1]->x,rect->edge[1]->y);
		glVertex2f(rect->edge[3]->x,rect->edge[3]->y);
		glVertex2f(rect->edge[2]->x,rect->edge[2]->y);
		glEnd();
	}

	void renderPoint(const Point* point, const Color* color, float pointSize)
	{
		glColor4ub(color->r,color->g,color->b,color->a);
		glPointSize(pointSize);

		glBegin(GL_POINTS);
		glVertex2f(point->x,point->y);
		glEnd();
	}

	void renderPoint(Point point, Color color, float pointSize)
	{
		glColor4ub(color.r,color.g,color.b,color.a);
		glPointSize(pointSize);

		glBegin(GL_POINTS);
		glVertex2f(point.x, point.y);
		glEnd();
	}

	void startRendering()
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glPushMatrix();
		glOrtho(0,SCREEN_WIDTH,SCREEN_HEIGHT,0,-1,1);
	}

	void stopRendering()
	{
		glPopMatrix();
		SDL_GL_SwapBuffers();
	}

	void rotateScreen(float angle)
	{
		glTranslatef(SCREEN_WIDTH/2,SCREEN_HEIGHT/2,0);
		glRotatef(angle,0,0,1);
		glTranslatef(-SCREEN_WIDTH/2,-SCREEN_HEIGHT/2,0);
	}

	void scaleScreen(float factor, int type)
	{
		if ( type != 0 )
			glTranslatef(SCREEN_WIDTH/2,SCREEN_HEIGHT/2,0);
		glScalef(factor,factor,0);
		if ( type != 0 )
			glTranslatef(-SCREEN_WIDTH/2,-SCREEN_HEIGHT/2,0);
	}

	void rotateRect(const Rect* rect, float angle)
	{
		glTranslatef(rect->x+rect->w/2,rect->y+rect->h/2,0);
		glRotatef(angle,0,0,1);
		glTranslatef(-(rect->x+rect->w/2),-(rect->y+rect->h/2),0);
	}

	void renderSprite( Rect* box, unsigned int texture, int texW, int spriteW, int frame )
	{
		//background color
		glColor4ub(255,255,255,255);

		//calculate sprite x and windth
		float x = (float)(frame*spriteW) / (float)texW;
		float w = (float)((frame+1)*spriteW) / (float)texW;

		//enable texture
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);

		//render
		glBegin(GL_QUADS);
		glTexCoord2d(x,0);glVertex2f(box->x,box->y);
		glTexCoord2d(w,0);glVertex2f(box->x+box->w,box->y);
		glTexCoord2d(w,1);glVertex2f(box->x+box->w,box->y+box->h);
		glTexCoord2d(x,1);glVertex2f(box->x,box->y+box->h);
		glEnd();

		//disable texture
		glDisable(GL_TEXTURE_2D);
	}

#if FONT
	//Render text at x, y position with color
	void renderText(const std::string &text, FTGLTextureFont* font, float size, float x, float y, Color color)
	{
		if ( font->FaceSize() != size )
			font->FaceSize(size);

		glColor4ub(color.r, color.g, color.b, color.a);

		glPushMatrix();
		glRotatef(180,1,0,0);

		float height = font->BBox(text.c_str()).Upper().Y() - font->BBox(text.c_str()).Lower().Y();

		glTranslatef(x,-height-y,0);

		font->Render(text.c_str());

		glPopMatrix();
	}

#endif
}




