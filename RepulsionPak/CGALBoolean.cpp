
#include "CGALBoolean.h"

#include "ClipperWrapper.h"
#include "SelfIntersectionFixer.h"

//
// http://doc.cgal.org/latest/Boolean_set_operations_2/index.html#Chapter_2D_Regularized_Boolean_Set-Operations
//

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <list>

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::Point_2                                   Point_2;
typedef CGAL::Polygon_2<Kernel>                           Polygon_2;
typedef CGAL::Polygon_with_holes_2<Kernel>                Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2>                   Pwh_list_2;

std::vector<AVector> CGALBoolean::UnionOperationWithGrowing(std::vector<std::vector<AVector>>& polys)
{
	std::cout << "UnionOperationWithGrowing\n";
	SelfIntersectionFixer  siFixer;

	for (int a = 0; a < polys.size(); a++)
		{ polys[a] = ClipperWrapper::GetLargestPoly(  ClipperWrapper::MiterOffsettingP(polys[a], 3, 3) ); }
	
	Polygon_2 P1;
	Polygon_2 P2;
	

	for (int a = 0; a < polys[0].size(); a++)
		{ P1.push_back(Point_2(polys[0][a].x, polys[0][a].y)); }

	//for (int a = 1; a < 3; a++)
	for (int a = 1; a < polys.size(); a++)
	{
		std::cout << a << "\n";
		std::vector<AVector> po = polys[a];
		for (int b = 0; b < po.size(); b++)
			{ P2.push_back(Point_2(po[b].x, po[b].y)); }
		
		Polygon_with_holes_2 unionR;
		if (CGAL::join(P1, P2, unionR))
		{ 
			std::cout << "union\n";
			P1 = unionR.outer_boundary(); 
		}
	}

	std::vector<AVector> unionPoly;
	Polygon_2::Vertex_const_iterator  vit;
	for (vit = P1.vertices_begin(); vit != P1.vertices_end(); ++vit)
	{
		float x = CGAL::to_double((*vit).x());
		float y = CGAL::to_double((*vit).y());
		unionPoly.push_back(AVector(x, y));
	}

	if (siFixer.IsSimple(unionPoly))
	{
		std::cout << "unionPoly is simple\n";
	}
	else
	{
		std::cout << "unionPoly is NOT simple\n";
	}

	return unionPoly;
}