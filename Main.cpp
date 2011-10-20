/*
Luka Horvat, redreaper132@gmail.com
30.3.2011
*/

#include "Luka.h"
#include <iostream>

int main ( int argc, char* args[] )
{
	//test trt
	//Initilize the framework, false -> windowed
	frm::initialize("Framework", 640, 480, false );

	//Create image and load file into it
	frm::Image img;
	img.loadImage("pad.png");

	//Create rects, first one has image binded to it
	frm::Rect A(50,50,50,50,0,&img); //Rect, with binded image to it
	frm::Rect B(400,50,150,120,0);

	//Create 2 rotRect with rects as base, after the construction there is no connection with Rect and RotRect
	frm::RotRect G(&A); //Rectangles, that are point based
	frm::RotRect H(&B);

	//Load font
	FTGLTextureFont font("FreeMono.ttf"); //Load font

	//Set starting FPS, if you want to use FPS limiting
	frm::setFPS();

	//Game loop
	bool play = true;

	//For FPS display
	int frames = 0;
	int time = SDL_GetTicks();
	int fps = SDL_GetTicks();

	//Main game loop
	while ( play )
	{
		//Event polling
		while ( SDL_PollEvent(&frm::event) ) //events like in SDL
		{
			if ( frm::event.type == SDL_QUIT )
			{
				play = false;
			}

			//Basic event usage for keyboard event
			if ( frm::event.type == SDL_KEYUP && frm::event.key.keysym.sym == SDLK_ESCAPE )
			{
				play = false;
			}

			//play = false;
		}

		//Rotate G, first we increase/decrease the angle, then we feed the angle to own function
		G.angle += 30*frm::getFPS();
		G.rotate(G.angle);	//angle is normalized
		G.move(60*frm::getFPS(),30*frm::getFPS()); //Move G on x and y axis

		//Rotation and movement of H RotRect
		H.angle -= 180*frm::getFPS();
		H.rotate(H.angle);
		H.move(-20*frm::getFPS(),40*frm::getFPS());

		//Move and rotate A
		A.x += 30*frm::getFPS();	//Move on X axis
		A.y += 30*frm::getFPS();	//Move on Y axis
		A.rotate(30*frm::getFPS()); //Rotate for 30°

		//Default clear color 
		glClearColor(0,0,0,1);

		if ( checkCollisionRotated(&G,&H) ) //Check collision with SAT
		{
			//If there is a collision we change the background color to blue
			glClearColor(0,0,1,1);
		}

		//Calculate framerate
		frames++;
		std::stringstream a;

		a.clear();
		a << "FPS: " << frames / ((SDL_GetTicks()-fps)/1000.f);

		time = SDL_GetTicks();

		//After LOGIC part of the main loop we reset the FPS 
		frm::setFPS();

		//RENDERING PART
		frm::startRendering();

		frm::scaleScreen(0.8,1); //Scale the screen from the middle (1)

		//Create a color for later use
		frm::Color col(255,0,0);

		//Render FPS, and 2 addtional sentances
		frm::renderText(a.str(), &font, 25, 10, 10, frm::Color(0,255,0));
		frm::renderText("Luka's framework", &font, 25, 10, 50, frm::Color(255,255,0));
		frm::renderText("This is a FTGL test", &font, 25, 10, 100, frm::Color(0,255,255));

		//Render the Rects different ways
		frm::renderRect(&A,&col);
		frm::renderRect(frm::Rect(100,400,10,50),frm::Color(0,255,100)); //Static Rect

		//We can also render a RotRect with renderRect function, by getting the Rect out of RotRect
		frm::renderRect(H.getRect(),&col);

		//Render point
		frm::renderPoint(frm::Point(300,10), col, 5);

		//Render RotRect G, then render the border of RotRect H 
		frm::renderRotRect(&G,&col);
		frm::Color green(0,255,0);
		frm::renderBorderRotRect(&H,&green, 2);

		//Stop rendering
		frm::stopRendering();

		//Limit framerate to 60, somewhere
		//frm::delayFPS();

	}

	//Quit SQL
	SDL_Quit();

	return 0;
}
