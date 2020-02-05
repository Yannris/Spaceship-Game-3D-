
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Mars HiRise Project - startup scene
// 
//  This is an openFrameworks 3D scene that includes an EasyCam
//  and example 3D geometry which I have reconstructed from Mars
//  HiRis photographs taken the Mars Reconnaisance Orbiter
//
//  You will use this source file (and include file) as a starting point
//  to implement assignment 5  (Parts I and II)
//
//  Please do not modify any of the keymappings.  I would like 
//  the input interface to be the same for each student's 
//  work.  Please also add your name/date below.

//  Please document/comment all of your work !
//  Have Fun !!
//
//  Student Name:   < Your Name goes Here >
//  Date: <date of last version>


#include "ofApp.h"
#include "Util.h"



//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup(){

	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bRoverLoaded = false;
	bTerrainSelected = true;
//	ofSetWindowShape(1024, 768);
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	cam.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();

	// setup rudimentary lighting 
	//
	initLightingAndMaterials();
    //set camera
    
    mainCam.setDistance(30);
    theCam = &mainCam;

    
    cam1.setGlobalPosition(glm::vec3(100, 0, 0));
    cam1.lookAt(glm::vec3(0, 0, 0));
    cam3.setGlobalPosition(glm::vec3(100, 0, 0));
    cam3.lookAt(glm::vec3(0, 0, 0));
    
    cam2.setGlobalPosition(glm::vec3(0, 150, 0));
    cam2.lookAt(glm::vec3(0, 0, 0));
    // lighting
    keyLight.setup();
    keyLight.enable();
    keyLight.setAreaLight(1, 1);
    keyLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
    keyLight.setDiffuseColor(ofFloatColor(1, 1, 1));
    keyLight.setSpecularColor(ofFloatColor(1, 1, 1));
    
    keyLight.rotate(45, ofVec3f(0, 1, 0));
    keyLight.rotate(-45, ofVec3f(1, 0, 0));
    keyLight.setPosition(5, 5, 5);
    /*
    fillLight.setup();
    fillLight.enable();
    fillLight.setSpotlight();
    fillLight.setScale(.05);
    fillLight.setSpotlightCutOff(15);
    fillLight.setAttenuation(2, .001, .001);
    fillLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
    fillLight.setDiffuseColor(ofFloatColor(1, 1, 1));
    fillLight.setSpecularColor(ofFloatColor(1, 1, 1));
    fillLight.rotate(-10, ofVec3f(1, 0, 0));
    fillLight.rotate(-45, ofVec3f(0, 1, 0));
    fillLight.setPosition(-5, 5, 5);
    
    rimLight.setup();
    rimLight.enable();
    rimLight.setSpotlight();
    rimLight.setScale(.05);
    rimLight.setSpotlightCutOff(30);
    rimLight.setAttenuation(.2, .001, .001);
    rimLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
    rimLight.setDiffuseColor(ofFloatColor(1, 1, 1));
    rimLight.setSpecularColor(ofFloatColor(1, 1, 1));
    rimLight.rotate(180, ofVec3f(0, 1, 0));
    rimLight.setPosition(0, 5, -7);
    */
    
	mars.loadModel("geo/moon-tri.obj");
	mars.setScaleNormalization(false);
    mars.setRotation(0, 180, 0, 0, 1);
    // lander model
    rover.loadModel("geo/lander.obj");
    rover.setScaleNormalization(false);
    rover.setPosition(0, 10, 0);
    rover.setRotation(0, 180, 0, 0, 1);
    bRoverLoaded = true;
    
	boundingBoxMoon = meshBounds(mars.getMesh(0));
    ofVec3f min = rover.getSceneMin() + rover.getPosition();
    ofVec3f max = rover.getSceneMax() + rover.getPosition();
    boundingBoxRover = Box (Vector3(min.x,min.y,min.z),Vector3(max.x,max.y,max.z));
	//  Test Box Subdivide
	//
	subDivideBox8(boundingBoxMoon, level1);
	subDivideBox8(level1[0], level2);
	subDivideBox8(level2[0], level3);
    
    // add octree for levelOfOctree levels
    levelOfOctree = 4;
    octree.create(mars.getMesh(0), levelOfOctree);
    
    // add particle system
    Sys.add(particle);
    Sys.addForce(t);
    Sys.addForce(p);
    Sys.particles[0].position.y = 10;
    emitter.init();
    emitter.sys->addForce(new GravityForce(ofVec3f(0, -9.8, 0)));
    emitter.setLifespan(0.2);
    emitter.sys->addForce(radialForce);
    
    emitter.setVelocity(ofVec3f(0, 0, 0));
    emitter.setOneShot(true);
    emitter.setEmitterType(RadialEmitter);
    emitter.setGroupSize(500);
    //music or sound
    fire.load("fire.wav");
    fire.setMultiPlay(true);
    background.load("background.mp3");
    background.play();
    background.setLoop(true);
    hit.load("hit.wav");
    Height = 0;
    
}

