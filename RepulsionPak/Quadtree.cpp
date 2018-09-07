#include "Quadtree.h"
#include "QTObject.h"

#include <iostream>

#include "glew.h"
#include "freeglut.h"

using namespace std;

Quadtree::Quadtree(float _x, float _y, float _width, float _height, int _level, int _maxLevel, Quadtree* _parent) :
	x		(_x),
	y		(_y),
	width	(_width),
	height	(_height),
	level	(_level),
	maxLevel(_maxLevel),
	parent(_parent)
{


	if (level == maxLevel) {
		return;
	}

	const float halfWidth = width * 0.5f;
	const float halfHeight = height * 0.5f;

	NW = new Quadtree(x, y, halfWidth, halfHeight, level+1, maxLevel, this);
	NE = new Quadtree(x + halfWidth, y, halfWidth, halfHeight, level + 1, maxLevel, this);
	SW = new Quadtree(x, y + halfHeight, halfWidth, halfHeight, level + 1, maxLevel, this);
	SE = new Quadtree(x + halfWidth, y + halfHeight, halfWidth, halfHeight, level + 1, maxLevel, this);
}

Quadtree::~Quadtree() {
	if (level == maxLevel) {
		return;
	}

	delete NW;
	delete NE;
	delete SW;
	delete SE;
}

void Quadtree::AddOrThrowObject(QTObject *object)
{
	if (contains(this, object))
	{
		AddObject(object);
		return;
	}

	if (parent)
	{
		parent->AddOrThrowObject(object);
	}
}

void Quadtree::AddObject(QTObject *object) {
	if (level == maxLevel) {
		objects.push_back(object);
		return;
	}

	if (contains(NW, object)) {
		NW->AddObject(object); return;
	} else if (contains(NE, object)) {
		NE->AddObject(object); return;
	} else if (contains(SW, object)) {
		SW->AddObject(object); return;
	} else if (contains(SE, object)) {
		SE->AddObject(object); return;
	}

	// objects only exist in leaf nodes
	//if (contains(this, object)) {
	//	objects.push_back(object);
	//}
}

void Quadtree::ThrowPointsFromLeaves()
{
	if (level < maxLevel) // not leaf
	{
		std::cout << "error!!!\n";
		return;
	}

	for (int a = objects.size() - 1; a >= 0; a--)
	{
		if (!contains(this, objects[a]))
		{
			parent->AddOrThrowObject( objects[a] ) ;
			objects.erase(objects.begin() + a);
		}
	}
}

void Quadtree::GetLeaves(vector<Quadtree*>& leaves)
{
	if (this->level == this->maxLevel)
	{
		leaves.push_back(this);
	}
	else
	{
		NW->GetLeaves(leaves);
		NE->GetLeaves(leaves);
		SW->GetLeaves(leaves);
		SE->GetLeaves(leaves);
	}
}

