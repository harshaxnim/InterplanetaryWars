#include "Controller.h"
#include <iostream>
#include <map>
#include <string>
#include <unistd.h>
#include <math.h>

using namespace std;

View view;

vector<Model> models;
vector<Controller::Meta> metas;

namespace Controller{

	int modelCount = 0;

	// Camera Params
	float viewx = -2.5;
	float viewy = 1.5;
	float viewz = 2.5;
	float fovy = 70;
	
	int earthLaunch = false;
	int marsLaunch = false;

	int earthFrame = false;
	int ectoFrame = true;
	int marsFrame = false;

	// Motion
	float xAng, yAng;
	int pause = false;
	float timeStep = 0.01;
	float rotation_transform[4][4];

	
	// ViewPort Size
	float dw, dh;
	int fullscreen = false;

	// Lighting
	GLfloat lightH1A[] = { 0.3, 0.3, 0.3, 1 }; // Headlight 1
	GLfloat lightH1D[] = { 0.6, 0.6, 0.6, 1 }; // Headlight 1
	
	GLfloat lightS1[] = { 1, 1, 1, 1 }; // Spotlight 1
	GLfloat lightS1D[] = { .1, .1, .1, 1 }; // Spotlight 1

	GLfloat lightS2[] = { 1, 1, 1, 1 }; // Spotlight 2
	GLfloat lightS3[] = { 1, 1, 1, 1 }; // Spotlight 3
	GLfloat lightS4[] = { 1, 1, 1, 1 }; // Spotlight 4
	GLfloat lightDark[] = { 0, 0, 0, 1 }; // Spotlight Dark

	GLfloat position[][4] = {
		{0, -1.02, 0}, // Headlight
		// { -1, 1.5, 0, 1 }, // Spotlight 1
		{ 0, 0, 0, 1 }, // Spotlight 1
		{ 1, 1.5, 0, 1 },  // Spotlight 2
		{ -1, 1.5, 2, 1 },  // Spotlight 3
		{ 5, 10,  -5, 1 },  // Spotlight 4
	};
	
		// spotlight params
	float sc = 70;
	float se = 4;
	float sd1[] = {-1,0,0};
	float sd2[] = {0,-1,0};
	float sd3[] = {0,-1,0};
	float sd4[] = {0,-1,0};

	int lightStatus[] = { 0, 1, 0, 0, 0};
	// int lightStatus[] = { 1, 1, 1, 1, 1};

	// Buffer Setup
	vector<GLuint> VBO(0), IBO(0), TEX(0);
	vector<string> texfNames(0);
	GLuint FTEX;

	// Floor Setup
	float floorSize;
	string floorTex;

	// misc

	int diag = 0;
	int texStatus = 1;

	// Scene
	Scene scene;

	float timerCounter = 0;


	// Method Implementations


	void loadObj(string s, Meta c, string texfName, int lightSource){
		Model model;
		models.push_back(model);
		cout << "Loading " << s << endl;
		models[modelCount].type = INS_OBJ;
		models[modelCount].loadObj(s, -lightSource);
		c.level= 0.01+
					c.base[0]*(models[modelCount].res.xCnt-models[modelCount].res.xMin)+ 
					c.base[1]*(models[modelCount].res.yCnt-models[modelCount].res.yMin)+ 
					c.base[2]*(models[modelCount].res.zCnt-models[modelCount].res.zMin);
		metas.push_back(c);
		modelCount++;
		VBO.resize(modelCount);
		IBO.resize(modelCount);
		TEX.resize(modelCount);
		texfNames.push_back(texfName);
	}

	void loadPly(string s, Meta c, string texfName, int type, int lightSource){
		Model model;
		models.push_back(model);
		cout << "Loading " << s << endl;
		models[modelCount].type = INS_PLY;
		models[modelCount].loadPly(s, -lightSource*c.normMultiplier, type);
		c.level= 0.01+
					c.base[0]*(models[modelCount].res.xCnt-models[modelCount].res.xMin)+ 
					c.base[1]*(models[modelCount].res.yCnt-models[modelCount].res.yMin)+ 
					c.base[2]*(models[modelCount].res.zCnt-models[modelCount].res.zMin);
		metas.push_back(c);
		modelCount++;
		VBO.resize(modelCount);
		IBO.resize(modelCount);
		TEX.resize(modelCount);
		texfNames.push_back(texfName);
	}

