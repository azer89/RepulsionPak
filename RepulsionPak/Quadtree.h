#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include <vector>

using namespace std;

class Quadtree;
class QTObject;

class Quadtree {
public:
	Quadtree(float x, float y, float width, float height, int level, int maxLevel, Quadtree* parent);
	~Quadtree();

	void AddOrThrowObject(QTObject *object);
	void			AddObject(QTObject *object);
	vector<QTObject*>	GetObjectsAt(float x, float y);

	vector<QTObject*>	GetObjects();
	void RebuildTree();

	vector<QTObject*> GetInvalidNodesAndReassign();

	void GetLeaves(vector<Quadtree*>& leaves);
	void ThrowPointsFromLeaves();

	void			Clear();

	void Draw();


private:
	float           x;
	float           y;
	float           width;
	float           height;
	int	            level;
	int             maxLevel;
	vector<QTObject*>	objects;

	Quadtree *      parent;
	Quadtree *      NW;
	Quadtree *      NE;
	Quadtree *      SW;
	Quadtree *      SE;

	bool            contains(Quadtree *child, QTObject *object);
};

#endif
