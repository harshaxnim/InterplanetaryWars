#include <math.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdio.h>

#include "Model.h"

using namespace std;

void Model::loadObj(string fName, int n){
	normMultiplier = n;
	string line;
	ifstream source(fName);
	if (source.is_open()){
		while(getline (source,line)){
			if ( line.compare(0,2,"v ") == 0 ){
				Bare temp;
				stringstream ss(line.substr(2));
				ss >> temp.x >> temp.y >> temp.z ;
				tempVertices.push_back(temp);
			}

			else if ( line.compare(0,2,"vn") == 0 ){
				Bare temp;
				stringstream ss(line.substr(3));
				ss >> temp.x >> temp.y >> temp.z ;
				tempNormals.push_back(temp);
			}

			else if ( line.compare(0,2,"vt") == 0 ){
				Bare temp;
				stringstream ss(line.substr(3));
				ss >> temp.x >> temp.y ;
				tempTextures.push_back(temp);
			}

			else if ( line.compare(0,1,"f") == 0 ){

				int a,b,c, d,e,f, g,h,i;
				sscanf(&line[0],"%*s %d/%d/%d %d/%d/%d %d/%d/%d\n", &a,&b,&c, &d,&e,&f, &g,&h,&i);
				addToVertices(a,b,c);
				addToVertices(d,e,f);
				addToVertices(g,h,i);
			}
		}
		vertexCount = vertexSet.size();
	}
	else{
		cout << "Can't open the file" << endl;
		exit(-1);
	}

	resize();

}

void Model::addToVertices(int l, int t, int n){
	vertexCount+=3;
	
	Vertex tmp;
	
	tmp.x = tempVertices[l-1].x;
	tmp.y = tempVertices[l-1].y;
	tmp.z = tempVertices[l-1].z;

	tmp.xt = tempTextures[t-1].x;
	tmp.yt = -tempTextures[t-1].y;

	tmp.xn = normMultiplier*tempNormals[n-1].x;
	tmp.yn = normMultiplier*tempNormals[n-1].y;
	tmp.zn = normMultiplier*tempNormals[n-1].z;

	vertexSet.push_back(tmp);
}

void Model::loadPly(string s, int n, int t){
	normMultiplier = n;
	string line;
	ifstream source(s);
	if (source.is_open()){
		float a=0,b=0,c=0,d=0;
		while(getline (source,line)){
			if(line.compare(0,14,"element vertex") == 0){
				stringstream ss(line.substr(14));
				ss >> vertexCount;
			}
			else if(line.compare(0,12,"element face") == 0){
				stringstream ss(line.substr(12));
				ss >> faceCount;
			}
			else if(line.compare(0,10,"end_header") == 0){
				break;
			}
		}

		for(int i=0; i<vertexCount; i++){
			getline (source,line);
			stringstream ss(line);
			ss >> a >> b >> c;

			Vertex v;
			v.x=a;
			v.y=b;
			v.z=c;
			vertexSet.push_back(v);
		}
		for(int i=0; i<faceCount;i++){
			getline (source,line);
			stringstream ss(line);
			ss >> d >> a >> b >> c;

			Face f;
			f.a=a;
			f.b=b;
			f.c=c;
			faces.push_back(f);
			calculateNormals(f);
		}
		superNormalizeNormals(); // account all normals
	}
	else{
		cout << "Can't open the file" << endl;
		exit(-1);
	}
	resize();
	calculateTexture(t);
}

void Model::calculateNormals(Face f){
	float xn=0, yn=0, zn=0, va[3]={0,0,0}, vb[3]={0,0,0}, val=0;
	
	// get vectors
	va[0] = vertexSet[f.a].x - vertexSet[f.b].x;
	va[1] = vertexSet[f.a].y - vertexSet[f.b].y;
	va[2] = vertexSet[f.a].z - vertexSet[f.b].z;
 
	vb[0] = vertexSet[f.a].x - vertexSet[f.c].x;
	vb[1] = vertexSet[f.a].y - vertexSet[f.c].y;
	vb[2] = vertexSet[f.a].z - vertexSet[f.c].z;
 
	// cross product
	xn = va[1] * vb[2] - vb[1] * va[2];
	yn = vb[0] * va[2] - va[0] * vb[2];
	zn = va[0] * vb[1] - vb[0] * va[1];
 
	// normalize
	val = normMultiplier*sqrt( xn*xn + yn*yn + zn*zn );
	xn /= val;
	yn /= val;
	zn /= val;
	
	// update
	updateNormal(f.a, xn, yn, zn);
	updateNormal(f.b, xn, yn, zn);
	updateNormal(f.c, xn, yn, zn);
	
}

