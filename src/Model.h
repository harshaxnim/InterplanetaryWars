#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>

// Texture mapping Type
#define INS_FLAT 1
#define INS_SPHERE 2
#define INS_CYLINDER 3

// File Type
#define INS_PLY 1
#define INS_OBJ 2
using namespace std;

struct{
	float x;
	float y;
	float z;

	float xn=0;
	float yn=0;
	float zn=0;
	float hn=0;

	float xt=0;
	float yt=0;
	float ht=0;
} typedef Vertex;

struct{
	int a;
	int b;
	int c;
} typedef Face;

struct{
	float x=0;
	float y=0;
	float z=0;
} typedef Bare;


struct{
	float xMin=0;
	float yMin=0;
	float zMin=0;

	float xMax=0;
	float yMax=0;
	float zMax=0;

	float xCnt=0;
	float yCnt=0;
	float zCnt=0;
} typedef Analysis;

class Model{
public:
	int type;
	int vertexCount=0, faceCount=0;
	vector<Face> faces;
	vector<Vertex> vertexSet;
	Analysis res;
	int normMultiplier=1;
	void loadPly(string s, int n, int t);	
	void calculateNormals(Face f);
	void calculateTexture(int type);
	void updateNormal(int index, float xn, float yn, float zn);
	void superNormalizeNormals(); // account all normals
	void resize();
	void analyse();
	Analysis getAnalysis();
	
	vector<Bare> tempVertices;
	vector<Bare> tempNormals;
	vector<Bare> tempTextures;
	void loadObj(string s, int normMultiplier);
	void addToVertices(int l, int t, int n);
};

#endif