//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update()
{
    // undate the location for rover base on the paritcle system
    Sys.update();
    ofVec3f location = Sys.particles[0].position;
    rover.setPosition(location.x, location.y, location.z);
    emitter.setPosition(location);
    emitter.update();
    collisionDtect();
    AGL();
    //cam
    cam1.lookAt(glm::vec3(location.x, -location.y, location.z));
    cam1.setPosition(location);
    cam2.lookAt(location);
    cam2.setPosition(50,10,50);
    cam3.lookAt(glm::vec3(-location.x, location.y, location.z));
    cam3.setPosition(location);
  


    
	
}
//--------------------------------------------------------------
void ofApp::draw(){

//	ofBackgroundGradient(ofColor(20), ofColor(0));   // pick your own backgroujnd
	ofBackground(ofColor::black);
//	cout << ofGetFrameRate() << endl;
    string AGLString = "AGL: " + std::to_string(Height);
       ofDrawBitmapString(AGLString,10,50);

    theCam->begin();
	ofPushMatrix();
	if (bWireframe)
    {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		mars.drawWireframe();
		if (bRoverLoaded) {
			rover.drawWireframe();
			if (!bTerrainSelected) drawAxis(rover.getPosition());
		}
		if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
	}
	else {
		ofEnableLighting();              // shaded mode
		mars.drawFaces();

		if (bRoverLoaded) {
			rover.drawFaces();
			if (!bTerrainSelected) drawAxis(rover.getPosition());
		}
		if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
	}


	if (bDisplayPoints)
    {                // display points as an option
		glPointSize(3);
		ofSetColor(ofColor::green);
		mars.drawVertices();
	}

	// highlight selected point (draw sphere around selected point)
	//
	if (bPointSelected) {
		ofSetColor(ofColor::blue);
		ofDrawSphere(selectedPoint, .1);
	}
    // added
    // if ray hit
    if(rayHit){
        ofSetColor(ofColor::black);
        ofDrawSphere(intersectPoint, .1);
    }
	
	ofNoFill();
	ofSetColor(ofColor::white);
	//drawBox(boundingBoxMoonMoon);

	// Some debug code to test subdivideBox8
	//

	/*ofSetColor(ofColor::red);
	for (int i=0; i < level1.size(); i++)
		drawBox(level1[i]);

	ofSetColor(ofColor::blue);
	for (int i = 0; i < level2.size(); i++)
		drawBox(level2[i]);

	ofSetColor(ofColor::yellow);
	for (int i = 0; i < level3.size(); i++)
		drawBox(level3[i]);*/

//new added here
   // octree.draw(octree.root, levelOfOctree, 0);
    emitter.draw();
    //mainCam.draw();
    //cam1.draw();
    //cam2.draw();


    ofDrawBox(0, 0, 0, 20, 0, 20);
    ofPopMatrix();
	theCam->end();
}

// 

// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {

	switch (key) {
	case 'C':
	case 'c':
		if (mainCam.getMouseInputEnabled()) mainCam.disableMouseInput();
		else mainCam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		break;
        case 'P':
        case 'p':
           
            


            break;
	case 'r':
		cam.reset();
		break;
	case 's':
		savePicture();
		break;
	case 't':
		setCameraTarget();
		break;
	case 'u':
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case 'w':
		toggleWireframeMode();
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	default:
		break;
    // control rover
        case OF_KEY_UP:
            p->forward = true;
            break;
        case OF_KEY_DOWN:
            p->back = true;
            break;
        case OF_KEY_LEFT:
            p->left = true;

            break;
        case OF_KEY_RIGHT:
            p->right = true;

            break;
        case OF_KEY_PAGE_UP:
            p->up = true;
            emitter.start();
            fire.play();

            break;
        case OF_KEY_PAGE_DOWN:
            p->down = true;
            break;
        case OF_KEY_F1:
            theCam = &mainCam;
            break;
        case OF_KEY_F2:
            theCam = &cam1;
            break;
        case OF_KEY_F3:
            theCam = &cam2;
            break;
        case OF_KEY_F4:
            theCam = &cam3;
            break;
	}
}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {
	
	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	default:
		break;
            // control rover
        case OF_KEY_UP:
            p->forward = false;
            p->clear();

            break;
        case OF_KEY_DOWN:
            p->back = false;
            p->clear();

            break;
        case OF_KEY_LEFT:
            p->left = false;
            p->clear();

            break;
        case OF_KEY_RIGHT:
            p->right = false;
            p->clear();

            break;
        case OF_KEY_PAGE_UP:
            p->up = false;
            p->clear();
            emitter.stop();
            fire.stop();
            fire.setPaused(true);
            break;
        case OF_KEY_PAGE_DOWN:
            p->down = false;
            p->clear();
            break;
            
    
            
	}
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));
	//if (level3[1].intersect(ray, -1000, 1000)) cout << "intersects" << endl;
    
    // new added here
    // calculate select time
    float startTime = ofGetElapsedTimeMillis();
    //check if intersect
    
    if (octree.intersect(ray, octree.root, octree.selectedNode))
    {
        rayHit = true;
        int point = octree.selectedNode.points[0];
        // get the point in 3d
        intersectPoint = mars.getMesh(0).getVertex(point);
        float endTime = ofGetElapsedTimeMillis();
        cout <<"it took " << endTime - startTime << " in milliseconds to select the point " << intersectPoint << endl ;
    }
}


//draw a box from a "Box" class  
//
void ofApp::drawBox(const Box &box) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	ofVec3f p = ofVec3f(center.x(), center.y(), center.z());
	float w = size.x();
	float h = size.y();
	float d = size.z();
	ofDrawBox(p, w, h, d);
}

// return a Mesh Bounding Box for the entire Mesh
//
Box ofApp::meshBounds(const ofMesh & mesh) {
	int n = mesh.getNumVertices();
	ofVec3f v = mesh.getVertex(0);
	ofVec3f max = v;
	ofVec3f min = v;
	for (int i = 1; i < n; i++) {
		ofVec3f v = mesh.getVertex(i);

		if (v.x > max.x) max.x = v.x;
		else if (v.x < min.x) min.x = v.x;

		if (v.y > max.y) max.y = v.y;
		else if (v.y < min.y) min.y = v.y;

		if (v.z > max.z) max.z = v.z;
		else if (v.z < min.z) min.z = v.z;
	}
	return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}

//  Subdivide a Box into eight(8) equal size boxes, return them in boxList;
//
void ofApp::subDivideBox8(const Box &box, vector<Box> & boxList) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	float xdist = (max.x() - min.x()) / 2;
	float ydist = (max.y() - min.y()) / 2;
	float zdist = (max.z() - min.z()) / 2;
	Vector3 h = Vector3(0, ydist, 0);

	//  generate ground floor
	//
	Box b[8];
	b[0] = Box(min, center);
	b[1] = Box(b[0].min() + Vector3(xdist, 0, 0), b[0].max() + Vector3(xdist, 0, 0));
	b[2] = Box(b[1].min() + Vector3(0, 0, zdist), b[1].max() + Vector3(0, 0, zdist));
	b[3] = Box(b[2].min() + Vector3(-xdist, 0, 0), b[2].max() + Vector3(-xdist, 0, 0));

	boxList.clear();
	for (int i = 0; i < 4; i++)
		boxList.push_back(b[i]);

	// generate second story
	//
	for (int i = 4; i < 8; i++) {
		b[i] = Box(b[i - 4].min() + h, b[i - 4].max() + h);
		boxList.push_back(b[i]);
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {


}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}


