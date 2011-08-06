/*
 *
 *  testApp.h
 *	Communitas
 *
 *  Created by Patricio Gonz‡lez Vivo on 05/07/10.
 *  Copyright 2011 PatricioGonzalezVivo.com. All rights reserved.
 *
 *
 *  This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 
 *	Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ 
 *	or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 *
 */

#ifndef _TESTAPP
#define _TESTAPP

#include "ofMain.h"
#include "ofxSpace.h"
#include "ofxParticle.h"
#include "ofxBlur.h"

#include "ofxTuio.h"
#include "ofxOsc.h"

#define PORT 12345

class testApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void mouseDragged(int x, int y, int button);
	void windowResized(int w, int h);
	
	void addParticle(int x, int y, int _n);
	void insertParticle(int x, int y, int _n, int _hue);
	void killParticle(int _n);
	
	void	tuioAdded(ofxTuioCursor & tuioCursor);
	void	tuioRemoved(ofxTuioCursor & tuioCursor);
	void	tuioUpdated(ofxTuioCursor & tuioCursor);
	
	myTuioClient	tuioClient;
	ofxOscReceiver	receiver;
	
	ofxSpace	universe;
	ofxSpace	nucle;
	
	vector<ofxParticle *> pAct;
	vector<ofxParticle *> pInact;
	
	ofxBlur blur;
	int		screenWidth,screenHeight;
	int		fboAlpha;
	
	float	timer;
	
	float	standBy;
	float	delay;
	
	bool	drawSpaceBorders;
	bool	interact;
};

#endif
