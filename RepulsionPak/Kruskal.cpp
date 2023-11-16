
#include "Kruskal.h"

#include "AnIndexedLine.h"

#include <algorithm>
#include <string>

/*struct CompareIndexedLine
{
private:
	AnIndexedLine _val;
public:
	CompareIndexedLine(AnIndexedLine val) : _val(val) {}

	bool operator()(int const idx) const
	{
		// compare the indices
		return _val._index0 == idx || _val._index1 == idx;
	}
};*/

bool CompareEdge(const AnIndexedLine& e1, const AnIndexedLine& e2)
{
	return e1.GetDist() < e2.GetDist();
}// sort ascending

//int Kruskal::FindAGraph(std::vector<AGraph> graphs, int massIndex)
//{
	/*for (int a = 0; a < graphs.size(); a++)
	{
		if (graphs[a].ContainMass(massIndex)) { return a; }
	}*/
	//	return -1;
	//}

	//AGraph Kruskal::CalculateShortestPath(std::vector<AVector> points)
	//{	
		/*std::vector<AGraph> graphs;

		// create one-mass graphs
		for (int a = 0; a < points.size(); a++)
		{
			AGraph aGraph;

			AMass aMass(points[a].x, points[a].y);
			//aMass._m = 1.0f; // weight 1.0kg
			aMass._idx = a;
			aGraph._massMap.insert(std::make_pair(a, aMass)); // std::map<int, AMass> _massMap;

			graphs.push_back(aGraph);
		}

		// create candidate edges
		std::vector<AnIndexedLine> allEdges;

		for (int a = 0; a < points.size() - 1; a++)
		{
			for (int b = a + 1; b < points.size(); b++)
			{
				AnIndexedLine anEdge(a, b, points[a].Distance(points[b]));
				allEdges.push_back(anEdge);
			}
		}

		// sort edges
		std::sort(allEdges.begin(), allEdges.end(), CompareEdge);

		// kruskal
		for (int a = 0; a < allEdges.size() && graphs.size() > 1; a++)
		{
			AnIndexedLine anEdge = allEdges[a];

			int graphIdx0 = FindAGraph(graphs, anEdge._index0);
			int graphIdx1 = FindAGraph(graphs, anEdge._index1);

			if (graphIdx0 != -1 && graphIdx1 != -1 && graphIdx0 != graphIdx1)
			{
				graphs[graphIdx0].Concat(graphs[graphIdx1]); // combine two graphs
				graphs[graphIdx0]._edges.push_back(anEdge);  // add edge
				graphs.erase(graphs.begin() + graphIdx1);    // delete the second one
			}
		}

		AGraph finalGraph = graphs[0];*/

		//return finalGraph;

	//}