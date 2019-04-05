#include "simploce/surface/cgal-triangulator.hpp"
#include "simploce/surface/triangulated-surface.hpp"
#include "simploce/surface/triangle.hpp"
#include "simploce/surface/edge.hpp"
#include "simploce/surface/vertex.hpp"
#include <stdexcept>
#include <iostream>
#include <map>
#include <utility> 
#include <list>
#include <fstream>

// Cgal Includes
#include <CGAL/config.h>
#include <CGAL/Timer.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

// Point Processing 
#include <CGAL/jet_estimate_normals.h>
#include <CGAL/pca_estimate_normals.h>
#include <CGAL/vcm_estimate_normals.h>
#include <CGAL/jet_smooth_point_set.h>
#include <CGAL/mst_orient_normals.h>

// Surface reconstruction
#include <CGAL/property_map.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/poisson_surface_reconstruction.h>

#include <CGAL/Scale_space_surface_reconstruction_3.h>
#include <CGAL/Scale_space_reconstruction_3/Advancing_front_mesher.h>
#include <CGAL/Scale_space_reconstruction_3/Jet_smoother.h>


// Concurrency
#ifdef CGAL_LINKED_WITH_TBB
typedef CGAL::Parallel_tag Concurrency_tag;
#else
typedef CGAL::Sequential_tag Concurrency_tag;
#endif
 
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::Point_3 Point;
typedef Kernel::Vector_3 Vector;
typedef std::pair<Point, Vector> PointVectorPair;
typedef CGAL::Timer Timer;

typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef CGAL::Scale_space_surface_reconstruction_3<Kernel> Reconstruction;

typedef CGAL::Scale_space_reconstruction_3::Jet_smoother<Kernel>           JetSmoother;
typedef CGAL::Scale_space_reconstruction_3::Weighted_PCA_smoother<Kernel>  PCASmoother;
typedef CGAL::Scale_space_reconstruction_3::Advancing_front_mesher<Kernel> AdvancingFrontMesher;
typedef CGAL::Scale_space_reconstruction_3::Alpha_shape_mesher<Kernel>     AlphaShapeMesher;


namespace simploce {
    
    static void scale_space_surface_reconstruction (std::vector<Point>& points,
						    int nb_neighbors=24,
						    float size_facets=0.5,
						    float scale=4.0)
    {
	std::clog << "Scale Space Method." << std::endl;
	CGAL::Timer t;
	t.start();
	
	std::clog << "Reconstruction >> ";
	// Construct the mesh in a scale space.
	Reconstruction reconstruct (points.begin(), points.end());
	
        //PCASmoother Psmoother(8, 300);
	// reconstruct.increase_scale(2, Psmoother);
	// reconstruct.reconstruct_surface(asmesher);

	reconstruct.increase_scale(scale, JetSmoother(nb_neighbors));    
	reconstruct.reconstruct_surface(AdvancingFrontMesher(size_facets));
	std::clog << "DONE in " << t.time() << "s." << std::endl;
	t.reset();

	std::clog << "Writting >> ";
	std::ostringstream oss;
	oss << "scale" << nb_neighbors << "_" << size_facets << " " << scale << ".off";
	std::ofstream out (oss.str());
	out << reconstruct;
	// dump_reconstruction(reconstruct, "scale.off")
	std::clog << "DONE in " << t.time() << "s." << std::endl;
	t.reset();
	
    }
    
