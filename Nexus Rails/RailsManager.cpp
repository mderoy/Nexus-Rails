#include "RailsManager.h"
#include "Matrix.h"
#include "Root.h"
#include<iostream>
#include<fstream>
#include<math.h>
#include <GL/freeglut.h>
using namespace std;

RailsManager::RailsManager() {
	railPositions = 0;
	bsplinePositions = 0;
	railColors = 0;
	currentRail = 1;
	currentTime = 0;
	startTimes = 0;
	connections = 0;
	transitionPercent = 1.0;
	speed = 0.01;
	actors = 0;
	previousRail = 0;
	flickerFix = false;
	numMisses = 0;
}

void RailsManager::reloadRails() {
	ifstream myReadFile;
	myReadFile.open("Data/Rails.txt");
	char output[100];
	char output2[100];
	int iOutput = 0;
	if (myReadFile.is_open()) {
		myReadFile >> output >> numRails;
		if (railPositions != 0) {
			delete[] railPositions;
			delete[] bsplinePositions;
			delete[] railColors;
			delete[] startTimes;
			delete[] connections;
			delete actors;
		}
		railPositions = new vector<Vector3 *>[numRails];
		bsplinePositions = new vector<Vector3 *>[numRails];
		railColors = new Vector3*[numRails];
		startTimes = new int[numRails];
		connections = new vector<Connection>[10000];
		actors = new vector<Actor *>();
		for (int i=0; i<numRails; i++) {
			myReadFile >> output >> iOutput;
			myReadFile >> output >> startTimes[i];
			float color[3];
			float pos[3];
			pos[0] = 0;
			myReadFile >> color[0] >> color[1] >> color[2];
			railColors[i] = new Vector3(color[0], color[1], color[2]);
			while (pos[0] != -10000) {
				myReadFile >> pos[0] >> pos[1] >> pos[2];
				if (pos[0] != -10000) {
					Vector3 *vPos = new Vector3(pos[0], pos[1], pos[2]);
					railPositions[i].push_back(vPos);
				}
			}
		}
		int conn[3];
		conn[0] = 0;
		while (conn[0] != -10000) {
			myReadFile >> conn[0] >> conn[1] >> conn[2];
			if (conn[0] >= 0) {
				Connection connection;
				connection.first = conn[1];
				connection.second = conn[2];
				connections[conn[0]].push_back(connection);
			}
		}

		while (output[0] != 'x' || output2[0] != 'x') {
			float pos[3];
			float scale[3];
			float glow[3];
			myReadFile >> output >> output2 >> pos[0] >> pos[1] >> pos[2] >> scale[0] >> scale[1] >> scale[2] >> glow[0] >> glow[1] >> glow[2];
			if (output[0] != 'x' || output2[0] != 'x') {
				Actor *newActor = new Actor(new string(output), new string(output2));
				newActor->setTranslate(pos[0],pos[1],pos[2]);
				newActor->setScale(scale[0],scale[1],scale[2]);
				newActor->emission[0] = glow[0];
				newActor->emission[1] = glow[1];
				newActor->emission[2] = glow[2];
				actors->push_back(newActor);
			}
		}
		
		myReadFile.close();
	}
	loadRails();
}

