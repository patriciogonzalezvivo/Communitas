/*
 *
 *  testApp.cpp
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

#include "testApp.h"

//--------------------------------------------------------------

void testApp::setup(){
	ofSetFrameRate(60);
    ofBackground(0, 0, 0);
	ofEnableAlphaBlending();
	//ofSetBackgroundAuto(true);
	
	screenWidth = 1280;//ofGetScreenWidth()*1.5;
	screenHeight = 960;//ofGetScreenHeight()*1.5;
	
	blur.setup(screenWidth, screenHeight);
	blur.setRadius(3);
	blur.setPasses(1);
	
	receiver.setup( PORT );
	
	timer				= 0;
	standBy				= 30;
	drawSpaceBorders	= false;
	interact			= true;
	
	universe.findCenter(screenWidth,screenHeight);
	universe.mass			= 0.75;
	universe.vortex			= screenHeight*0.1;
	universe.maxDistance	= screenHeight*0.5;
	
	//universe.density		= 0.4;
	universe.ph				= 6.5;
	universe.joinParticles	= true;
	universe.correctOrbits	= true;
	universe.proportional	= true;
	universe.clockwise		= true;
	
	//universe.impulse		= 0.4;
	universe.angle			= 0;
	
	nucle.findCenter(screenWidth,screenHeight);
	nucle.mass				= 10;
	nucle.vortex			= screenHeight*0.1;
	nucle.maxDistance		= screenHeight*0.1;
	
	nucle.density			= 1;	//= 0.05;
	nucle.ph				= 7;	//= 6.9;
	nucle.joinParticles		= true;
	
	nucle.correctOrbits		= false;// = true;
	nucle.proportional		= false;
	nucle.clockwise			= false;
	nucle.impulse			= 0.19;
	nucle.angle				= 0;	//= -25;
	
	ofAddListener(tuioClient.cursorAdded,this,&testApp::tuioAdded);
	ofAddListener(tuioClient.cursorRemoved,this,&testApp::tuioRemoved);
	ofAddListener(tuioClient.cursorUpdated,this,&testApp::tuioUpdated);
	tuioClient.start(3333);
}

//--------------------------------------------------------------
void testApp::update(){
	
	while( receiver.hasWaitingMessages() ){
		ofxOscMessage m;
		receiver.getNextMessage( &m );
		
		if( m.getAddress() == "/particle/new" ){
			int x = m.getArgAsInt32( 0 );
			int y = m.getArgAsInt32( 1 );
			int n = m.getArgAsInt32( 2 );
			int h = m.getArgAsInt32( 3 );
			insertParticle(x, y, n, h);
		}
	}
	
	tuioClient.getMessage();
	
	//-------------------------------
	
	if (timer <= standBy){
		nucle.mass				= 10;
		nucle.maxDistance		= screenHeight*0.07;
		
		if ( timer >= (standBy-standBy/3)){
			universe.mass		= 10;
			universe.proportional= false;
			universe.clockwise	= false;
			universe.impulse	= timer - (standBy - standBy/3);
			universe.angle		= -40;
			universe.density	= 0.8;
			blur.setRadius(1);
			fboAlpha			= 200;
		} else {
			universe.mass		= 0.75;
			universe.proportional= true;
			universe.clockwise	= true;
			universe.impulse	= 0.3;
			universe.angle		= 0;
			universe.density	= 0.7;
			blur.setRadius(0.5);
			fboAlpha			= 150;
		}
	} else {
		nucle.maxDistance		= screenHeight*0.01;
		nucle.mass				= 0.7;
		blur.setRadius(2);
		fboAlpha				= 255;
	}
	
	for (int i=0; i < pAct.size() ; i++) 
		if (universe.onSpace(pAct[i]->loc) && (pAct[i]->alpha > 5) ){
			pAct[i]->update();
			
			if (nucle.onSpace(pAct[i]->loc)) 
				if (pAct[i]->bArrive)
					killParticle(i);
			
		} else killParticle(i);
    
	
	for (int i=0; i < pInact.size() ; i++)
		if (!universe.onSpace(pInact[i]->loc) || (pInact[i]->alpha < 5) )
			pInact.erase(pInact.begin()+i);
		else pInact[i]->update();
	
	timer += 1/ofGetFrameRate();
}

//--------------------------------------------------------------
void testApp::draw(){
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	blur.begin();
	for (int i = pInact.size()-1; i > 0 ; i--)
		pInact[i]->draw();
	blur.end(false);
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofSetColor(fboAlpha, fboAlpha, fboAlpha,fboAlpha);
	blur.draw();
	
	for (int i = pInact.size()-1; i > 0 ; i--)
		pInact[i]->draw();
	
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofSetColor(255, 255, 255);
	for (int i = pAct.size()-1; i > 0 ; i--)
		pAct[i]->draw();
		
	
	if (drawSpaceBorders){
		universe.drawBorder();
		nucle.drawBorder();
	}
}

//----------------------------------------------------------- Agrega una partÃ­cula

void testApp::addParticle(int x, int y, int _n) {
	if ( universe.onSpace( ofVec2f(x,y) ) ){
		timer = 0;
		
		int nLnk = 0;
		for (int i = pAct.size()-1; i >= 0 ; i--){
			if (nLnk == 0) 
				if (pAct[i]->idN == _n) 
					nLnk = i;
			
			if ( ofVec2f(x,y).distance(pAct[i]->loc) <= 40) 
				pAct[i]->alpha = 255;
		}
	
		ofxParticle * obj = new ofxParticle(_n,&universe,ofVec2f(x,y));
	
		if ( nLnk > 0) obj->setLnk(pAct[nLnk]);
	
		pAct.push_back(obj);
	}
}

void testApp::insertParticle(int x, int y, int _n, int _hue = -1) {
	if ( universe.onSpace( ofVec2f(x,y) + universe.center ) ){
		timer = 0;
		ofxParticle * obj = new ofxParticle(_n,&universe,ofVec2f(x,y)+universe.center,true);
		
		int nLnk = 0;
		for (int i = 0; i < pAct.size() ; i++){
			if (nLnk == 0) {
				if (pAct[i]->idN == _n-1) 
					nLnk = i;
				else if (pAct[i]->idN == _n+1) 
					pAct[i]->setLnk(obj);
			}
			
			if ( ofVec2f(x,y).distance(pAct[i]->loc) <= 40) 
				pAct[i]->alpha = 255;
		}
		
		if ( nLnk > 0) obj->setLnk(pAct[nLnk]);
		if ( _hue != -1) obj->setColor(_hue);
		
		pAct.push_back(obj);
	}
}

void testApp::killParticle(int _n){
	pAct[_n]->setSpace(&nucle);
	pAct[_n]->vel = pAct[_n]->getOrbitalForce(1);
	pAct[_n]->alpha /= 1.5;
	if (pInact.size() > 2000) pInact.erase(pInact.begin());
	pInact.push_back(pAct[_n]);
	pAct.erase(pAct.begin()+_n);
}

//----------------------------------------------------------- EVENTOS
void testApp::keyPressed(int key){
	switch(key) {
		case 'f': ofToggleFullscreen(); break;
		case 'x': drawSpaceBorders = !drawSpaceBorders; break;
		case 's': timer = standBy - standBy/3; break;
		case 'c':
			blur.begin();
			ofClear(0, 0, 0, 255);
			blur.end(false);
		break;
		case OF_KEY_LEFT: universe.maxDistance++; break;
		case OF_KEY_RIGHT: universe.maxDistance--; break;
	}
}

void testApp::mouseDragged(int x, int y, int button){
}

void testApp::windowResized(int w, int h){
	blur.setup(screenWidth, screenHeight);
	blur.begin();
	ofClear(0, 0, 0, 255);
	blur.end(false);
	
	universe.findCenter(screenWidth,screenHeight);
	nucle.findCenter(screenWidth,screenHeight);
}

void testApp::tuioAdded(ofxTuioCursor & tuioCursor){
	addParticle(tuioCursor.getX()*screenWidth,tuioCursor.getY()*screenHeight,tuioCursor.getSessionId());
}

void testApp::tuioUpdated(ofxTuioCursor &tuioCursor){
	addParticle(tuioCursor.getX()*screenWidth,tuioCursor.getY()*screenHeight,tuioCursor.getSessionId());
}

void testApp::tuioRemoved(ofxTuioCursor & tuioCursor){
}