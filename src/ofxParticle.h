/*
 *
 *  ofxParticle.cpp
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

#ifndef _OFXPARTICLE
#define _OFXPARTICLE

#include "ofMain.h"
#include "ofxSpace.h"

class ofxParticle {
public:
	ofxParticle(){
	    idN = 0;
		lnk = NULL;
		color.set(255,255,255,255);

		acc.set(0,0);
		vel.set(0,0);
		mass = 1;
		render_radio = radio = 4;
		alpha = 255;
	};

	ofxParticle(int _idN, const ofxSpace *toSpace, ofVec2f _loc, bool _fromCenter){
		idN = _idN;
		lnk = NULL;
		setSpace(toSpace);
		setColor(_loc);

		if (_fromCenter){
			loc = space->center;
			sLoc.set(_loc);
			bArrive = false;
		} else {
			loc.set(_loc);
			setColor(_loc);
			bArrive = true;
			save();
		}

		acc.set(0,0);
		vel.set(0,0);
		mass = 1;
		render_radio = radio = 4;
		alpha = 255;
	};

	~ofxParticle(){

	};


	ofxParticle& save(){sLoc.set(loc); return * this;};
	ofxParticle& setSpace(const ofxSpace *toSpace)	{ space = toSpace; return * this;};
	ofxParticle& setLnk(const ofxParticle *toLnk)	{ lnk = toLnk; return * this;};
	ofxParticle& setColor(ofVec2f _loc){
	    ofVec2f l = _loc - space->center;
	    color.setHsb((ofRadToDeg(atan2(l.x,l.y)+PI)/360.0f)*255, 255, 255);
	    return * this;
	    };
	ofxParticle& setColor(int _hue){ color.setHsb(_hue, 255, 255); return * this; };
	ofxParticle& addOrbitalForce(float _force){	applyForce(getOrbitalForce(_force)); return * this;};
	ofxParticle& applyForce(ofVec2f _force){
		_force /= mass;
		acc += _force;
		return * this;
	};

	int getColor(){ return color.getHue();};
	ofVec2f steer(ofVec2f target, bool arrival = true) {
		ofVec2f steerForce;

		if (arrival){
			ofVec2f desired = target - loc;  // A vector pointing from the location to the target
			float d = desired.length(); // Distance from the target is the magnitude of the vector
			// If the distance is greater than 0, calc steering (otherwise return zero vector)
			if (d > 0) {
				// Normalize desired
				desired.normalize();
				// Two options for desired vector magnitude (1 -- based on distance, 2 -- maxspeed)
				if (d < 1)
					desired *= (d); // This damping is somewhat arbitrary
				//else
				//desired *= 2;
				// Steering = Desired minus Velocity
				steerForce = desired - vel; //PVector.sub(desired,vel);
				//steer.limit(0.9);  // Limit to maximum steering force
			} else
				steerForce = ofVec2f(0,0);

		} else {
			steerForce = target - loc;
			//steer.limit(maxforce);
		}

		return steerForce;
	}

	ofVec2f	getOrbitalForce(float _force){
		ofVec2f toCenter = space->center;
		toCenter -= loc;

		ofVec2f toCenterOFX(toCenter.x,toCenter.y);
		toCenterOFX.perpendicular();
		toCenterOFX.normalize();

		if ( space->proportional ) toCenterOFX *= loc.distance(space->center)/(10000*_force);
		else toCenterOFX *= _force;

		toCenterOFX.rotate(space->angle);

		if (space->clockwise) toCenterOFX *= -1;

		return ofVec2f(toCenterOFX.x,toCenterOFX.y);
	};

	void update(){
		if (bArrive){
			if (space->correctOrbits) addOrbitalForce(space->impulse);
			ofVec2f direction = space->center - loc;	// Primero calcula la dirección al centro del espacio que lo rije
			float d = direction.length();				// De allí calcula la fuerza con que lo atrae
			d = ofClamp(d,5.0f,25.0f);
			float force = (9.8 * space->mass * mass) / (d * d);
			direction.normalize();						// Vuelve la dirección a un parámetro entre 0 y 1;
			applyForce(direction * force);				// De la multiplicación del vector con la dirección con la fuerza de atracción la suma al desplazamiento
		} else {
			if (sLoc != loc)
				applyForce(steer(sLoc));
			else
				bArrive = true;
		}

		//Update en si mismo
		vel += acc;
		loc += vel;
		vel *= space->density;
		acc *= 0;

		float distance = loc.distance(space->center);
		if ( distance <= space->vortex)
			render_radio = ofMap(distance,0,(float)space->vortex,0.0f,radio, true);
		else
			render_radio = radio;

		alpha += (space->ph/10-0.7);
		if (alpha >= 1) render_radio = render_radio * alpha/255;
		color.a = alpha;
	};

	void draw(){
		ofSetColor( color );

		if ( space->joinParticles ){
			if ( lnk != NULL )
				if ( ( lnk->space->joinParticles) && (lnk->loc.distance(loc) < 40)){
					ofSetLineWidth(render_radio*2);
					ofLine(loc.x,loc.y,lnk->loc.x,lnk->loc.y);
				} else lnk = NULL;
		}
		ofCircle(loc.x,loc.y,render_radio*0.6);
	};

	ofColor	color;
	ofVec2f	loc,acc,vel,sLoc;
	float	mass, radio, render_radio, alpha;
	int		idN;
	bool	bArrive;

private:
	const	ofxSpace * space;
	const	ofxParticle * lnk;
};
#endif
