#define SFML_STATIC

#include <SFML\System.hpp>
#include <SFML\Window.hpp>
#include <SFML\OpenGL.hpp>
#include <SFML\Graphics.hpp>

#include "physics.h"

using namespace sf;
using namespace std;

// SYSTEM OBJECTS
Clock clk;
VideoMode vmd;
ContextSettings cst;

// PROGRAM VARS
double time_elapsed = 0;
double time_elapsed_prev = 0;
double time_delta = 0;
double time_delta_draw = 0;
double fps = 0;

int d_width;
int d_height;

// RESOURCES
RectangleShape rect_info;

// FUNCTIONS
void draw(RenderWindow window);



int main()
{
	Physics::Init();
	rand();
	//for(int i=0; i<100; ++i) Physics::AddObj( rand()%1920, rand()%1080, rand()%20, rand()%50, rand()%500, rand()%500 );
	for(int i=0; i<200; ++i)Physics::AddObj( 960, 25, 1, 10, 250-(rand()%500), rand()%500 );


	/* Billard
	Physics::AddObj( 400, 500, 2, 30, 0, 0 );

	Physics::AddObj( 1000, 500, 2, 30, 0, 0 );

	Physics::AddObj( 1052, 469, 2, 30, 0, 0 );
	Physics::AddObj( 1052, 531, 2, 30, 0, 0 );

	Physics::AddObj( 1104, 439, 2, 30, 0, 0 );
	Physics::AddObj( 1104, 500, 2, 30, 0, 0 );
	Physics::AddObj( 1104, 561, 2, 30, 0, 0 );

	Physics::AddObj( 1156, 408, 2, 30, 0, 0 );
	Physics::AddObj( 1156, 469, 2, 30, 0, 0 );
	Physics::AddObj( 1156, 531, 2, 30, 0, 0 );
	Physics::AddObj( 1156, 592, 2, 30, 0, 0 );
	*/

    clk.restart();

    vmd = VideoMode::getDesktopMode();
    d_width = vmd.width;
    d_height = vmd.height;

    cst.depthBits = 0;
    cst.stencilBits = 0;
    cst.antialiasingLevel = 0;
    cst.majorVersion = 99;
    cst.minorVersion = 99;

	RenderWindow wnd;
	wnd.create( VideoMode(d_width, d_height), "Physics", Style::Fullscreen, cst );
	
    wnd.setVerticalSyncEnabled(false);
    cst = wnd.getSettings();

    // INIT RESOURCES

    RectangleShape rect_info( Vector2f( 270, 350 ) );
    rect_info.setFillColor( Color( 0, 0, 0, 160) );
    rect_info.setPosition( 10, 10 );

    Font Trebuc;
    if( !Trebuc.loadFromFile("resources/trebuc.ttf") ) return 1;
    Text info_text;
    info_text.setFont(Trebuc);
    info_text.setCharacterSize(16);
    info_text.setColor(Color::Yellow);
    info_text.setStyle(Text::Bold);
    info_text.setPosition( 20, 20 );
    wstring info_str;

	sf::Vector2i mouse = sf::Mouse::getPosition();


    //MAIN LOOP
    clk.restart();


    wnd.setActive();
	bool running = true;

	bool pressed = false;

    while( running )
    {
         // HANDLE EVENTS
		mouse = Mouse::getPosition();
        Event event;
        while (wnd.pollEvent(event))
        {
            if(event.type == Event::Closed || Keyboard::isKeyPressed(Keyboard::Escape) ) running = false;
            else if (event.type == Event::Resized)
            {
                d_width = event.size.width;
                d_height = event.size.height;
                glViewport( 0, 0, d_width, d_height );
            }

            if( Keyboard::isKeyPressed(Keyboard::W) ) Physics::AddObj( 960, 25, 1, 10, 50-(rand()%25), rand()%500 );  

			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					//Physics::GrabAt( );
					Physics::AddObj( 960, 25, 1, 5, 250-(rand()%500), rand()%500 ); 
				}
			}

			if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					Physics::Push( );
				}
			}
        }
        // END HANDLE EVENTS

        //DRAW INFO
		time_elapsed_prev = time_elapsed;
		time_elapsed = clk.getElapsedTime().asMicroseconds();
		time_delta = time_elapsed - time_elapsed_prev;
		time_delta_draw += time_delta; 
		fps = 1000000/time_delta;

        if(time_delta_draw > 500000)
        {
            info_str =  L"PERFORMANS BÝLGÝLERÝ";
            info_str += L"\n - Çözünürlük: "+to_wstring(d_width)+L" X "+to_wstring(d_height);
            info_str += L"\n - FPS: "+to_wstring( (int)fps );
			info_str += L"\n - dt: "+to_wstring( time_elapsed_prev );
			info_str += L"\n - dt: "+to_wstring( time_delta/1000000 );
            info_str += L"\n\nOPENGL BÝLGÝLERÝ";
            info_str += L"\n - OpenGL Versiyonu: "+to_wstring( cst.majorVersion )+L"."+to_wstring( cst.minorVersion );
            info_str += L"\n - Bits Per Pixel: "+to_wstring( cst.depthBits );
            info_str += L"\n - Stencil Bits: "+to_wstring( cst.stencilBits );
            info_str += L"\n - Anti-Aliasing: "+to_wstring( cst.antialiasingLevel );
            info_str += L"\n\nTEMEL BÝLGÝLER";
            info_str += L"\n - Adres Uzunluðu: 64-bit";
            info_str += L"\n - Kütüphane Eriþimi: Statik";
            info_str += L"\n - Thread Sayýsý: 1";
            info_text.setString(info_str);
			time_delta_draw = 0;
        }

		if(time_elapsed>1000000)Physics::Run( time_delta );

		wnd.clear( Color( 0, 0, 0, 0 ) );
        //wnd.clear( Color( 255, 255, 220, 50 ) );
        //DRAW

		Physics::Draw( &wnd );


        wnd.draw(rect_info);
        wnd.draw(info_text);

        //END DRAW
        wnd.display();
    }

    // END MAIN LOOP

    return 0;
}

void draw(RenderWindow window)
{
	window.clear();
}