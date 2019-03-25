#include "simploce/surface/cgal-triangulator.hpp"
#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/surface/edge.hpp"
#include "simploce/surface/vertex.hpp"
#include <stdexcept>
#include <iostream>
#include <map>

// Cgal Includes
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Alpha_shape_3.h>
#include <CGAL/Triangulation_hierarchy_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>

// Cgal typedefs 
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_3 Point_3;

typedef CGAL::Alpha_shape_vertex_base_3<K>                   Vb;
typedef CGAL::Triangulation_hierarchy_vertex_base_3<Vb>      Vbh; 
typedef CGAL::Alpha_shape_cell_base_3<K>                     Fb;
typedef CGAL::Triangulation_data_structure_3<Vbh,Fb>         Tds;

typedef CGAL::Delaunay_triangulation_3<K, Tds, CGAL::Fast_location> Delaunay;
    
typedef CGAL::Alpha_shape_3<Delaunay> Alpha_shape_3;



namespace simploce {

    TriangulatedSurface
    CgalTriangulator::generate(const std::vector<position_t>& points,
			       bool spherical,
			       const radius_t& radius) const
    {
	std::clog << "WARNING: value of 'spherical' is ignored." << std::endl;
	std::clog << "WARNING: value of 'radius' is used as alpha value." << std::endl;

        // Convert position_t to Point_3
	std::vector<Point_3> cgalPoints;
	for (auto p : points)
	{
	    Point_3 cp(p.x(), p.y(), p.z());
	    cgalPoints.push_back(cp);
	}
	
	// Insert points into Alpha Shape.
	Alpha_shape_3 surface(cgalPoints.begin(), cgalPoints.end(), Alpha_shape_3::REGULARIZED);
	
	// Identinfy alpha values corresponding to solid components
	Alpha_shape_3::NT alpha_solid = surface.find_alpha_solid();
	
	// Get the optimal alpha value to get one single solid component
	Alpha_shape_3::Alpha_iterator opt = surface.find_optimal_alpha(1);

	// Set the alpha value to the optimal.
	// After that point the surface return is the final alpha shape
	surface.set_alpha(*opt);
	std::clog << "CGAL: Using optimal alpha value "
		  << *opt << " to get one connected component."<< std::endl;
	assert(as.number_of_solid_components() == 1);

	std::vector<Alpha_shape_3::Vertex_handle> cgalVertices;
	std::vector<Alpha_shape_3::Edge> cgalEdges;
	std::vector<Alpha_shape_3::Facet> facets;

	// Only REGULAR element are keep. E.g. an edge is regular if it is shared by only
	// two faces. This way, we end up with only edge that are ont the surface of the
	// alpha shape.
        surface.get_alpha_shape_vertices(std::back_inserter(cgalVertices),
					 Alpha_shape_3::REGULAR);
	surface.get_alpha_shape_edges(std::back_inserter(cgalEdges),
				      Alpha_shape_3::REGULAR);
	surface.get_alpha_shape_facets(std::back_inserter(facets),
				       Alpha_shape_3::REGULAR);

	std::vector<vertex_ptr_t> vertices;
	std::map<Point_3, vertex_ptr_t> verticesMap;
	for (auto v_it : cgalVertices)
	{
	    Point_3 temp = v_it->point();
	    position_t point(temp.x(), temp.y(), temp.z());
	    vertex_ptr_t v = std::make_shared<vertex_t>(point);
	    verticesMap[v_it->point()] = v;
	    vertices.push_back(v);
	}

	std::vector<Edge> edges;
	for (auto e_it : cgalEdges)
	{
	    auto tmp_tetra = e_it.get<0>();
	    int p1, p2;
	    p1 = e_it.get<1>();
	    p2 = e_it.get<2>();
	    Edge edge{verticesMap.find(tmp_tetra->vertex(p1)->point())->second,
	    	    verticesMap.find(tmp_tetra->vertex(p2)->point())->second};
	    edges.push_back(edge);
	}

	std::vector<Triangle> triangles;
	for (auto tri_it : facets)
	{
	    // Check orientation of the facet
	    if (surface.classify(tri_it.first) != Alpha_shape_3::EXTERIOR)
	    {
	        tri_it = surface.mirror_facet(tri_it);
	    }

	    // Cycle throught the tetrahderon's vertices to get the facet
	    std::vector<vertex_ptr_t> temp;
	    auto tmp_tetra = tri_it.first;	    
	    for (int i = 0; i < 4; i++) {
		if (i != tri_it.second) {
		    temp.push_back(verticesMap.find(tmp_tetra->vertex(i)->point())->second);
		}
	    }

	    // Swap vertex to keep consistensy with orientation
	    if (tri_it.second%2 == 0)
	    {
		std::swap(temp.at(0), temp.at(1));
	    }
	    
	    Triangle triangle{temp.at(0), temp.at(1), temp.at(2)};
	    triangles.push_back(triangle);
	}
	
	return TriangulatedSurface{vertices, triangles, edges};    
    }
    
    triangulator_ptr_t CgalTriangulator::make()
    {
	return std::make_shared<CgalTriangulator>();
    }
}
