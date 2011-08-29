/*
 *
 *  ofxBlur.h
 *	Communitas
 *
 *  Created by Patricio González Vivo on 05/07/10.
 *  Copyright 2011 PatricioGonzalezVivo.com. All rights reserved.
 *
 *
 *  This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0
 *	Unported License. To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/
 *	or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 *
 */

#pragma once
#include "ofMain.h"

class ofxBlur {
private:
	ofFbo ping, pong;
	ofShader horizontalBlur, verticalBlur;
	int passes;
	float radius;

public:
	void setup(int width, int height) {
		ping.allocate(width, height);
		pong.allocate(width, height);
		horizontalBlur.load("", "shaders/horizontalBlur.frag");
		verticalBlur.load("", "shaders/verticalBlur.frag");
	}

	void setPasses(int passes) {
		this->passes = passes;
	}
	void setRadius(float radius) {
		this->radius = radius;
	}

	void begin() {
		ofPushStyle();
		ofPushMatrix();
		pong.begin();
	}
	void end(bool draw = true) {
		pong.end();

		glColor4f(1, 1, 1, 1);
		for(int i = 0; i < passes; i++) {
			ping.begin();
			horizontalBlur.begin();
			horizontalBlur.setUniform1f("radius", radius);
			pong.draw(0, 0);
			horizontalBlur.end();
			ping.end();

			pong.begin();
			verticalBlur.begin();
			verticalBlur.setUniform1f("radius", radius);
			ping.draw(0, 0);
			verticalBlur.end();
			pong.end();
		}

		if(draw) {
			pong.draw(0, 0);
		}
		ofPopStyle();
		ofPopMatrix();
	}

	//ofTexture& getTexture() {return pong.getTexture(0);};
	void draw(){pong.draw(0, 0);}
};