    static void poisson_surface_reconstruction (std::vector<Point>& points,
						int normal_neighbors=18,
						int spacing_neighbors=6,
						float scaling=1.0)
    {

        // Smoothing of the points set
	std::clog << "Poisson Method." << std::endl;
	CGAL::Timer t;
	t.start();
	
	// std::clog << "Smoothing Point cloud >> ";
	// CGAL::jet_smooth_point_set<Concurrency_tag>(points, 6);
	// std::clog << "DONE in " << t.time() << "s." << std::endl;
	// t.reset();
	
        std::list<PointVectorPair> pointsNormal;
	for (auto p : points)
	{
	    PointVectorPair pn;
	    pn.first = p;
	    pn.second = Vector();
	    pointsNormal.push_back(pn);
	}
	t.reset();
	
	// Estimates normals direction.
	// Note: pca_estimate_normals() requiresa range of points
	// as well as property maps to access each point's position and normal.
	std::clog << "Compute Normals >> "; 
	// const int normal_neighbors = 18; // K-nearest neighbors = 3 rings
	CGAL::jet_estimate_normals<Concurrency_tag>
	    (pointsNormal, normal_neighbors,
	     CGAL::parameters::point_map(CGAL::First_of_pair_property_map<PointVectorPair>()).
	     normal_map(CGAL::Second_of_pair_property_map<PointVectorPair>()));
        std::clog << "DONE in " << t.time() << "s." << std::endl;
	t.reset();
	 
	// Orients normals.
	// Note: mst_orient_normals() requires a range of points
	// as well as property maps to access each point's position and normal.
	std::clog << "Orient Normals >> ";
	std::list<PointVectorPair>::iterator unoriented_points_begin =
	    CGAL::mst_orient_normals(pointsNormal, normal_neighbors,
				     CGAL::parameters::point_map(
					 CGAL::First_of_pair_property_map<PointVectorPair>()).
				     normal_map(
					 CGAL::Second_of_pair_property_map<PointVectorPair>()));
        std::clog << "DONE in " << t.time() << "s." << std::endl;
	
	// Optional: delete points with an unoriented normal
	// if you plan to call a reconstruction algorithm that expects oriented normals.
	pointsNormal.erase(unoriented_points_begin, pointsNormal.end());
	t.reset();
	
	Polyhedron output_mesh;
	std::clog << "Compute Average Spacing >> "; 
	double average_spacing = CGAL::compute_average_spacing<Concurrency_tag>
	    (pointsNormal, spacing_neighbors,
	     CGAL::parameters::point_map(CGAL::First_of_pair_property_map<PointVectorPair>()));
        std::clog << "DONE in " << t.time() << "s." << std::endl;
	t.reset();
	
	std::clog << "Reconstruction >> ";
	CGAL::poisson_surface_reconstruction_delaunay
	    (pointsNormal.begin(), pointsNormal.end(),
	     CGAL::First_of_pair_property_map<PointVectorPair>(),
	     CGAL::Second_of_pair_property_map<PointVectorPair>(),
	     output_mesh, scaling*average_spacing);
        std::clog << "DONE in " << t.time() << "s." << std::endl;
	t.reset();
	
	std::clog << "Writting >> ";
        std::ostringstream oss;
	oss << "poisson_" << normal_neighbors << "_"
	    << spacing_neighbors << "_" << scaling << ".off";
	std::ofstream out (oss.str());
	out << output_mesh;
        std::clog << "DONE in " << t.time() << "s." << std::endl;
	t.reset();
    }

    void advancing_front_sruface_reconstruction (std::vector<Point>& points)
    {

	
	
    }
    