void RailsManager::loadRails()
{
	for (int railID=0; railID<numRails; railID++) {
	int npts = railPositions[railID].size()-1;
	int nsegment = 32;
		//Vector3 pts[33];

		for(int startingPoint =0 ; startingPoint < npts;startingPoint++)
		{
			for(int i =0; i<=nsegment; i++)
			{
				if(startingPoint >= npts)
				{
					break;
				}
				float t = (float)i/nsegment;
				Vector3 pts = calculateSplinePoint(t,railID,startingPoint);
				Vector3* ptsptr = new Vector3(pts);
				if(pts[0]!=-3000)
				{
					//here
					bsplinePositions[railID].push_back(ptsptr);
				}
				//cout<<pts[i][0]<<" "<<pts[i][1]<<" "<<pts[i][2]<<endl;
			}
		}

	}
}
void RailsManager::drawRails(Camera *camera) {
	//int numSubDivides = 10.0;
	GLSLProgram *glslProgram = Root::shaderManager->getShader("Basic");
	//glLineWidth(10.0);
	//for (int i=0; i<numRails; i++) {
	//glslProgram->sendUniform("material.color", railColors[i][0][0],railColors[i][0][1],railColors[i][0][2]);

	//glBegin(GL_LINE_STRIP);
	//for (int x=0; x<railPositions[i].size()-1; x++) {
	//for (float p=0.0; p<1.0; p += 1.0/numSubDivides) {
	//Vector3 point = railPositions[i][x][0]*(1-p) + railPositions[i][x+1][0]*p;
	//glVertex3f(point[0],point[1],point[2]);
	//}
	//}
	//glEnd();
	glLineWidth(10.0);
	Vector3 camPos = camera->geteyeV();
	camPos[1]=0;
	/*for (int railID=0; railID<numRails; railID++) {
		glslProgram->sendUniform("material.color", railColors[railID][0][0],railColors[railID][0][1],railColors[railID][0][2]);
		glslProgram->sendUniform("material.emission", railColors[railID][0][0],railColors[railID][0][1],railColors[railID][0][2]);
		int npts = railPositions[railID].size()-1;
		int nsegment = 32;
		Vector3 pts[33];
		glBegin(GL_LINE_STRIP);
		for(int startingPoint =0 ; startingPoint < npts;startingPoint++)
		{
			for(int i =0; i<=nsegment; i++)
			{
				if(startingPoint >= npts)
				{
					break;
				}
				float t = (float)i/nsegment;
				pts[i] = calculateSplinePoint(t,railID,startingPoint);
				if(pts[i][0]!=-3000 && Vector3(camPos-pts[i]).length() < 150.0)
				{
					glVertex3f(pts[i][0],pts[i][1],pts[i][2]);
				}
				//cout<<pts[i][0]<<" "<<pts[i][1]<<" "<<pts[i][2]<<endl;
			}
		}
*/

		
		for(int railID =0;railID <numRails;railID++)
		{
			glslProgram->sendUniform("material.color", railColors[railID][0][0],railColors[railID][0][1],railColors[railID][0][2]);
			glslProgram->sendUniform("material.emission", railColors[railID][0][0],railColors[railID][0][1],railColors[railID][0][2]);
			glBegin(GL_LINE_STRIP);
			for(int i =0;i<bsplinePositions[railID].size()-1;i++)
			{
				if (Vector3(camPos-Vector3((bsplinePositions[railID][i])[0][0],(bsplinePositions[railID][i])[0][1],(bsplinePositions[railID][i])[0][2])).length() < 120.0) {
					glVertex3f((bsplinePositions[railID][i])[0][0],(bsplinePositions[railID][i])[0][1],(bsplinePositions[railID][i])[0][2]);
					//cout<<(bsplinePositions[railID][i])[0][0]<<endl;
				}
			}
			glEnd();
		}
		
		//glBegin(GL_LINE_STRIP);
		//for(int r =5;r<30;r++)
		//{
		//	glVertex3f(pts[r][0],pts[r][1],pts[r][2]);
		//}
		//glEnd();
	//}
	/*for (int railID=0; railID<numRails; railID++) {
		int npts = railPositions[railID].size()-1;
		for(int startingPoint = 0 ; startingPoint < npts;startingPoint++) {
			glslProgram->sendUniform("material.color", 1.0f, 1.0f, 1.0f);
			glslProgram->sendUniform("material.emission", 1.0f, 1.0f, 1.0f);
			glPointSize(25);
			glBegin(GL_POINTS);
				Vector3 pnt = calculateSplinePoint(0,railID,startingPoint);
				if (Vector3(camPos-pnt).length() < 150.0) {
					glVertex3f(pnt[0],pnt[1],pnt[2]);
				}
			glEnd();
		}
	}*/
/*
glMatrixMode(GL_PROJECTION);
glPushMatrix();
glLoadIdentity();
gluOrtho2D(0.0, 1280, 0.0, 720);

glMatrixMode(GL_MODELVIEW);
glPushMatrix();
glLoadIdentity();
glColor3f(0.0, 1.0, 0.0); // Green
glRasterPos2i(10, 10);

	void* font = GLUT_BITMAP_TIMES_ROMAN_24;
	glutBitmapString(font, (const unsigned char*)"TESTING");
glMatrixMode(GL_MODELVIEW);
glPopMatrix();

glMatrixMode(GL_PROJECTION);
glPopMatrix();
*/
}

