#include <SFML\System.hpp>
#include <SFML\Window.hpp>
#include <SFML\OpenGL.hpp>
#include <SFML\Graphics.hpp>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace sf;
using namespace std;

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

namespace Physics
{
	// Core
	int obj_count;

	bool is_grabbed;
	int grabbed;

	// For environment
	double gravity;
	double resistance_air;
	double resistance_wall;
	double richocet;
	double scale;
	double area_x;
	double area_y;

	// For objects
	vector<double> m;		// mass
	vector<double> r;		// radius
	vector<double> A;		// cross section area
	vector<double> s_x;		// position
	vector<double> s_y;		
	vector<double> v_x;		// speed
	vector<double> v_y;		
	vector<double> a_x;		// acceleration
	vector<double> a_y;
	vector<double> k_x;		// kinetic energy
	vector<double> k_y;		
	vector<double> p_x;		// momentum
	vector<double> p_y;		
	
	vector<double> rm;		// moment of inertia
	vector<double> rv;		// angular speed
	vector<double> ra;		// angular acceleration
	vector<double> rk;		// angular kinetic energy
	vector<double> rp;		// angular momentum

	vector<CircleShape> shp;

	// Functions
	void Init( );
	void AddObj( double, double, double, double, double, double, Texture* );
	void GrabAt( );
	void Push( );
	void Run( double );
	void Draw( RenderWindow* );
};

void Physics::Init( )
{
	Physics::obj_count = 0;
	is_grabbed = false;
	grabbed = 0;

	Physics::gravity = 0;
	Physics::resistance_air = 1.1;
	Physics::resistance_wall = 0;
	Physics::richocet = 0.7;
	Physics::scale = 100; // should be applied to constant values like gravity
	Physics::area_x = 1920;
	Physics::area_y = 1080;
}

void Physics::AddObj( double x, double y, double d, double r, double v_x, double v_y, Texture* tex )
{
	Physics::m.push_back(d*4.18*r*r*r + 0.000001);		// mass
	Physics::r.push_back(r);		// radius
	Physics::A.push_back(M_PI*r*r);
	Physics::s_x.push_back(x);		// position
	Physics::s_y.push_back(y);		
	Physics::v_x.push_back(v_x);		// speed
	Physics::v_y.push_back(v_y);		
	Physics::a_x.push_back(0);		// acceleration
	Physics::a_y.push_back(0);
	Physics::k_x.push_back(0);		// kinetic energy
	Physics::k_y.push_back(0);		
	Physics::p_x.push_back(0);		// momentum
	Physics::p_y.push_back(0);		
	
	Physics::rm.push_back(0);		// moment of inertia
	Physics::rv.push_back(0);		// angular speed
	Physics::ra.push_back(0);		// angular acceleration
	Physics::rk.push_back(0);		// angular kinetic energy
	Physics::rp.push_back(0);		// angular momentum

	Physics::shp.push_back( CircleShape(r,30) );
	//Physics::shp[ Physics::obj_count ].setFillColor( Color( rand() % 255, rand() % 255, rand() % 255, 255 ) );
	Physics::shp[ Physics::obj_count ].setFillColor( Color( 255, 255, 255, 255) );
	Physics::shp[ Physics::obj_count ].setPosition( x, y );
	Physics::shp[ Physics::obj_count ].setOrigin( r, r );
	Physics::shp[ Physics::obj_count ].setTexture(tex);

	++Physics::obj_count;
}

void Physics::GrabAt( )
{
	Vector2i mouse = Mouse::getPosition();
	for(int i=0; i<obj_count; ++i)
	{
		if( mouse.x < s_x[i]+r[i] && mouse.x > s_x[i]-r[i] && mouse.y > s_y[i]-r[i] && mouse.y < s_y[i]+r[i] && is_grabbed == false )
		{
			is_grabbed = true;
			grabbed = i;
			i = obj_count;
		}
	}
}

void Physics::Push( )
{
	if(is_grabbed == true)
	{
		Vector2i mouse = Mouse::getPosition();
		v_x[grabbed] += (s_x[grabbed] - mouse.x)*(s_x[grabbed] - mouse.x)*sgn(s_x[grabbed] - mouse.x)/100;
		v_y[grabbed] += (s_y[grabbed] - mouse.y)*(s_y[grabbed] - mouse.y)*sgn(s_y[grabbed] - mouse.y)/100;
		is_grabbed = false;
	}
}