// don't call this at the root node
vector<QTObject*> Quadtree::GetInvalidNodesAndReassign()
{
	vector<QTObject*> invalidObjects;

	if (level < maxLevel) // not leaf
	{
		{
			vector<QTObject*> childReturnObjects = NW->GetInvalidNodesAndReassign();
			invalidObjects.insert(invalidObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
		}
		{
			vector<QTObject*> childReturnObjects = NE->GetInvalidNodesAndReassign();
			invalidObjects.insert(invalidObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
		}
		{
			vector<QTObject*> childReturnObjects = SW->GetInvalidNodesAndReassign();
			invalidObjects.insert(invalidObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
		}
		{
			vector<QTObject*> childReturnObjects = SE->GetInvalidNodesAndReassign();
			invalidObjects.insert(invalidObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
		}

		// try to reassign to the neighbors
		for (int a = invalidObjects.size() - 1; a >= 0; a--)
		{
			if (contains(this, invalidObjects[a]))
			{
				AddObject(invalidObjects[a]);
				invalidObjects.erase(invalidObjects.begin() + a);
			}
		}
	}
	else // leaf
	{
		for (int a = objects.size() - 1; a >= 0; a--)
		{
			if (!contains(this, objects[a]))
			{
				invalidObjects.push_back(objects[a]);
				objects.erase(objects.begin() + a);
			}
		}
	}
	return invalidObjects;
}

void Quadtree::RebuildTree()
{
	//std::cout << ".";
	
	//vector<QTObject*> invalidObjects;
	{
		vector<QTObject*> childReturnObjects = NW->GetInvalidNodesAndReassign();
		for (int a = childReturnObjects.size() - 1; a >= 0; a--)
		{
			AddObject(childReturnObjects[a]);
		}
		//invalidObjects.insert(invalidObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
	}
	{
		vector<QTObject*> childReturnObjects = NE->GetInvalidNodesAndReassign();
		for (int a = childReturnObjects.size() - 1; a >= 0; a--)
		{
			AddObject(childReturnObjects[a]);
		}
		//invalidObjects.insert(invalidObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
	}
	{
		vector<QTObject*> childReturnObjects = SW->GetInvalidNodesAndReassign();
		for (int a = childReturnObjects.size() - 1; a >= 0; a--)
		{
			AddObject(childReturnObjects[a]);
		}
		//invalidObjects.insert(invalidObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
	}
	{
		vector<QTObject*> childReturnObjects = SE->GetInvalidNodesAndReassign();
		for (int a = childReturnObjects.size() - 1; a >= 0; a--)
		{
			AddObject(childReturnObjects[a]);
		}
		//invalidObjects.insert(invalidObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
	}
	/*for (int a = invalidObjects.size() - 1; a >= 0; a--)
	{
		AddObject(invalidObjects[a]);
	}*/

	/*if (level != 0)
	{
		return;
	}

	vector<QTObject*> allObjects = GetObjects();
	Clear();
	for (int a = 0; a < allObjects.size(); a++)
	{
		AddObject(allObjects[a]);
	}*/
}

vector<QTObject*>	Quadtree::GetObjects()
{
	//return objects;

	if (level == maxLevel) 
	{
		return objects;
	}
	
	vector<QTObject*> returnObjects;
	{
		vector<QTObject*> childReturnObjects = NW->GetObjects();
		returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
	}
	{
		vector<QTObject*> childReturnObjects = NE->GetObjects();
		returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
	}
	{
		vector<QTObject*> childReturnObjects = SW->GetObjects();
		returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
	}
	{
		vector<QTObject*> childReturnObjects = SE->GetObjects();
		returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
	}
	return returnObjects;
}

vector<QTObject*> Quadtree::GetObjectsAt(float _x, float _y) {
	if (level == maxLevel) {
		return objects;
	}
	
	vector<QTObject*> returnObjects, childReturnObjects;

	// objects only exist in leaf nodes
	//if (!objects.empty()) {
	//	returnObjects = objects;
	//}

	const float halfWidth = height * 0.5f;
	const float halfHeight = height * 0.5f;

	if (_x > x + halfWidth && _x < x + width) {
		if (_y > y + halfHeight && _y < y + height) {
			childReturnObjects = SE->GetObjectsAt(_x, _y);
			returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
			return returnObjects;
		} else if (_y > y && _y <= y + halfHeight) {
			childReturnObjects = NE->GetObjectsAt(_x, _y);
			returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
			return returnObjects;
		}
	} else if (_x > x && _x <= x + halfWidth) {
		if (_y > y + halfHeight && _y < y + height) {
			childReturnObjects = SW->GetObjectsAt(_x, _y);
			returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
			return returnObjects;
		} else if (_y > y && _y <= y + halfHeight) {
			childReturnObjects = NW->GetObjectsAt(_x, _y);
			returnObjects.insert(returnObjects.end(), childReturnObjects.begin(), childReturnObjects.end());
			return returnObjects;
		}
	}

	return returnObjects;
}

void Quadtree::Clear() {
	if (level == maxLevel) {
		objects.clear();
		return;
	} else {
		NW->Clear();
		NE->Clear();
		SW->Clear();
		SE->Clear();
	}

	if (!objects.empty()) 
	{
		objects.clear();
	}
}

void Quadtree::Draw()
{
	// draw here
	if(!objects.empty())
	{
		glColor3f(0, 1, 0);
		glLineWidth(0.5f);
		glBegin(GL_LINES);

		// topleft to topright
		glVertex2f(x, y);
		glVertex2f(x + width, y);

		// topright to bottom right
		glVertex2f(x + width, y);
		glVertex2f(x + width, y + height);

		// topleft to bottomleft
		glVertex2f(x, y);
		glVertex2f(x, y + height);

		// bottomleft to bottomright
		glVertex2f(x, y + height);
		glVertex2f(x + width, y + height);

		glEnd();

		glColor3f(1, 0, 0);
		glPointSize(2.0);
		glBegin(GL_POINTS);
		for (int a = 0; a < objects.size(); a++)
		{
			glVertex2f(objects[a]->x, objects[a]->y);
		}
		glEnd();
		
	}

	if (level != maxLevel) 
	{
		NW->Draw();
		NE->Draw();
		SW->Draw();
		SE->Draw();
	}
}

bool Quadtree::contains(Quadtree *child, QTObject *object) 
{
	return !(object->x < child->x ||
             object->y < child->y ||
		     object->x > child->x + child->width  ||
		     object->y > child->y + child->height);// ||
		     //object->x + object->width  < child->x ||
		     //object->y + object->height < child->y ||
		     //object->x + object->width  > child->x + child->width ||
		     //object->y + object->height > child->y + child->height);
}