void RailsManager::drawActors(Camera *camera, string shader) {
	Vector3 camPos = camera->geteyeV();
	camPos[1]=0;
	for (vector<Actor *>::iterator it = actors->begin(); it != actors->end(); it++) {
		Actor *actor = (*it);
		Root::ModelviewMatrix.push(Root::ModelviewMatrix.top());
			if (Vector3(camPos-actor->getTranslateV()).length() < 150.0) {
				actor->transformToMatrix(&Root::ModelviewMatrix.top());
				actor->drawActor(shader);
			}
		Root::ModelviewMatrix.pop();
	}
}

void RailsManager::drawGrade(Camera *camera, string shader) {

	Actor actor;
	if (numMisses <= 1) {
		actor = Actor(new string("A"),new string("Black"));
	} else if (numMisses <= 5) {
		actor = Actor(new string("B"),new string("Black"));
	} else if (numMisses <= 10) {
		actor = Actor(new string("C"),new string("Black"));
	} else {
		actor = Actor(new string("D"),new string("Black"));
	}
	actor.setTranslate(19,28.7,-105);
	actor.emission[0]=1.0;
	actor.emission[1]=1.0;
	Root::ModelviewMatrix.push(Root::ModelviewMatrix.top());
			actor.transformToMatrix(&Root::ModelviewMatrix.top());
			actor.drawActor(shader);

	Root::ModelviewMatrix.pop();
}

// railPositions: Rail, Segment, 0, xyz
// railColors: Rail, 0, rgb

