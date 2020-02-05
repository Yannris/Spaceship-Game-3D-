#pragma once

#include "ofMain.h"
#include  "ofxAssimpModelLoader.h"
#include "box.h"
#include "ray.h"
#include "Octree.h"
#include "ParticleSystem.h"
#include "ParticleEmitter.h"
class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void drawAxis(ofVec3f);
		void initLightingAndMaterials();
		void savePicture();
		void toggleWireframeMode();
		void togglePointsDisplay();
		void toggleSelectTerrain();
		void setCameraTarget();
		bool  doPointSelection();
		void drawBox(const Box &box);
        void collisionDtect();
        void AGL();

		Box meshBounds(const ofMesh &);
		void subDivideBox8(const Box &b, vector<Box> & boxList);

		bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);

		ofEasyCam cam;
		ofxAssimpModelLoader mars, rover;
		ofLight light;
    
		Box boundingBoxMoon;
        Box boundingBoxRover;

		vector<Box> level1, level2, level3;
	
		bool bAltKeyDown;
		bool bCtrlKeyDown;
		bool bWireframe;
		bool bDisplayPoints;
		bool bPointSelected;
		
		bool bRoverLoaded;
		bool bTerrainSelected;
	
		ofVec3f selectedPoint;
		ofVec3f intersectPoint;


		const float selectionRange = 4.0;
        // added here
    float Height;
    Octree octree;
    ofVec3f contactPt;
    bool rayHit;
    bool bCollision = false;
    int levelOfOctree;
    // rover with particle sys
    ParticleSystem Sys = ParticleSystem();
    Particle particle = Particle();
    
    GravityForce * g = new GravityForce(ofVec3f(0, -9.8, 0));
    pushForce * p = new pushForce();
    TurbulenceForce * t = new TurbulenceForce(ofVec3f(-1.5, -1.5, -1.5), ofVec3f(2.5, 2.5, 2.5));
    //change it to non one time force
    RadialForce * radialForce = new RadialForce();
    ParticleEmitter emitter = ParticleEmitter();
    //sound
    ofSoundPlayer background;
    ofSoundPlayer fire;
    ofSoundPlayer hit;

    //cam
    
    ofEasyCam mainCam;
    ofCamera cam1, cam2, cam3;
    ofCamera *theCam;
    //light
    ofLight keyLight, fillLight, rimLight;
};