void Model::updateNormal(int index, float xn, float yn, float zn){
	vertexSet[index].xn += xn;
	vertexSet[index].yn += yn;
	vertexSet[index].zn += zn;
	vertexSet[index].hn++;
}

void Model::superNormalizeNormals(){ // account all normals
	for(int i=0; i<vertexCount; i++){
		vertexSet[i].xn /= vertexSet[i].hn;
		vertexSet[i].yn /= vertexSet[i].hn;
		vertexSet[i].zn /= vertexSet[i].hn;
		vertexSet[i].hn = 1;
	}

}

void Model::analyse(){
	res.xMin = res.xMax = vertexSet[0].x;
	res.yMin = res.yMax = vertexSet[0].y;
	res.zMin = res.zMax = vertexSet[0].z;
	for(int i=0; i<vertexCount; i++){
		if(res.xMin > vertexSet[i].x) res.xMin = vertexSet[i].x;
		if(res.xMax < vertexSet[i].x) res.xMax = vertexSet[i].x;
		
		if(res.yMin > vertexSet[i].y) res.yMin = vertexSet[i].y;
		if(res.yMax < vertexSet[i].y) res.yMax = vertexSet[i].y;

		if(res.zMin > vertexSet[i].z) res.zMin = vertexSet[i].z;
		if(res.zMax < vertexSet[i].z) res.zMax = vertexSet[i].z;

		res.xCnt += vertexSet[i].x;
		res.yCnt += vertexSet[i].y;
		res.zCnt += vertexSet[i].z;
	}
	res.xCnt /= vertexCount;
	res.yCnt /= vertexCount;
	res.zCnt /= vertexCount;

}

void Model::resize(){
	analyse();
	float fullMax = max(max(res.xMax-res.xMin,res.yMax-res.yMin),res.zMax-res.zMin);

	for(int i=0; i<vertexCount; i++){
		vertexSet[i].x /= fullMax;
		vertexSet[i].y /= fullMax;
		vertexSet[i].z /= fullMax;
	}
	analyse();

}

Analysis Model::getAnalysis(){
	return res;
}

void Model::calculateTexture(int type){
	if ( type == INS_FLAT ){
		for (int i=0; i< vertexCount; i++){
			vertexSet[i].xt = (vertexSet[i].x - res.xMin) / (res.xMax - res.xMin);
			vertexSet[i].yt = (-vertexSet[i].y + res.yMax) / (res.yMax - res.yMin);
		}
	}
	else if ( type == INS_SPHERE ){
		for (int i=0; i< vertexCount; i++){
			float actuals[3] = { vertexSet[i].x - res.xCnt, -vertexSet[i].y + res.yCnt, vertexSet[i].z - res.zCnt};
			vertexSet[i].xt = (atan2f(actuals[2], actuals[0]) + M_PI) / (2*M_PI);
			vertexSet[i].yt = (atan2f(actuals[2], actuals[1]) + M_PI) / (2*M_PI);
		}
	}
	else if ( type == INS_CYLINDER ){
		for (int i=0; i<vertexCount; i++){
			float actuals[3] = { vertexSet[i].x - res.xCnt, (-vertexSet[i].y + res.yMax) / (res.yMax - res.yMin), vertexSet[i].z - res.zCnt};
			vertexSet[i].xt = 5*(atan2f(actuals[2], actuals[0]) + M_PI)/(2*M_PI);
			vertexSet[i].yt = actuals[1];
		}
	}
}