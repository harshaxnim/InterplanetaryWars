#include <vector>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <math.h>
#include <cmath>

// My Includes
#include "Controller.h"

#define INS_LOOP 1
#define IMPACT 0.5

using namespace std;

using namespace Controller;

int c = 0;
float tp[] = {0,0,0,1};

float marsDist = 4;
float mBlazeX, mBlazeY, mBlazeZ, mBlazeA, mBlazeT;
float marsX, marsY, marsZ;
float mBlazeTimer;
int marsCrash = false;
float marsCrashT;

float earthDist = 2.5;
float eBlazeX, eBlazeY, eBlazeZ, eBlazeA, eBlazeT;
float earthX, earthY, earthZ;
float eBlazeTimer;
int earthCrash = false;
float earthCrashT;



float maxDist = marsDist + earthDist;

float crashSpeed = 0.5;
int blazePower = 1;

vector<int> children;
int parent;

void cleanUp(){
	if(getpid() == parent){
		for(int pid : children){
			cout << "Downing " << pid << endl;
			kill(pid, SIGKILL);
		}
	}
}

void say(string phrase){
	string command = "say "+phrase+" &";
	system(&command[0]);
}

void play(string fName, int loop = 0){
	children.push_back(fork());
	if(getpid() != parent){
		string path = "audio/"+fName;
		if(loop) execlp("mpg123","mpg123", "--loop", "10", &path[0], 0);
		else execlp("afplay","afplay", &path[0], 0);
	}
}

void showMatrix(double mat[16]){
	for (int i=0; i<16; i+=4){
		printf("%f %f %f %f\n",mat[i],mat[i+1],mat[i+2],mat[i+3]);
	}
	printf("\n\n");
	c++;
	if(c>10) exit(0);
}

void starAnim(){
	Controller::rotate(1*timerCounter, 0, 1, 0);
}

void sunAnim(){
	Controller::rotate(50*timerCounter, 0, 1, 0);
}

void moonAnim(){
	Controller::rotate(30*timerCounter, 0, 1, 0);
	Controller::move(0.3,0,0);
	Controller::rotate(-30*timerCounter, 0, 1, 0);
}

void earthAnim(){
	float theta = (50*(timerCounter+0.01));
	float earthPos = earthDist-(earthCrash*crashSpeed*(timerCounter-earthCrashT));
	Controller::move(earthPos,0,0);
	Controller::rotate(theta, 0, 1, 0);
	if(earthCrash) glLightfv(GL_LIGHT3, GL_POSITION, tp);

	earthX = earthPos*cos(theta*M_PI/180);
	earthY = 0.17;
	earthZ = -earthPos*sin(theta*M_PI/180);

	if(earthFrame){
		viewx = earthX;
		viewy = earthY;
		viewz = earthZ;
	}

	if(earthCrash && abs(earthDist-(earthCrash*crashSpeed*(timerCounter-earthCrashT)))<0.1 ){
		play("explode.wav");
		say("earth completely destroyed");
		scene.transfer("earth","sun","null");
		ectoFrame = true;
		earthFrame = false;
		
		glDisable(GL_LIGHT3);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, lightS1);
	}
	
	if (earthLaunch){
		play("lasershoot.wav");
		eBlazeX = earthX;
		eBlazeY = earthY;
		eBlazeZ = earthZ;
		eBlazeA = atan2(earthX, earthZ);
		eBlazeT = timerCounter;
		earthLaunch = false;
		scene.transfer("eBlaze", "null", "root");
		glEnable(GL_LIGHT2);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, lightS1D);
		// earthFrame = false;
		// ectoFrame = true;
	}
}

void marsAnim(){
	float theta = (50*(timerCounter+0.01));
	float marsPos = marsDist-(marsCrash*crashSpeed*(timerCounter-marsCrashT));
	Controller::rotate(theta, 0, 1, 0);
	Controller::move(marsPos ,0 ,0);
	Controller::rotate(-theta*(marsCrash*100), 0, 1, 0);
	if(marsCrash) glLightfv(GL_LIGHT2, GL_POSITION, tp);
	
	theta=(2*(theta+0.25));

	marsX = marsPos*cos(theta*M_PI/180);
	marsY = 0.09;
	marsZ = -marsPos*sin(theta*M_PI/180);

	if(marsFrame){
		viewx = marsX;
		viewy = marsY;
		viewz = marsZ;
	}


	if(marsCrash && abs(marsDist-(marsCrash*crashSpeed*(timerCounter-marsCrashT)))<0.1 ){
		play("explode.wav");
		say("mars completely destroyed");
		scene.transfer("mars","sun","null");
		ectoFrame = true;
		marsFrame = false;

		glDisable(GL_LIGHT2);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, lightS1);
	}
	if (marsLaunch){
		play("bungeeshoot.wav");
		mBlazeX = marsX;
		mBlazeY = marsY;
		mBlazeZ = marsZ;
		mBlazeA = atan2(marsX, marsZ);
		mBlazeT = timerCounter;
		marsLaunch = false;
		scene.transfer("mBlaze", "null", "root");
		glEnable(GL_LIGHT3);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, lightS1D);
		// marsFrame = false;
		// ectoFrame = true;
	}
}

void blazeDestroyAnim(){
	rotate(10000*timerCounter, 0,0,1);
}

