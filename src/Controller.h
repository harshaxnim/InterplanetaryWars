#ifndef CONTROLLER_H
#define CONTROLLER_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>
#include <GL/glext.h>
#endif

#include <SOIL.h>
#include <string>
#include <map>
#include "View.h"
#include "Model.h"
#include "utils/FastTrackball.h"


namespace Controller{

	struct{
		int normMultiplier;

		int angle;
		float about[3];

		float trans[3];
		
		int base[3];
		float scale;

		string myName;
		string parentName;
		void (*anim)();


		float level;
	} typedef Meta;

	class Scene;

	extern GLfloat lightS1[];
	extern GLfloat lightS1D[];

	extern float timerCounter;
	extern float viewx;
	extern float viewy;
	extern float viewz;
	extern float xAng;
	extern int earthFrame;
	extern int earthLaunch;
	extern int marsFrame;
	extern int marsLaunch;
	extern int ectoFrame;

	extern Scene scene;



	void init();
	void loadPly(string s, Meta c, string tex, int type, int source = -1);
	void loadObj(string s, Meta c, string t, int source = -1);
	void floorSetup(float s, string tex);
	
	void display();
	void reshape(int x, int y);
	void keyboard(unsigned char c, int x, int y);
	void mouse(int b, int s, int x, int y);
	void motion(int x, int y);
	void timer(int val);

	void drawAxes();
	void drawMesh();
	void drawModels();
	void drawModel(int i);
	void adjustModel(int i);
	void drawModel(string name);
	void drawBoundingBox(int i);
	void drawLights();
	void drawHeadlight();
	void drawFloor();
	void drawTextureButton();
	void showHUD();

	void switchLight(int l);
	void fakeRender();

	void rotate( float angle, float x, float y, float z );
	void move( float x, float y, float z );

	class SceneNode{
	public:
		SceneNode(){};
		SceneNode(int i){id = i;};
		
		vector<SceneNode*> children;
		int id;
		void (*trans)() = NULL;

		void addChild(SceneNode* child);
		void assignTrans(void t());
		void draw();
	};

	typedef map <string, SceneNode> Map;

	class Scene{
	public:
		map<string,SceneNode> nodes;

		Scene(){
			nodes["root"] = SceneNode(-1);
			nodes["null"] = SceneNode(-2);
		}

		void newNode(int id, string nodeName, string parent, void t());
		SceneNode* getNode(string nodeName);
		void transfer(string child, string currentParent, string newParent);
		void drawScene();
	};
};

#endif