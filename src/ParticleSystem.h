#pragma once
//  Kevin M. Smith - CS 134 SJSU

#include "ofMain.h"
#include "Particle.h"


//  Pure Virtual Function Class - must be subclassed to create new forces.
//
class ParticleForce {
protected:
public:
	bool applyOnce = false;
	bool applied = false;
	virtual void updateForce(Particle *) = 0;
};

class ParticleSystem {
public:
	void add(const Particle &);
	void addForce(ParticleForce *);
	void remove(int);
	void update();
	void setLifespan(float);
	void reset();
	int removeNear(const ofVec3f & point, float dist);
	void draw();
	vector<Particle> particles;
	vector<ParticleForce *> forces;
};



// Some convenient built-in forces
//

class pushForce : public ParticleForce
{
public:
    ofVec3f force;
    float F = 0.5;
    bool up = false;
   bool down = false;
   bool left = false;
   bool right = false;
   bool forward = false;
   bool back = false;
    void updateForce(Particle *);
    void add(const ofVec3f addforce)
    {force += addforce;}
    pushForce(){ofVec3f(0, 0, 0);}
    void clear(){force = ofVec3f(0, 0, 0); }
};


class GravityForce: public ParticleForce {
	ofVec3f gravity;
public:
	GravityForce(const ofVec3f & gravity);
	void updateForce(Particle *);
};

class TurbulenceForce : public ParticleForce {
	ofVec3f tmin, tmax;
public:
	TurbulenceForce(const ofVec3f & min, const ofVec3f &max);
	void updateForce(Particle *);
};

class RadialForce : public ParticleForce
{
	
public:
    float magnitude = 12;
    float height = 0.2;
	RadialForce(float magnitude);
    RadialForce() {};
	void updateForce(Particle *);
};

class RingRadialForce : public ParticleForce {
    float RingMagnitude;
public:
    RingRadialForce(float RingMagnitude);
    void updateForce(Particle *);
};


class SpinForce : public ParticleForce {
    float spinMagnitude;
public:
    SpinForce(float spinMagnitude);
    void updateForce(Particle *);
};