void eBlazeAnim(){
	eBlazeTimer = timerCounter-eBlazeT;
	float dist = -3*eBlazeTimer;
	move(dist*sin(eBlazeA),0,dist*cos(eBlazeA));
	move(eBlazeX, 0, eBlazeZ);
	rotate(blazePower*eBlazeTimer, 0,1,0);
	glLightfv(GL_LIGHT2, GL_POSITION, tp);
	

	float blazeX = eBlazeX + dist*sin(eBlazeA);
	float blazeZ = eBlazeZ + dist*cos(eBlazeA);
	float d = sqrt( pow(blazeX-marsX,2) + pow(blazeZ-marsZ,2) );

	if ( (d <= IMPACT) && (abs(dist) >= (maxDist/2)) ){ // successful impact
		marsCrash = true;
		marsCrashT = timerCounter;

		play("electricshock.mp3");
		say("mars going down");
		
		scene.transfer("eBlaze", "root", "mars");
		scene.nodes["eBlaze"].assignTrans(blazeDestroyAnim);

	}

	if (abs((float)dist) > 10){ // out of bounds
		scene.transfer("eBlaze", "root", "null");
		glLightfv(GL_LIGHT1, GL_DIFFUSE, lightS1);
		glDisable(GL_LIGHT2);
		return;
	}

}
void mBlazeAnim(){
	mBlazeTimer = timerCounter-mBlazeT;
	float dist = -7*mBlazeTimer;
	move(dist*sin(mBlazeA),0,dist*cos(mBlazeA));
	move(mBlazeX, 0, mBlazeZ);
	rotate(blazePower*mBlazeTimer, 0,1,0);
	glLightfv(GL_LIGHT3, GL_POSITION, tp);

	
	float blazeX = mBlazeX + dist*sin(mBlazeA);
	float blazeZ = mBlazeZ + dist*cos(mBlazeA);
	float d = sqrt( pow(blazeX-earthX,2) + pow(blazeZ-earthZ,2) );

	if ( (d <= IMPACT) && (abs(dist) >= (maxDist/2)) ){ // successful impact
		earthCrash = true;
		earthCrashT = timerCounter;
		
		play("electricshock.mp3");
		say("earth is going down");

		glEnable(GL_LIGHT3);
		scene.transfer("mBlaze", "root", "earth");
		scene.nodes["mBlaze"].assignTrans(blazeDestroyAnim);

	}

	if (abs((float)dist) > 10){ // out of bounds
		scene.transfer("mBlaze", "root", "null");
		glDisable(GL_LIGHT3);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, lightS1);
		return;
	}
}




int main(int argc, char* argv[]){
	parent = getpid();

	say("Welcome to Inter-planetary war simulator!");
	play("bg1.mp3",INS_LOOP);
	play("bg2.mp3",INS_LOOP);

	Controller::Meta starMeta = {
		1,
		-00, {1,0,0},
		{0,0,0},
		{0,1,0},
		100,
		"stars",
		"root",
		starAnim
	};
	Controller::loadObj("models/obj/stars/Stars.obj", starMeta, "models/obj/stars/Stars_D.png",1);

	Controller::Meta sunMeta = {
		1,
		-00, {1,0,0},
		{0,0,0},
		{0,1,0},
		0.7,
		"sun",
		"root",
		sunAnim
	};
	Controller::loadObj("models/obj/sun/Sun.obj", sunMeta, "models/obj/sun/Sun_D.png",1);

	Controller::Meta earthMeta = {
		1,
		-00, {1,0,0},
		{0,0,0},
		{0,1,0},
		0.3,
		"earth",
		"sun",
		earthAnim
	};
	Controller::loadObj("models/obj/earth/Earth.obj", earthMeta, "models/obj/earth/Earth_D.png");

	Controller::Meta moonMeta = {
		1,
		-00, {1,0,0},
		{0,0,0},
		{0,1,0},
		0.07,
		"moon",
		"earth",
		moonAnim
	};
	Controller::loadObj("models/obj/moon/Moon.obj", moonMeta, "models/obj/moon/Moon_D.png");
	
	Controller::Meta marsMeta = {
		1,
		-00, {1,0,0},
		{0,0,0},
		{0,1,0},
		0.15,
		"mars",
		"sun",
		marsAnim
	};
	Controller::loadObj("models/obj/mars/Mars.obj", marsMeta, "models/obj/mars/Mars_D.png");

	Controller::Meta mBlazeMeta = {
		1,
		90, {0,1,0},
		{0,0,0},
		{0,1,0},
		0.35,
		"mBlaze",
		"null",
		mBlazeAnim
	};
	Controller::loadObj("models/obj/blaze/Blaze.obj", mBlazeMeta, "models/obj/blaze/mBlaze_D.png",1);

	Controller::Meta eBlazeMeta = {
		1,
		260, {0,1,0},
		{0,0,0},
		{0,1,0},
		0.2,
		"eBlaze",
		"null",
		eBlazeAnim
	};
	Controller::loadObj("models/obj/blaze/Blaze.obj", eBlazeMeta, "models/obj/blaze/eBlaze_D.png",1);

	atexit(cleanUp);
	Controller::init();
	return 0;
}

// normMultiplier
// angle, about[3]
// trans[3]
// base[3]
// scale
// myName
// parentName
// animFunc