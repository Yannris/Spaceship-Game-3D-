
// Kevin M.Smith - CS 134 SJSU

#include "ParticleSystem.h"

void ParticleSystem::add(const Particle &p) {
	particles.push_back(p);
}

void ParticleSystem::addForce(ParticleForce *f) {
	forces.push_back(f);
}

void ParticleSystem::remove(int i) {
	particles.erase(particles.begin() + i);
}

void ParticleSystem::setLifespan(float l) {
	for (int i = 0; i < particles.size(); i++) {
		particles[i].lifespan = l;
	}
}

void ParticleSystem::reset() {
	for (int i = 0; i < forces.size(); i++) {
		forces[i]->applied = false;
	}
}

void ParticleSystem::update() {
	// check if empty and just return
	if (particles.size() == 0) return;
	vector<Particle>::iterator p = particles.begin();
	vector<Particle>::iterator tmp;

	// check which particles have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, we need to use an iterator.
	//
	while (p != particles.end()) {
		if (p->lifespan != -1 && p->age() > p->lifespan) {
			tmp = particles.erase(p);
			p = tmp;
		}
		else p++;
	}

	// update forces on all particles first 
	//
	for (int i = 0; i < particles.size(); i++) {
		for (int k = 0; k < forces.size(); k++) {
			if (!forces[k]->applied)
				forces[k]->updateForce( &particles[i] );
		}
	}

	// update all forces only applied once to "applied"
	// so they are not applied again.
	//
	for (int i = 0; i < forces.size(); i++) {
		if (forces[i]->applyOnce)
			forces[i]->applied = true;
	}

	// integrate all the particles in the store
	//
	for (int i = 0; i < particles.size(); i++)
		particles[i].integrate();

}

// remove all particlies within "dist" of point (not implemented as yet)
//
int ParticleSystem::removeNear(const ofVec3f & point, float dist) { return 0; }

//  draw the particle cloud
//
void ParticleSystem::draw() {
	for (int i = 0; i < particles.size(); i++) {
		particles[i].draw();
	}
}

// push Force Field
void pushForce::updateForce(Particle * particle)
{
    if (up) {
        this->add(ofVec3f(0, F, 0));
    }
    else if (down) {
        this->add(ofVec3f(0, -F, 0));
    }
    else if (left) {
        this->add(ofVec3f(-F, 0, 0));
    }
    else if (right) {
        this->add(ofVec3f(F, 0, 0));
    }
    else if (forward) {
        
        this->add(ofVec3f(0, 0, F));
    }
    else if (back) {
        this->add(ofVec3f(0, 0, -F));
    }
    
    particle->forces += force;
}
// Gravity Force Field 
//
GravityForce::GravityForce(const ofVec3f &g) {
	gravity = g;
}

void GravityForce::updateForce(Particle * particle) {
	//
	// f = mg
	//
	particle->forces += gravity * particle->mass;
}

// Turbulence Force Field 
//
TurbulenceForce::TurbulenceForce(const ofVec3f &min, const ofVec3f &max) {
	tmin = min;
	tmax = max;
}

void TurbulenceForce::updateForce(Particle * particle) {
	//
	// We are going to add a little "noise" to a particles
	// forces to achieve a more natual look to the motion
	//
	particle->forces.x += ofRandom(tmin.x, tmax.x);
	particle->forces.y += ofRandom(tmin.y, tmax.y);
	particle->forces.z += ofRandom(tmin.z, tmax.z);
}

// Impulse Radial Force - this is a "one shot" force that
// eminates radially outward in random directions.
//
RadialForce::RadialForce(float magnitude)
{
	this->magnitude = magnitude;
	applyOnce = true;
}

void RadialForce::updateForce(Particle * particle) {

	// we basically create a random direction for each particle
	// the force is only added once after it is triggered.
	//
	ofVec3f dir = ofVec3f(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1));
	particle->forces += dir.getNormalized() * magnitude;
}

// Impulse Radial Force - this is a "one shot" force that
// eminates radially outward in x and y  directions.
//
RingRadialForce::RingRadialForce(float RingMagnitude) {
    this->RingMagnitude = RingMagnitude;
    applyOnce = true;
}

void RingRadialForce::updateForce(Particle * particle) {
    
    // we basically create a random direction for each particle
    // the force is only added once after it is triggered.
    //
    ofVec3f dir = ofVec3f(ofRandom(-1, 1), (0,0),  ofRandom(-1, 1));
    particle->forces += dir.getNormalized() * RingMagnitude;
}

SpinForce::SpinForce(float RingMagnitude) {
    
    this->spinMagnitude = spinMagnitude;
    applyOnce = true;
}

void SpinForce::updateForce(Particle * particle) {
    
    // we basically create a random direction for each particle
    // the force is only added once after it is triggered.
    //
    /*
     ofVec2f dir = ofVec2f(particle->forces.x, particle->forces.z);
    ofVec2f pendir = dir.perpendicular();
    ofVec3f Pendir = ofVec3f(pendir.x, 0,pendir.y);
    particle->forces += Pendir*5000;
*/
    particle->velocity.rotate(30, ofVec3f(0, 1, 0)) ;
}