//
//  ScreenSpace Selection Method: 
//  This is not the octree method, but will give you an idea of comparison
//  of speed between octree and screenspace.
//
//  Select Target Point on Terrain by comparing distance of mouse to 
//  vertice points projected onto screenspace.
//  if a point is selected, return true, else return false;
//
bool ofApp::doPointSelection() {

	ofMesh mesh = mars.getMesh(0);
	int n = mesh.getNumVertices();
	float nearestDistance = 0;
	int nearestIndex = 0;

	bPointSelected = false;

	ofVec2f mouse(mouseX, mouseY);
	vector<ofVec3f> selection;

	// We check through the mesh vertices to see which ones
	// are "close" to the mouse point in screen space.  If we find 
	// points that are close, we store them in a vector (dynamic array)
	//
	for (int i = 0; i < n; i++) {
		ofVec3f vert = mesh.getVertex(i);
		ofVec3f posScreen = cam.worldToScreen(vert);
		float distance = posScreen.distance(mouse);
		if (distance < selectionRange) {
			selection.push_back(vert);
			bPointSelected = true;
		}
	}

	//  if we found selected points, we need to determine which
	//  one is closest to the eye (camera). That one is our selected target.
	//
	if (bPointSelected) {
		float distance = 0;
		for (int i = 0; i < selection.size(); i++) {
			ofVec3f point =  cam.worldToCamera(selection[i]);

			// In camera space, the camera is at (0,0,0), so distance from 
			// the camera is simply the length of the point vector
			//
			float curDist = point.length(); 

			if (i == 0 || curDist < distance) {
				distance = curDist;
				selectedPoint = selection[i];
			}
		}
	}
	return bPointSelected;
}

// Set the camera to use the selected point as it's new target
//  
void ofApp::setCameraTarget() {

}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}



//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
} 

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {

	ofVec3f point;
	mouseIntersectPlane(ofVec3f(0, 0, 0), cam.getZAxis(), point);

	if (rover.loadModel(dragInfo.files[0])) {
		rover.setScaleNormalization(false);
		rover.setScale(.005, .005, .005);
		rover.setPosition(point.x, point.y, point.z);
		bRoverLoaded = true;
	}
	else cout << "Error: Can't load model" << dragInfo.files[0] << endl;
}

bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
	glm::vec3 mouse(mouseX, mouseY, 0);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}
void ofApp::collisionDtect()
{
    ofVec3f vel = Sys.particles[0].velocity;
    if (vel.y >= 0) return;
    bool hits = false;
    
    Vector3 location = Vector3 (Sys.particles[0].position.x,Sys.particles[0].position.y,Sys.particles[0].position.z);
    TreeNode node;
    octree.intersect(location, octree.root, node , hits);
    if (hits)
    {
        Vector3 velocity = Vector3(vel.x,vel.y,vel.z);
        //bCollision = true;
        //check velocity, too fast can't land
        if (velocity.length()>3)
        {
            hit.play();

            Sys.particles[0].position.y = 1;
            Sys.particles[0].velocity = ofVec3f (Sys.particles[0].velocity.x,-Sys.particles[0].velocity.y,Sys.particles[0].velocity.z);
        }
        else
        {
        // check if inside the area
            if (location.x()<10 && location.x()>-10 && location.y()<10 && location.y()>-10)
            {
                Sys.forces.clear();
                Sys.particles[0].position.y = 0;
                cout<< "yes";
            }
            //not inside landing area
            else
            {
                hit.play();
                Sys.particles[0].position.y = 1;
                Sys.particles[0].velocity = ofVec3f (Sys.particles[0].velocity.x,-Sys.particles[0].velocity.y,Sys.particles[0].velocity.z);
            }
        }
    }
    
}
void ofApp::AGL()
{
    TreeNode node;
    //create a ray to shoot toward ground.
    Ray ray = Ray(Vector3(Sys.particles[0].position.x, Sys.particles[0].position.y, Sys.particles[0].position.z), Vector3(0, -1, 0));
    octree.intersect(ray, octree.root, node);
    Height = Sys.particles[0].position.y - node.box.max().y()  ;

}