void Physics::Run( double _dt )
{
	double time_factor = _dt / 1000000;
	for(int i=0; i<obj_count; ++i)
	{


		v_y[i] += gravity * scale * time_factor;
		v_x[i] += gravity * 0.5 * scale * time_factor;

		double v = sqrt( v_x[i]*v_x[i] + v_y[i]*v_y[i] );
		double angle = atan2( v_y[i], v_x[i] );

		//if( v < 100 * time_factor ) v = 0;
		//else v -= 100 * time_factor;

		v *= 1 - ( 0.1 * time_factor );

		v_x[i] = cos(angle) * v;
		v_y[i] = sin(angle) * v;

		// Richocet from other balls
		for(int j=i+1; j<obj_count; ++j)
		{
			double dy = s_y[i] - s_y[j];
			double dx = s_x[i] - s_x[j];
			double dr = r[i] + r[j] + 0.0000001;
			double dist = sqrt(dy*dy + dx*dx);

			if( dist <= dr )
			{
				double vi = sqrt( v_x[i]*v_x[i] + v_y[i]*v_y[i] );
				double vj = sqrt( v_x[j]*v_x[j] + v_y[j]*v_y[j] );
	
				double avi = atan2( v_y[i], v_x[i] ); 
				double avj = atan2( v_y[j], v_x[j] ); 
	
				double avc = atan2( dy , dx );
	
				double vxin; // v _ i _ x new
				double vxjn;
				double vyin;
				double vyjn;
	
				double colltime;

				vxin = ( ( vi*cos(avi-avc)*(m[i]-m[j]) + 2*m[j]*vj*cos(avj-avc) ) / (m[i]+m[j]) )*cos(avc) + vi*sin(avi-avc)*cos(avc+M_PI_2);
				vyin = ( ( vi*cos(avi-avc)*(m[i]-m[j]) + 2*m[j]*vj*cos(avj-avc) ) / (m[i]+m[j]) )*sin(avc) + vi*sin(avi-avc)*sin(avc+M_PI_2);

				vxjn = ( ( vj*cos(avj-avc)*(m[j]-m[i]) + 2*m[i]*vi*cos(avi-avc) ) / (m[j]+m[i]) )*cos(avc) + vj*sin(avj-avc)*cos(avc+M_PI_2);
				vyjn = ( ( vj*cos(avj-avc)*(m[j]-m[i]) + 2*m[i]*vi*cos(avi-avc) ) / (m[j]+m[i]) )*sin(avc) + vj*sin(avj-avc)*sin(avc+M_PI_2);

				colltime = (dr-dist) / (vi+vj);

				s_x[i] -= (v_x[i] * colltime);
				s_y[i] -= (v_y[i] * colltime); 
				s_x[j] -= (v_x[j] * colltime); 
				s_y[j] -= (v_y[j] * colltime); 

				v_x[i] = vxin;
				v_y[i] = vyin;
				v_x[j] = vxjn;
				v_y[j] = vyjn;

				s_x[i] += (v_x[i] * colltime);
				s_y[i] += (v_y[i] * colltime);
				s_x[j] += (v_x[j] * colltime); 
				s_y[j] += (v_y[j] * colltime);
			}

			dy = s_y[i] - s_y[j];
			dx = s_x[i] - s_x[j];
			dr = r[i] + r[j];
			dist = sqrt(dy*dy + dx*dx);

			if( dist < 3 )
			{
				v_x[i] = rand() % 150;
				v_y[i] = rand() % 150;
				v_x[j] = - ( rand() % 150 );
				v_y[j] = - ( rand() % 150 );
				s_x[i] -= r[i] * ( rand() % 2 );
				s_y[i] -= r[i] * ( rand() % 2 );
				s_x[j] += r[j] * ( rand() % 2 );
				s_y[j] += r[j] * ( rand() % 2 );
			}

		}

		// Richocet from walls
		if( (s_y[i]+r[i]) >= area_y )
		{
			s_y[i] += ( area_y - s_y[i]-r[i] ) * ( 1 + richocet );
			v_y[i] *= -richocet;
		}
		if( (s_y[i]-r[i]) <= 0 )
		{
			s_y[i] -= ( 0 + s_y[i]-r[i] ) * ( 1 + richocet );
			v_y[i] *= -richocet;
		}
		if( (s_x[i]+r[i]) >= area_x )
		{
			s_x[i] += ( area_x - s_x[i]-r[i] ) * ( 1 + richocet );
			v_x[i] *= -richocet;
		}
		if( (s_x[i]-r[i]) <= 0 )
		{
			s_x[i] -= ( 0 + s_x[i]-r[i] ) * ( 1 + richocet );
			v_x[i] *= -richocet;
		}










		// Acceleration to speed
		//v_x[i] -= a_x[i] * time_factor ;
		//v_y[i] += a_y[i] * time_factor;



		// Air resistance to speed
		//v_x[i] -= ( 0.235 * resistance_air * v_x[i] * v_x[i] * A[i] * sgn( v_x[i] ) * time_factor ) / m[i];
		//v_y[i] -= ( 0.235 * resistance_air * v_y[i] * v_y[i] * A[i] * sgn( v_y[i] ) * time_factor ) / m[i];

		// Speed to space
		s_x[i] += v_x[i] * time_factor;
		s_y[i] += v_y[i] * time_factor;

		

		// Set image position
		shp[i].setPosition( s_x[i], s_y[i] );
	}
}

void Physics::Draw( RenderWindow* wnd )
{
	for(int i=0; i<obj_count; ++i)
	{
		(*wnd).draw( Physics::shp[i] );
	}
	if(is_grabbed)
	{
		Vector2i mouse = Mouse::getPosition();

		Vertex line[] =
		{
			Vertex( Vector2f(mouse.x, mouse.y), Color(0,0,0,255) ),
			Vertex( Vector2f(s_x[grabbed], s_y[grabbed]), Color(0,0,0,255) )
		};
		(*wnd).draw( line , 2, Lines); 
	}
}