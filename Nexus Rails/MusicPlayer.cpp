#include <iostream>
#include <fstream>
#include <windows.h>
#include "MusicPlayer.h"
#include "audiere.h"
using namespace audiere;
using namespace std;

AudioDevicePtr* device;
OutputStreamPtr* music;
OutputStreamPtr* jump;
int MusicPlayer::init()
{	
	//init and play music
	string fileName="Data/awoefuldog.wav";
   device = new AudioDevicePtr(OpenDevice());
   music = new OutputStreamPtr(OpenSound(*device,fileName.c_str(), false));
   (*music)->play(); (*music)->setRepeat(true); (*music)->setVolume(1.0f);

	string jumpFile = "Data/Randomize7.wav";
     jump = new OutputStreamPtr(OpenSound(*device,jumpFile.c_str(), false));
   return 0;
}
float transition =0.0f;
void MusicPlayer::setSpeed(float speed)
{
	if(speed >= .034 && transition < 1)
	{
		(*music)->setPitchShift(1+transition);
		transition = transition+0.009f;
		if(transition >=1)
		{
		(*music)->setPitchShift(2);
		}
	}
	else if(speed < .035)
	{
		(*music)->setPitchShift(1);
		transition = 0.0f;
	}
	//(*music)->setPitchShift(speed*100);
}

void MusicPlayer::jumpSound()
{
	if((*jump)->isPlaying())
	{
		(*jump)->stop();
		(*jump)->reset();
	}
	(*jump)->play(); (*jump)->setRepeat(false); (*jump)->setVolume(1.0f);
}