
/* ---------- ShapeRadiusMatching V2  ---------- */

#include "CGALTriangulation.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h> // info
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <iostream>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_with_info_2<int, K> Vb; // info
typedef CGAL::Delaunay_mesh_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds> CDT;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT> Criteria;
typedef CDT::Vertex_handle Vertex_handle;
typedef CDT::Point CDTPoint;

#include <limits>

CGALTriangulation::CGALTriangulation()
{
}


CGALTriangulation::~CGALTriangulation()
{
}

void CGALTriangulation::Triangulate(std::vector<AVector> boundary, std::vector<AVector>& vertices, std::vector<AnIdxTriangle>& triangles)
{
	//float scaleFactor = 10;

	CDT cdt;
	std::vector<Vertex_handle> vHandles;
	int sz = boundary.size();
	for (int a = 0; a < sz; a++) // bug
	{
		Vertex_handle vh = cdt.insert(CDTPoint(boundary[a].x, boundary[a].y));
		vHandles.push_back(vh);
	}

	// add constraints which are the boundary itself
	for (int a = 0; a < sz - 1; a++)
	{
		cdt.insert_constraint(vHandles[a], vHandles[a + 1]);
	}
	if (boundary[0].Distance(boundary[sz - 1]) > std::numeric_limits<float>::epsilon())
	{
		cdt.insert_constraint(vHandles[vHandles.size() - 1], vHandles[0]);
	}

	std::list<CDTPoint> seeds;
	seeds.push_back(CDTPoint(0, 0));

	CGAL::refine_Delaunay_mesh_2(cdt, seeds.begin(), seeds.end(), Criteria());

	// make indices and save the vertices
	int a = 0;
	for (CDT::Finite_vertices_iterator vit = cdt.finite_vertices_begin();
		vit != cdt.finite_vertices_end();
		vit++)
	{
		vit->info() = a;

		float x1 = (*(vit)).point().x();
		float y1 = (*(vit)).point().y();
		vertices.push_back(AVector(x1, y1));
		a++;
	}

	for (CDT::Finite_faces_iterator fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit) ////
	{
		if (fit->is_in_domain())
		{
			AnIdxTriangle tri((*(fit->vertex(0))).info(), (*(fit->vertex(1))).info(), (*(fit->vertex(2))).info());
			triangles.push_back(tri);
		}
	}
}