void RailsManager::updateTime(Camera *camera, float dt) {
	currentTime += max(speed,0.0f);
	if (speed < 0.500)
		speed += dt*0.004;
	int currentSegment = (int)currentTime;
	if (transitionPercent > 0.99 && (Root::inputManager->isAnyKey())) {
		vector<Connection> segConnections = connections[currentSegment];
		for (vector<Connection>::iterator it = segConnections.begin(); it != segConnections.end(); it++) {
			if (it->first == currentRail) {
				Root::MUSICPLAYER->jumpSound();
				transitionPos = camera->geteyeV();
				previousRail = currentRail;
				currentRail = it->second;
				transitionPercent = 0.0f;
				flickerFix = true;
			} else if (it->second == currentRail) {
				Root::MUSICPLAYER->jumpSound();
				transitionPos = camera->geteyeV();
				previousRail = currentRail;
				currentRail = it->first;
				transitionPercent = 0.0f;
				flickerFix = true;
			}
		}
	}
	int localSegment = (int)currentTime - startTimes[currentRail];
	if (localSegment < 0) return;	
	if (localSegment >= railPositions[currentRail].size()-3) {	// crash into end
		if (currentRail == 18) { // last rail
			speed -= dt*0.02;
			Root::MUSICPLAYER->setSpeed(speed);
			if (Root::inputManager->isKeyDownOnce('r')) {
				currentRail = 0;
				currentTime = 0;
				speed = 0.01;
				numMisses = 0;
			}
			return;
		} else {
			currentTime = startTimes[currentRail];
			localSegment = 0;
			if (currentRail > 0) {
				numMisses++;
			}
			speed = max(0.01f,speed*(2.0f/3.0f));
			//Root::MIDIPLAYER->die();
			Root::MUSICPLAYER->hurtSound();
		}
	}
	float p = currentTime - (int)currentTime;
	Vector3 position = calculateSplinePoint(p, currentRail, localSegment);
	Vector3 futurePosition = calculateSplinePoint(p+0.01, currentRail, localSegment);
	futurePosition[1] += 0.95;
	if (position[0] == -3000) {
		return;
	}
	if (transitionPercent < 1.0) {
		transitionPercent += dt*5.0;
	} else {
		transitionPercent = 1.0;
	}
	position[1] += 1.0;
	Vector3 finalCamPos = position*transitionPercent + transitionPos*(1.0-transitionPercent);
	camera->setPosition(finalCamPos[0],finalCamPos[1],finalCamPos[2]);
	camera->recalculate();
	
	Vector3 look = futurePosition - position;
	look = look.normalize();
	//look[1] -= 0.5;
	//if (transitionPercent <= 0.999 || flickerFix) {
	//	look = lastLook;
	//	if (transitionPercent > 0.999) {
	//		flickerFix = false;
	//	}
	//}
	//look = look*transitionPercent + futurePosition*(1.0-transitionPercent);

	Vector3 camPos = camera->geteyeV();
	camera->setLookAt(camPos[0]+look[0], camPos[1]+look[1], camPos[2]+look[2]);
	camera->setUp(0,1.0,0);

	lastPosition = finalCamPos;
	lastLook = look;
	

	//Root::MIDIPLAYER->playRail(speed,currentRail);
	Root::MUSICPLAYER->setSpeed(speed);
}




Vector3 RailsManager::calculateSplinePoint(float t, int railID, int startingPoint)
{
	int npts = railPositions[railID].size()-1;

	Vector3 point0;
	Vector3 point1;
	Vector3 point2;
	Vector3 point3;

	if(startingPoint <= npts)
	{
		point0 = Vector3(railPositions[railID][startingPoint][0][0],railPositions[railID][startingPoint][0][1],railPositions[railID][startingPoint][0][2]);
	}
	else
	{
		return Vector3(-3000,0,0);
	}
	if(startingPoint+1 <= npts)
	{
		point1 = Vector3(railPositions[railID][startingPoint+1][0][0],railPositions[railID][startingPoint+1][0][1],railPositions[railID][startingPoint+1][0][2]);
	}
	else
	{
		return Vector3(-3000,0,0);
	}
	if(startingPoint+2 <= npts)
	{
		point2 = Vector3(railPositions[railID][startingPoint+2][0][0],railPositions[railID][startingPoint+2][0][1],railPositions[railID][startingPoint+2][0][2]);
	}
	else
	{
		return Vector3(-3000,0,0);
	}
	if(startingPoint+3 <= npts)
	{
		point3 = Vector3(railPositions[railID][startingPoint+3][0][0],railPositions[railID][startingPoint+3][0][1],railPositions[railID][startingPoint+3][0][2]);
	}
	else
	{
		return Vector3(-3000,0,0);
	}
	point0*=((float)(((float)1/6)*(pow(-t,3)+(3*pow(t,2))-(3*t)+1)));
	point1*=((float)(((float)1/6)*((3*pow(t,3))-(6*pow(t,2))+4)));
	point2*=((float)(((float)1/6)*((-3*pow(t,3))+(3*pow(t,2))+(3*t +1))));
	point3*=((float)(((float)1/6)*(pow(t,3))));

	Vector3 total = Vector3(0,0,0);
	total+=point0;
	total+=point1;
	total+=point2;
	total+=point3;
	return total;
}


Vector3 RailsManager::getRailColor() {
	return railColors[currentRail][0]*transitionPercent + railColors[previousRail][0]*(1.0-transitionPercent);
}
