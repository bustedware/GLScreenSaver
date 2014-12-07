/***********************************************************************
	File: main.cpp

	Jacob Bills - 2008

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>

//$--------------------------------------------------------------------- 
	Main GL programming is here, this screensaver draws graphs of 2 different
	functions.

	Last Modified: November 1, 2008
	Version: v1.0
//*********************************************************************/

#include <math.h>
#include <ctime>
#include "GLScreenSaver.hpp"

const int UPDATE_INTERVAL = 5000; //5000 milliseconds = 5 seconds
const int UPDATE_TIMER = 2;

//screensaver class
GLScreenSaver scr;

//function prototypes and stuff
typedef float (*function)(float, float);
typedef void (*normal) (float, float, float, float*);
float f1(float x, float z);
float f2(float x, float z);
void normal1(float x, float y, float z, float* norm);
void normal2(float x, float y, float z, float* norm);
function drawFuncs[2] = {&f1, &f2};
normal normFuncs[2] = {&normal1, &normal2};
int curFIndex = 0;
int drawMethods[3] = {GL_LINE_STRIP, GL_POLYGON, GL_POINTS};
int curDrawIndex = 1;

//angles to rotate
float theta = 0, beta = 0;
float thetaInc = 0.1f, betaInc = 0.05f;

//window coordinates
float xmin=-2.0f, xmax=2.0f, ymin=-2.0f,ymax=2.0f,zmin=-2.0f,zmax=2.0f;

//square grid to generate
int n = 50, m = 50;

//generate a random number
int random_number(int min, int max) 
{ 
    srand((unsigned)time(0)); 
    int random_integer;
    int range=max-min+1;

    random_integer = (rand()%range)+min;

	return random_integer;
}

//setup gl world
void Setup(int w, int h)
{
	//set the viewing area
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(xmin, xmax, ymin, ymax, zmin, zmax);

	GLfloat mat_specular[] = {0.0f, 1.0f, 0.0f, 0.0f};
	GLfloat mat_diffuse[] = {1.0f, 1.0f, 1.0f, 0.0f};
	GLfloat mat_ambient[] = {1.0f, 1.0f, 1.0f, 0.0f};
	GLfloat shininess[] = {2.0f};
	
	GLfloat light_specular[] = {0.0f, 1.0f, 0.0f, 1.0f};
	GLfloat light_diffuse[] = {1.0f, 0.9f, 0.9f, 0.0f};
	GLfloat light_ambient[] = {0.1f, 0.1f, 0.1f, 0.0f};
	GLfloat light_position[] = {-20.0f, 4.0f, 0.0f, 0.0f};

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	glShadeModel(GL_SMOOTH);
	float lmodel_ambient[] = {0.4f, 0.4f, 0.4f, 0.4f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0,0,0,0);
	glColor3f(0,0,0);
}

//update the world
void Update()
{
	theta += thetaInc;
	if (theta > 360)
		theta = 0;

	beta += betaInc;
	if (beta > 60 || beta <= 0)
		betaInc *= -1;
}

//draw the world
void Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(beta,1,0,0);
	glRotatef(theta,0,1,0);

	float norm[3];
	float xgap=(xmax-xmin)/n;
	float zgap=(zmax-zmin)/m;

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			glBegin(drawMethods[curDrawIndex]);
			float x = xmin+i*xgap;
			float z = zmin+j*zgap;
			float y = drawFuncs[curFIndex](x,z);
			glVertex3f(x,y,z);
			normFuncs[curFIndex](x,y,z,norm);
			glNormal3fv(norm);

			x = xmin+i*xgap;
			z = zmin+(j+1)*zgap;
			y = drawFuncs[curFIndex](x,z);
			glVertex3f(x,y,z);
			normFuncs[curFIndex](x,y,z,norm);
			glNormal3fv(norm);

			x = xmin+(i+1)*xgap;
			z = zmin+(j+1)*zgap;
			y = drawFuncs[curFIndex](x,z);
			glVertex3f(x,y,z);
			normFuncs[curFIndex](x,y,z,norm);
			glNormal3fv(norm);
			glEnd();

			glBegin(drawMethods[curDrawIndex]);
			x = xmin+(i+1)*xgap;
			z = zmin+j*zgap;
			y = drawFuncs[curFIndex](x,z);
			glVertex3f(x,y,z);
			normFuncs[curFIndex](x,y,z,norm);
			glNormal3fv(norm);

			x = xmin+i*xgap;
			z = zmin+j*zgap;
			y = drawFuncs[curFIndex](x,z);
			glVertex3f(x,y,z);
			normFuncs[curFIndex](x,y,z,norm);
			glNormal3fv(norm);

			x = xmin+(i+1)*xgap;
			z = zmin+(j+1)*zgap;
			y = drawFuncs[curFIndex](x,z);
			glVertex3f(x,y,z);
			normFuncs[curFIndex](x,y,z,norm);
			glNormal3fv(norm);
			glEnd();
		}
	}
}

//function 1
float f1(float x, float z)
{
	return exp(-x*x-z*z);
}


//calculate the normal to function 1
void normal1(float x, float y, float z, float* norm)
{
	norm[0] = 2*x*exp(-x*x-z*z);
	norm[1] = 1;
	norm[2] = 2*z*exp(-x*x-z*z);
	float d = norm[0]*norm[0]+norm[1]*norm[1]+norm[2]*norm[2];
	if (d>0)
	{
		for (int k = 0; k < 3; k++)
			norm[k]/=d;
	}
}

//function 2
float f2(float x, float z)
{
	return z*cos(x);
}

//calculate the normal to function 2
void normal2(float x, float y, float z, float* norm)
{
	norm[0] = z*sin(x);
	norm[1] = 1;
	norm[2] = -1*cos(x);
	float d = norm[0]*norm[0]+norm[1]*norm[1]+norm[2]*norm[2];
	if (d>0)
	{
		for (int k = 0; k < 3; k++)
			norm[k]/=d;
	}
}

//handles timer calls
void Timer(WPARAM wParam)
{
	if (wParam == UPDATE_TIMER)
	{
		curFIndex = random_number(0, 1);
		curDrawIndex = random_number(0, 2);
	}
}

//clean up
void Clean()
{
	KillTimer(scr.hWnd, UPDATE_TIMER);
}

//entry point
void screen_saver_main()
{
	scr.glSetupFunc(Setup);
	scr.glUpdateFunc(Update);
	scr.glDrawFunc(Draw);
	scr.glTimerFunc(Timer);
	scr.glCleanFunc(Clean);

	SetTimer(scr.hWnd, UPDATE_TIMER, UPDATE_INTERVAL, NULL);
}