	void floorSetup(float s, string texName){
		floorTex = texName;
		floorSize = s;
	}


	void init(){
		view.initializeWindow();
		
		cout << "Running OpenGL " << glGetString(GL_VERSION) << endl << endl;

		view.assignCallBacks(reshape, display, keyboard, mouse, motion, timer);

		glClearColor(0.1,0.1,0.1,1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnable (GL_DEPTH_TEST);
		glEnable(GL_NORMALIZE);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Lighting
		glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);

		if(lightStatus[0]) glEnable(GL_LIGHT0); // Headlight
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightH1A);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightH1D);

		
		if(lightStatus[1]) glEnable(GL_LIGHT1); // Spotlight 1
		glLightfv(GL_LIGHT1, GL_AMBIENT, lightDark);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, lightS1);
		// glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, &sc);
		// glLightfv(GL_LIGHT1, GL_SPOT_EXPONENT, &se);
		// glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, sd1);

		if(lightStatus[2]) glEnable(GL_LIGHT2); // Spotlight 2
		glLightfv(GL_LIGHT2, GL_AMBIENT, lightDark);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, lightS2);
		// glLightfv(GL_LIGHT2, GL_SPOT_CUTOFF, &sc);
		// glLightfv(GL_LIGHT2, GL_SPOT_EXPONENT, &se);
		// glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, sd2);

		if(lightStatus[3]) glEnable(GL_LIGHT3); // Spotlight 3
		glLightfv(GL_LIGHT3, GL_AMBIENT, lightDark);
		glLightfv(GL_LIGHT3, GL_DIFFUSE, lightS3);
		// glLightfv(GL_LIGHT3, GL_SPOT_CUTOFF, &sc);
		// glLightfv(GL_LIGHT3, GL_SPOT_EXPONENT, &se);
		// glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, sd3);

		if(lightStatus[4]) glEnable(GL_LIGHT4); // Spotlight 4
		glLightfv(GL_LIGHT4, GL_AMBIENT, lightDark);
		glLightfv(GL_LIGHT4, GL_DIFFUSE, lightS4);
		glLightfv(GL_LIGHT4, GL_SPOT_CUTOFF, &sc);
		glLightfv(GL_LIGHT4, GL_SPOT_EXPONENT, &se);
		glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, sd4);


		// Buffers and Textures
		// Create a new VBO and use the variable id to store the VBO id
		glGenBuffers (modelCount, &VBO[0]);
		glGenBuffers (modelCount, &IBO[0]);
		glGenTextures(modelCount, &TEX[0]);


		// Do the model specific ops here
		for(int i=0; i<modelCount; i++){

			// Now push the data into the buffer
			if (models[i].type == INS_PLY){
				
				// Push Vertex Properties
				glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
				glBufferData(GL_ARRAY_BUFFER, models[i].vertexSet.size()*sizeof(Vertex), &models[i].vertexSet[0], GL_STATIC_DRAW);
	
				// Push Indices
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[i]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, models[i].faces.size()*sizeof(Face), &models[i].faces[0], GL_STATIC_DRAW);
			}

			else if (models[i].type == INS_OBJ){
				// Push Vertex Properties
				glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
				glBufferData(GL_ARRAY_BUFFER, models[i].vertexSet.size()*sizeof(Vertex), &models[i].vertexSet[0], GL_STATIC_DRAW);
			}

			
			// Load/Push Textures
			int width, height;
			unsigned char* image;
			width=0;
			height=0;
			image = SOIL_load_image(&texfNames[i][0], &width, &height, 0, SOIL_LOAD_RGBA);
			cout << texfNames[i] << ": " << width << " " << height << endl;
			glBindTexture(GL_TEXTURE_2D, TEX[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			// glGenerateMipmap(GL_TEXTURE_2D);


			// Scene Setup
			scene.newNode(i, metas[i].myName, metas[i].parentName, metas[i].anim);

		}

		// Floor Setup 

		int width, height;
		unsigned char* image;
		image = SOIL_load_image(&floorTex[0], &width, &height, 0, SOIL_LOAD_RGB);

		glBindTexture(GL_TEXTURE_2D, FTEX);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
		
		view.kickOff();
	}


	void display(){

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(fovy, ((float)dw)/dh, 0.01, 100);
		
		glMatrixMode(GL_MODELVIEW);
		
		glLoadIdentity();

		showHUD();

		glRotatef(xAng, 0,1,0);
		if (ectoFrame){
			viewx = -4.5;
			viewy = 2.5;
			viewz = 3.5;
		}

		gluLookAt(viewx,viewy,viewz,  0, 0, 0,  0.0, 1.0, 0.0);

		// Update spotlights of the Scene
		glLightfv(GL_LIGHT1, GL_POSITION, position[1]);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, sd1);

		// glLightfv(GL_LIGHT2, GL_POSITION, position[2]);
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, sd2);
		
		// glLightfv(GL_LIGHT3, GL_POSITION, position[3]);
		glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, sd3);

		// glLightfv(GL_LIGHT4, GL_POSITION, position[4]);
		glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, sd4);
		

		// drawLights();
		if (diag) {
			drawAxes();
			drawMesh();
			drawFloor();
		}
		
		scene.drawScene();

	}

	void reshape(int x, int y){
		dw = x;
		dh = y;
		glutWarpPointer(dw/2,dh/2);

		glViewport(0,0,dw,dh);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(fovy, ((float)x)/y, 0.01, 100);

	}

	void timer(int val){
		view.refresh();
		view.updateDisplay();
		view.timerReset(timer);

		if(!pause) timerCounter+=timeStep;
	}
	
	void motion(int x, int y){
		xAng=(x-(dw/2))/10;
	}

	void drawModels(){
		for(int i=0; i<modelCount; i++){
			drawModel(i);
		}

	}

	void drawModel(int i){
		glPushMatrix();
		adjustModel(i);

		if (i < 0) return;
		
		glColor4f(1,1,1,1);
		if (i == 0){

			glDisable(GL_CULL_FACE);			
		}
		if(i==4) glColor4f(1,1,1,1);
		// else glColor4f(1,1,1,1);

		if (texStatus) glEnable(GL_TEXTURE_2D);
		glEnable (GL_LIGHTING);

		// Actual Bind and Draw
		glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
		glBindTexture(GL_TEXTURE_2D, TEX[i]);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), 0);

		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, sizeof(Vertex), (void*)(3*sizeof(float)));

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)(7*sizeof(float)));
		
		if (models[i].type == INS_PLY){
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[i]);
			glDrawElements(GL_TRIANGLES, models[i].faces.size()*3, GL_UNSIGNED_INT, 0);
		}
		else if (models[i].type == INS_OBJ){
			glPointSize(1);
			glDrawArrays(GL_TRIANGLES, 0, models[i].vertexSet.size());
		}

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		// Done
		
		glDisable (GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);


		if(diag) drawBoundingBox(i);
		if (i == 0){
			glEnable(GL_CULL_FACE);
		}

		glPopMatrix();

	}

	void adjustModel(int i){
		glRotatef(metas[i].angle, metas[i].about[0], metas[i].about[1], metas[i].about[2]); // Rotate to make the side up
		glScalef(metas[i].scale,metas[i].scale,metas[i].scale);
		glTranslatef(-models[i].res.xCnt, -models[i].res.yCnt, -models[i].res.zCnt); // Places in the centre
	}

	void drawFloor(){
		if (texStatus) glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, FTEX);
		glEnable(GL_LIGHTING);

		glNormal3f(0,1,0);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0);
			glVertex3f(-floorSize,-0.01,-floorSize);

			glTexCoord2f(0.0, 5);
			glVertex3f(-floorSize,-0.01,floorSize);

			glTexCoord2f(5, 5);
			glVertex3f(floorSize,-0.01,floorSize);

			glTexCoord2f(5, 0.0);
			glVertex3f(floorSize,-0.01,-floorSize);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
	}

	void drawAxes(){
		float depth = 0.3;
		float length = 1;
		glLineWidth(3);
		glColor3f(depth,0,0);
		glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(length,0,0);
		glEnd();
		glColor3f(0,depth,0);
		glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(0,length,0);
		glEnd();
		glColor3f(0,0,depth);
		glBegin(GL_LINES);
			glVertex3f(0,0,0);
			glVertex3f(0,0,length);
		glEnd();
	}

	void drawMesh(){
		glColor3f(.3,.3,.3);
		glLineWidth(1);
		float spacing=0.5;
		float range = 5;
		for(float x = -range; x <= range; x+=spacing){
			glBegin(GL_LINE_LOOP);
			glVertex4f(x,0,range,1);
			glVertex4f(x,0,-range,1);
			glEnd();
		};

		for(float z = range; z >= -range; z-=spacing){
			glBegin(GL_LINE_LOOP);
			glVertex4f(-range,0,z,1);
			glVertex4f(range,0,z,1);
			glEnd();
		};

	}


	void drawBoundingBox(int i){
		Model model = models[i];
		glLineWidth(1);
		glColor3f(1,1,1);
		glBegin(GL_LINE_LOOP);
			glVertex3f(model.res.xMax, model.res.yMax, model.res.zMax);
			glVertex3f(model.res.xMax, model.res.yMin, model.res.zMax);
			glVertex3f(model.res.xMin, model.res.yMin, model.res.zMax);
			glVertex3f(model.res.xMin, model.res.yMax, model.res.zMax);
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(model.res.xMax, model.res.yMax, model.res.zMin);
			glVertex3f(model.res.xMax, model.res.yMin, model.res.zMin);
			glVertex3f(model.res.xMin, model.res.yMin, model.res.zMin);
			glVertex3f(model.res.xMin, model.res.yMax, model.res.zMin);
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(model.res.xMax, model.res.yMax, model.res.zMin);
			glVertex3f(model.res.xMax, model.res.yMax, model.res.zMax);

			glVertex3f(model.res.xMax, model.res.yMin, model.res.zMin);
			glVertex3f(model.res.xMax, model.res.yMin, model.res.zMax);

			glVertex3f(model.res.xMin, model.res.yMin, model.res.zMin);
			glVertex3f(model.res.xMin, model.res.yMin, model.res.zMax);

			glVertex3f(model.res.xMin, model.res.yMax, model.res.zMin);
			glVertex3f(model.res.xMin, model.res.yMax, model.res.zMax);
		glEnd();

	}

	void drawLights(){
		glPointSize(20);
		glBegin(GL_POINTS);
		for(int i=1; i<sizeof(position)/(16); i++){
			if(lightStatus[i]) glColor3f(1,1,1);
			else glColor3f(0.3,0.3,0.3);
			glVertex3f(position[i][0],position[i][1],position[i][2]);
		}
		glEnd();
	}

	
	void keyboard(unsigned char c, int x, int y){
		if (c == 27 || c == 81 || c == 113) { // ESC or Q
			printf("\nExit\n");
			exit(0);
		}

		// Camera Move
		float mStep = 0.05;
		if(ectoFrame){
			if (c == 'w') viewy += mStep;
			if (c == 'a') viewx -= mStep;
			if (c == 's') viewy -= mStep;
			if (c == 'd') viewx += mStep;
			if (c == 'z') viewz += mStep;
			if (c == 'x') viewz -= mStep;
		}
		
		// Camera Zoom
		float zStep = 2;
		if (c == '=') fovy-=zStep;
		if (c == '-') fovy+=zStep;
		
		// Light Control
		if (c == '0'){
			switchLight(0);
		}
		if (c == '1'){
			switchLight(1);
		}

		if (c == 'r'){
			timerCounter = 0;
		}

		if (c == 'p'){
			pause = !pause;
		}
		if ( (c == '[') && pause ){
			timerCounter-=timeStep*5;
		}
		if ( (c == ']') && pause ){
			timerCounter+=timeStep*5;
		}

		if (c == ','){
			earthFrame = true;
			ectoFrame = marsFrame = false;
		}
		if (c == '.'){
			marsFrame = true;
			ectoFrame =	earthFrame = false;
		}
		if (c == '/'){
			ectoFrame = true;
			marsFrame = earthFrame = false;
		}


		if (c == 'v') {
			diag = !diag;
			if(diag) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		if (c == 't') texStatus = !texStatus;

		if (c == 'f'){
			if(fullscreen){
				view.winRestore();
			}
			else{
				view.winFullscreen();
			}
			fullscreen = !fullscreen;
		}
	}
	
	void mouse(int b, int s, int x, int y){
		if (s == GLUT_DOWN){
			if(marsFrame) marsLaunch = true;
			if(earthFrame || ectoFrame) earthLaunch = true;
		}
	}


	void switchLight(int l){
		if (l == 0){
			lightStatus[0] = !lightStatus[0];
			if(lightStatus[0]) glEnable(GL_LIGHT0); else
			glDisable(GL_LIGHT0);
		}
		if (l == 1){
			lightStatus[1] = !lightStatus[1];
			if(lightStatus[1]) glEnable(GL_LIGHT1); else
			glDisable(GL_LIGHT1);
		}
		if (l == 2){
			lightStatus[2] = !lightStatus[2];
			if(lightStatus[2]) glEnable(GL_LIGHT2); else
			glDisable(GL_LIGHT2);
		}

		if (l == 3){
			lightStatus[3] = !lightStatus[3];
			if(lightStatus[3]) glEnable(GL_LIGHT3); else
			glDisable(GL_LIGHT3);
		}

		if (l == 4){
			lightStatus[4] = !lightStatus[4];
			if(lightStatus[4]) glEnable(GL_LIGHT4); else
			glDisable(GL_LIGHT4);
		}
	}

	void showHUD(){
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		drawHeadlight();
		drawTextureButton();

		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}

	void drawTextureButton(){
		glPointSize(100);
		glColor3f(0.5,0.5,0.5);
		glBegin(GL_POINTS);
			glVertex3f(position[0][0],-position[0][1],position[0][2]);
		glEnd();
	}

	void drawHeadlight(){
		glPointSize(100);
		if(lightStatus[0]) glColor3f(1,1,1);
		else glColor3f(0.2,0.2,0.2);
		glBegin(GL_POINTS);
			glVertex3f(position[0][0],position[0][1],position[0][2]);
		glEnd();
	}


	 // SceneGraph Implementation ////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	
	// SceneNode //////////////////////
	void SceneNode::addChild(SceneNode* child){
		children.push_back(child);
	}

	void SceneNode::assignTrans(void t()){
		trans = t;
	}

	void SceneNode::draw(){
		glPushMatrix();
		this->trans();
		drawModel(id);
		for(SceneNode* child : children){
			child->draw();
		}
		glPopMatrix();
	}

	// Scene //////////////////////
	void Scene::newNode(int id, string nodeName, string parent, void trans()){
		SceneNode node(id);
		if(nodes.count(nodeName) > 0){
			cout << "Watch Out! Replacing the existing node with ID: " << nodeName << endl;
		}
		nodes[nodeName] = node;

		nodes[nodeName].assignTrans(trans);

		if (nodes.count(parent) == 0){
			cout << "Can't assign to " << parent<< ". Not Found" << endl;
			exit(1);
		}
		nodes[parent].addChild(&nodes[nodeName]);
	}

	void Scene::drawScene(){
		for( SceneNode* s : nodes["root"].children ) {
			s->draw();
		}
	}

	SceneNode* Scene::getNode(string name){
		return &nodes[name];
	}

	void Scene::transfer(string child, string currentParent, string newParent){
		if(nodes.count(child) == 0){
			cout << "Can't find the child!" << endl;
			return;
		}
		int cid = nodes[child].id;

		vector<SceneNode*>& c = nodes[currentParent].children;
		int found = false;
		for (int i=0; i < c.size(); i++){
			if( c[i]->id == cid ){
				// cout << i <<"th child cleared" << endl;
				c.erase(c.begin()+i);
				found = true;
			}
		}
		if(!found){
			// cout << "That's not the parent!" << endl;
			return;
		}

		nodes[newParent].addChild(&nodes[child]);
		// cout << "Transfer successful!" << endl;
	}


	// Helpers for Main

	void rotate( float angle, float x, float y, float z ){
		glRotatef(angle, x, y, z);
	}
	void move( float x, float y, float z ){
		glTranslatef(x, y, z);
	}

}