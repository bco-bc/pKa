#include "simploce/surface/cgal-triangulator.hpp"
#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/surface/edge.hpp"
#include "simploce/surface/vertex.hpp"
#include <stdexcept>
#include <iostream>

// Cgal Includes
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Alpha_shape_3.h>
#include <CGAL/Triangulation_hierarchy_3.h>
#include <CGAL/Triangulation_vertex_base_with_info_3.h>

// Cgal def
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

        // Convert in Point_3
	std::vector<Point_3> cgalPoints;
	for (auto p : points)
	{
	    Point_3 cp(p.x(), p.y(), p.z());
	    cgalPoints.push_back(cp);
	}

	// Insert in Alpha Shape.
	Alpha_shape_3 surface(cgalPoints.begin(), cgalPoints.end());
	Alpha_shape_3::NT alpha_solid = surface.find_alpha_solid();
	Alpha_shape_3::Alpha_iterator opt = surface.find_optimal_alpha(1);
	std::clog << "CGAL: Using optimal alpha value "
		  << *opt << " to get one connected component."<< std::endl;
	surface.set_alpha(*opt);
	assert(as.number_of_solid_components() == 1);

	std::vector<Alpha_shape_3::Vertex_handle> vertexes;
	std::vector<Alpha_shape_3::Cell_handle> cells;
	std::vector<Alpha_shape_3::Facet> facets;
	std::vector<Alpha_shape_3::Edge> segments;

        surface.get_alpha_shape_vertices(std::back_inserter(vertexes), Alpha_shape_3::REGULAR);
	surface.get_alpha_shape_cells(std::back_inserter(cells), Alpha_shape_3::INTERIOR);
	surface.get_alpha_shape_facets(std::back_inserter(facets), Alpha_shape_3::REGULAR);
	surface.get_alpha_shape_facets(std::back_inserter(facets), Alpha_shape_3::SINGULAR);
	surface.get_alpha_shape_edges(std::back_inserter(segments), Alpha_shape_3::REGULAR);

	std::cout << "Cells : " << cells.size() << std::endl;
	std::cout << "Facets : " << facets.size() << std::endl;
	std::cout << "Edges : " << segments.size() << std::endl;
	
	std::vector<vertex_ptr_t> vertices;
	std::vector<Edge> edges;
	std::vector<Triangle> triangles;
	for (auto vertex : vertexes)
	{
	    Point_3 temp = vertex->point();
	    position_t point(temp.x(), temp.y(), temp.z());
	    vertex_ptr_t v = std::make_shared<vertex_t>(point);
	    vertices.push_back(v);
	}

	for (auto edge : segments)
	{
	    // std::cout << edge.second << " " << edge.third << std::endl;
	    Edge e{vertices.at(edge.second), vertices.at(edge.third)};
	    edges.push_back(e);
	}
	
	for (std::size_t i=0; i < facets.size(); ++i)
	{ 
	//     To have a consistent orientation of the facet, always consider an exterior cell
	    if (surface.classify(facets.at(i).first) != Alpha_shape_3::EXTERIOR)
	    {
		facets[i]=surface.mirror_facet(facets.at(i));
	    }
	    
	    CGAL_assertion(surface.classify(facets.at(i).first) == Alpha_shape_3::EXTERIOR);

	    int indices[3]={(facets.at(i).second+1)%4,
			    (facets.at(i).second+2)%4,
			    (facets.at(i).second+3)%4,};

	//     according to the encoding of vertex indices, this is needed to get
	//     a consistent orienation
	    if (facets.at(i).second%2==0)
	    {
		std::swap(indices[0], indices[1]);
	    }

	    // std::cout << indices[0] << " " << indices[1] << " " << indices [2] << std::endl;
	    Triangle t{vertices.at(indices[0]),
		    vertices.at(indices[1]),
		    vertices.at(indices[2])};
	    triangles.push_back(t);
	}

	return TriangulatedSurface{vertices, triangles, edges};    
    }
    
    triangulator_ptr_t CgalTriangulator::make()
    {
	return std::make_shared<CgalTriangulator>();
    }
}