    TriangulatedSurface
    CgalTriangulator::generate(const std::vector<position_t>& points,
			       bool spherical,
			       const radius_t& radius) const
    {
	std::clog << "WARNING: value of 'spherical' is ignored." << std::endl;
	std::clog << "WARNING: value of 'radius' is ignored." << std::endl;
	std::clog << "Using CGAL Version " <<  CGAL_VERSION_NR << " (1MMmmb1000)." << std::endl;
	
        // Convert position_t to Point
	std::vector<Point> cgalPoints;
	for (auto p : points)
	{
	    Point cp(p.x(), p.y(), p.z());
	    cgalPoints.push_back(cp);
	}
	poisson_surface_reconstruction(cgalPoints, 24, 24, 0.5);
	poisson_surface_reconstruction(cgalPoints, 24, 18, 0.5);
	poisson_surface_reconstruction(cgalPoints, 24, 12, 0.5);
	poisson_surface_reconstruction(cgalPoints, 24, 6, 0.5);
	poisson_surface_reconstruction(cgalPoints, 18, 24, 0.5);
	poisson_surface_reconstruction(cgalPoints, 18, 18, 0.5);
	poisson_surface_reconstruction(cgalPoints, 18, 12, 0.5);
	poisson_surface_reconstruction(cgalPoints, 18, 6, 0.5);
	poisson_surface_reconstruction(cgalPoints, 12, 24, 0.5);
	poisson_surface_reconstruction(cgalPoints, 12, 18, 0.5);
	poisson_surface_reconstruction(cgalPoints, 12, 12, 0.5);
	poisson_surface_reconstruction(cgalPoints, 12, 6, 0.5);
	poisson_surface_reconstruction(cgalPoints, 6, 24, 0.5);
	poisson_surface_reconstruction(cgalPoints, 6, 18, 0.5);
	poisson_surface_reconstruction(cgalPoints, 6, 12, 0.5);
	poisson_surface_reconstruction(cgalPoints, 6, 6, 0.5);
	
	poisson_surface_reconstruction(cgalPoints, 24, 24, 0.3);
	poisson_surface_reconstruction(cgalPoints, 24, 18, 0.3);
	poisson_surface_reconstruction(cgalPoints, 24, 12, 0.3);
	poisson_surface_reconstruction(cgalPoints, 24, 6, 0.3);
	poisson_surface_reconstruction(cgalPoints, 18, 24, 0.3);
	poisson_surface_reconstruction(cgalPoints, 18, 18, 0.3);
	poisson_surface_reconstruction(cgalPoints, 18, 12, 0.3);
	poisson_surface_reconstruction(cgalPoints, 18, 6, 0.3);
	poisson_surface_reconstruction(cgalPoints, 12, 24, 0.3);
	poisson_surface_reconstruction(cgalPoints, 12, 18, 0.3);
	poisson_surface_reconstruction(cgalPoints, 12, 12, 0.3);
	poisson_surface_reconstruction(cgalPoints, 12, 6, 0.3);
	poisson_surface_reconstruction(cgalPoints, 6, 24, 0.3);
	poisson_surface_reconstruction(cgalPoints, 6, 18, 0.3);
	poisson_surface_reconstruction(cgalPoints, 6, 12, 0.3);
	poisson_surface_reconstruction(cgalPoints, 6, 6, 0.3);
	// scale_space_surface_reconstruction(cgalPoints,24, 0.1, 5.0);
	// scale_space_surface_reconstruction(cgalPoints,24, 0.2, 5.0);
	// scale_space_surface_reconstruction(cgalPoints,24, 0.3, 5.0);
	// scale_space_surface_reconstruction(cgalPoints,24, 0.4, 5.0);
	// scale_space_surface_reconstruction(cgalPoints,24, 0.5, 5.0);
	// scale_space_surface_reconstruction(cgalPoints,24, 1.0, 5.0);
	// scale_space_surface_reconstruction(cgalPoints,24, 1.5, 5.0);
	// scale_space_surface_reconstruction(cgalPoints,24, 2.0, 5.0);
        // scale_space_surface_reconstruction(cgalPoints,24, 0.5, 4.0);
        // scale_space_surface_reconstruction(cgalPoints,24, 1.0, 4.0);
	// scale_space_surface_reconstruction(cgalPoints,24, 1.5, 4.0);
	// scale_space_surface_reconstruction(cgalPoints,24, 2.0, 4.0);
	// scale_space_surface_reconstruction(cgalPoints,24, 0.5, 2.0);
	// scale_space_surface_reconstruction(cgalPoints,24, 1.0, 2.0);
	// scale_space_surface_reconstruction(cgalPoints,24, 1.5, 2.0);
	// scale_space_surface_reconstruction(cgalPoints,24, 2.0, 2.0);
	// scale_space_surface_reconstruction(cgalPoints,24, 0.5, 1.0);
	// scale_space_surface_reconstruction(cgalPoints,24, 1.0, 1.0);
	// scale_space_surface_reconstruction(cgalPoints,24, 1.5, 1.0);
	// scale_space_surface_reconstruction(cgalPoints,24, 2.0, 1.0);
	
 	std::vector<vertex_ptr_t> vertices;
	std::vector<Edge> edges;
	std::vector<Triangle> triangles;

	return TriangulatedSurface{vertices, triangles, edges};    
    }
    
    triangulator_ptr_t CgalTriangulator::make()
    {
	return std::make_shared<CgalTriangulator>();
    }
}
