//  Kevin M. Smith - Basic Octree Class - CS134/235 4/18/18
//


#include "Octree.h"
#include "Util.h"



// draw Octree (recursively)
//
void Octree::draw(TreeNode & node, int numLevels, int level) {
	if (level >= numLevels) return;
	drawBox(node.box, level);
	level++;
	for (int i = 0; i < node.children.size(); i++) {
		draw(node.children[i], numLevels, level);
	}
}

// draw only leaf Nodes
//
void Octree::drawLeafNodes(TreeNode & node) {

}


//draw a box from a "Box" class  
//
// max can show 10 levels
void Octree::drawBox(const Box &box, int level)
{
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	ofVec3f p = ofVec3f(center.x(), center.y(), center.z());
	float w = size.x();
	float h = size.y();
	float d = size.z();
    if(level%10==0){
        ofSetColor(255, 0, 0);
    }
    else if(level%10==1){
        ofSetColor(255, 255, 0);
    }
    else if(level%10==2){
        ofSetColor(255, 0, 255);
    }
    else if(level%10==3){
        ofSetColor(0, 255, 255);
    }
    else if(level%10==4){
        ofSetColor(0, 255, 0);
    }
    else if(level%10==5){
        ofSetColor(128, 255, 0);
    }
    else if(level%10==6){
        ofSetColor(255, 128, 255);
    }
    else if(level%10==7){
        ofSetColor(128, 255, 255);
    }
    else if(level%10==8){
        ofSetColor(128, 255, 0);
    }
    else if(level%10==9){
        ofSetColor(128, 128, 128);
    }
 
    
	ofDrawBox(p, w, h, d);
}

// return a Mesh Bounding Box for the entire Mesh
//
Box Octree::meshBounds(const ofMesh & mesh) {
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
	cout << "vertices: " << n << endl;
//	cout << "min: " << min << "max: " << max << endl;
	return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}

// getMeshPointsInBox:  return an array of indices to points in mesh that are contained 
//                      inside the Box.  Return count of points found;
//
int Octree::getMeshPointsInBox(const ofMesh & mesh, const vector<int>& points,
	Box & box, vector<int> & pointsRtn)
{
    int count = 0;
    for (int i = 0; i < points.size(); i++)
    {
        //get 3d points
        ofVec3f v = mesh.getVertex(points[i]);
        // if the points are inside the box,
        if (box.inside(Vector3(v.x, v.y, v.z))) {
            count++;
            pointsRtn.push_back(points[i]);
        }
    }
    return count;
}





//  Subdivide a Box into eight(8) equal size boxes, return them in boxList;
//
void Octree::subDivideBox8(const Box &box, vector<Box> & boxList) {
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
// create a octree, box, point and children
void Octree::create(const ofMesh & geo, int numLevels)
{
    root = TreeNode();
//intialize root.box
    Box boundingBox = meshBounds(geo);
    root.box = boundingBox;
    
    
//intialize root.points
    vector<ofIndexType> marsIndices = geo.getIndices();
    
    //change ofIndexType vector to int vector
    vector <int> pointsInsideMesh(marsIndices.size());
    for(int i=0; i < marsIndices.size(); i++){
        pointsInsideMesh[i] = marsIndices[i];
    }
    
    vector <int> pointsInsideBox;
    // push the point insdide the mesh to pointsInsideBox
    getMeshPointsInBox(geo, pointsInsideMesh, boundingBox, pointsInsideBox);
    
    /*  equals to getMeshPointsInBox method
    for (int i = 0; i < pointsInsideMesh.size(); i++)
    {
        ofVec3f v = geo.getVertex(pointsInsideMesh[i]);
        if (boundingBox.inside(Vector3(v.x, v.y, v.z))) {
            pointsInsideBox.push_back(pointsInsideMesh[i]);
        }
    }
    */
    
    root.points = pointsInsideBox;
   // cout<<"points" <<pointsInsideBox[0];
//intialize root.children (8)
    subdivide(geo, root, numLevels, 0);
    
}


void Octree::subdivide(const ofMesh & mesh, TreeNode & node, int numLevels, int level)
{
    level++;
    // if over the set level
    if (level > numLevels){
        return;
    }
    
    //divide the node.box into a childrenBoxes（8 box）
    vector<Box> childrenBoxes;
    subDivideBox8(node.box, childrenBoxes);
    
    //create 8 children node of root
    for (int i = 0; i < 8; i++)
    {
        TreeNode child = TreeNode();
        child.box = childrenBoxes[i];
        //push the points inside the childbox to child.points
        // initialize child.points
        getMeshPointsInBox(mesh, node.points, child.box, child.points);
        // recursively divide the child.box to our determinded level
        subdivide(mesh, child, numLevels, level);
        
        //attach children to their parent node.
        if(child.points.size()>0)
        {
            node.children.push_back(child);
        }
    }
}

bool Octree::intersect(const Ray &ray, const TreeNode & node, TreeNode & nodeRtn)
{
    //if node/ root is intersected
    if(node.box.intersect(ray, -10000, 10000))
    {
        // check node/ root's children
        if(node.children.size()>0)
        {
            for(int i = 0; i<node.children.size(); i++)
            {
                // the point should be found in the child's box, and find the smallest box, see else
                intersect(ray, node.children[i], nodeRtn);
                
            }
        }
        //if no child
        else{
            nodeRtn = node;
            selectedNode = nodeRtn;
            return true;
        }
    }
    //isn't intersected
    else
    {
       
        return false;
    }
}

bool Octree::intersect(const Vector3 &point, const TreeNode & node, TreeNode & nodeRtn, bool & hit)
{
    // check node/ root's children

    if (node.box.inside(point))
    {
         if(node.children.size()>0)
         {
             for(int i = 0; i<node.children.size(); i++)
             {
                 // the point should be found in the child's box, and find the smallest box, see else
                 intersect(point, node.children[i], nodeRtn,hit);
                 
                
             }
         }
    
        //if no child, must be here
        else
        {
            hit = true;
            //cout<<"here";
            nodeRtn = node;
            return true;
        }
    }
    //isn't intersected
    else
    {
        return false;
    }
}
