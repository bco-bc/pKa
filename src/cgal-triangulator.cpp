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
#include <CGAL/Real_timer.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

// Point Processing
#include <CGAL/grid_simplify_point_set.h>
#include <CGAL/jet_estimate_normals.h>
#include <CGAL/pca_estimate_normals.h>
#include <CGAL/vcm_estimate_normals.h>
#include <CGAL/jet_smooth_point_set.h>
#include <CGAL/mst_orient_normals.h>
#include <CGAL/estimate_scale.h>
#include <CGAL/edge_aware_upsample_point_set.h>
#include <CGAL/bilateral_smooth_point_set.h>

// Surface reconstruction
#include <CGAL/property_map.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/poisson_surface_reconstruction.h>

#include <CGAL/Scale_space_surface_reconstruction_3.h>
#include <CGAL/Scale_space_reconstruction_3/Advancing_front_mesher.h>
#include <CGAL/Scale_space_reconstruction_3/Jet_smoother.h>

#include <boost/lexical_cast.hpp>

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
typedef Kernel::FT FT;

typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef CGAL::Scale_space_surface_reconstruction_3<Kernel> Reconstruction;

typedef CGAL::Scale_space_reconstruction_3::Jet_smoother<Kernel>           JetSmoother;
typedef CGAL::Scale_space_reconstruction_3::Weighted_PCA_smoother<Kernel>  PCASmoother;
typedef CGAL::Scale_space_reconstruction_3::Advancing_front_mesher<Kernel> AdvancingFrontMesher;
typedef CGAL::Scale_space_reconstruction_3::Alpha_shape_mesher<Kernel>     AlphaShapeMesher;


namespace simploce {
    
    // instance of CGAL::cpp11::function<bool(double)>
    struct Progress_to_std_cerr_callback
    {
	mutable std::size_t nb;
	CGAL::Real_timer timer;
	double t_start;
	mutable double t_latest;
	const std::string name;
	Progress_to_std_cerr_callback (const char* name)
	    : name (name)
	    {
		timer.start();
		t_start = timer.time();
		t_latest = t_start;
	    }
  
	bool operator()(double advancement) const
	    {
		// Avoid calling time() at every single iteration, which could
		// impact performances very badly
		++ nb;
		if (advancement != 1 && nb % 100 != 0)
		    return true;
		double t = timer.time();
		if (advancement == 1 || (t - t_latest) > 0.1) // Update every 1/10th of second
		{
		    std::clog << "\r" // Return at the beginning of same line and overwrite
			      << name << ": " << int(advancement * 100) << "%";
      
		    if (advancement == 1)
			std::clog << std::endl;
		    t_latest = t;
		}
		return true;
	    }
    };

    int reconstruct(std::vector<Point>& points, Polyhedron& mesh, double scale=0.25)
    {
	// Smoothing by fitting on smoothing Jet surface
	const int nb_neighbors = 24; // K-nearest neighbors = 3 rings
	CGAL::jet_smooth_point_set<Concurrency_tag>(points, nb_neighbors,
        					    CGAL::parameters::callback(
							Progress_to_std_cerr_callback(
							    "Jet Smoothing")));
	
	// Make PointVectorPair List
	std::vector<PointVectorPair> pvpVector;
	for (auto point : points)
	{
	    PointVectorPair pvp;
	    pvp.first = point;
	    pvp.second = Vector();
	    pvpVector.push_back(pvp);
	}
	std::list<PointVectorPair> pvpList(pvpVector.begin(), pvpVector.end());

	// Estimates normals direction.
	// Note: pca_estimate_normals() requiresa range of points
	// as well as property maps to access each point's position and normal.
	
	CGAL::pca_estimate_normals<Concurrency_tag>
	    (pvpList, nb_neighbors,
	     CGAL::parameters::point_map(CGAL::First_of_pair_property_map<PointVectorPair>()).
	     normal_map(CGAL::Second_of_pair_property_map<PointVectorPair>()).
	     callback(Progress_to_std_cerr_callback("PCA estimate normals")));
	
	// Orients normals.
	// Note: mst_orient_normals() requires a range of points
	// as well as property maps to access each point's position and normal.
	std::list<PointVectorPair>::iterator unoriented_points_begin =
	    CGAL::mst_orient_normals(pvpList, nb_neighbors,
				     CGAL::parameters::point_map(CGAL::First_of_pair_property_map<PointVectorPair>()).
				     normal_map(CGAL::Second_of_pair_property_map<PointVectorPair>()).
				     callback(Progress_to_std_cerr_callback("MST orient normals")));

	// Optional: delete points with an unoriented normal
	// if you plan to call a reconstruction algorithm that expects oriented normals.
	pvpList.erase(unoriented_points_begin, pvpList.end());

	// Smooth from normals
	const double sharpness_angle = 90;   // control sharpness of the result.
	CGAL::bilateral_smooth_point_set <Concurrency_tag>(
	    pvpList,
	    nb_neighbors,
	    CGAL::parameters::point_map(CGAL::First_of_pair_property_map<PointVectorPair>()).
	    normal_map(CGAL::Second_of_pair_property_map<PointVectorPair>()).
	    sharpness_angle (sharpness_angle).
	    callback(Progress_to_std_cerr_callback("Bilateral smoothing")));

	// Reconstruction
	// Polyhedron output_mesh;

	// Average Spacing computation
	double average_spacing = CGAL::compute_average_spacing<Concurrency_tag>
	    (pvpList, nb_neighbors,
	     CGAL::parameters::point_map(CGAL::First_of_pair_property_map<PointVectorPair>()).
	     callback(Progress_to_std_cerr_callback("Average Spacing")));

	if (!CGAL::poisson_surface_reconstruction_delaunay
	    (pvpList.begin(), pvpList.end(),
	     CGAL::First_of_pair_property_map<PointVectorPair>(),
	     CGAL::Second_of_pair_property_map<PointVectorPair>(),
	     mesh, scale*average_spacing))
	{
	    // std::ofstream out("kitten_poisson-20-30-0.375.off");
	    // out << output_mesh;
	    return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
	
    }

    int getVertices(Polyhedron& mesh, std::vector<vertex_ptr_t>& vertices)
    {

	return EXIT_SUCCESS;
    }

    int getEdges(Polyhedron& mesh, std::vector<Edge>& edges)
    {
	return EXIT_SUCCESS;
    }

    int getTriangles(Polyhedron& mesh, std::vector<Triangle>& triangles)
    {
	return EXIT_SUCCESS;
    }

    TriangulatedSurface toTriangulatedSurface(Polyhedron& mesh)
    {
	std::vector<vertex_ptr_t> vertices;
	std::vector<Edge> edges;
	std::vector<Triangle> triangles;
	
	if (getVertices(mesh, vertices))
	{
	    std::clog << "ERROR: Failed to get Vertices" << std::endl;
	}
	    
        if (getTriangles(mesh, triangles))
        {
	    std::clog << "ERROR: Failed to get Triangles" << std::endl;
	}
	
	if (getEdges(mesh, edges))
	{
	    std::clog << "ERROR: Failed to get edges" << std::endl;
	}
	
	return TriangulatedSurface{vertices, triangles, edges};  
    }
    
    TriangulatedSurface
    CgalTriangulator::generate(const std::vector<position_t>& points,
			       bool spherical,
			       const radius_t& radius) const
    {
	std::clog << "WARNING: value of 'spherical' is ignored." << std::endl;
	std::clog << "WARNING: value of 'radius' is ignored." << std::endl;
	std::string cgalVersion = std::to_string(CGAL_VERSION_NR);
	std::clog << "Using CGAL Version "
		  << cgalVersion.substr(1,2) << "."
		  << cgalVersion.substr(3,4) << std::endl;
	
        // Convert position_t to Point
	std::vector<Point> cgalPoints;
	for (auto p : points)
	{
	    Point cp(p.x(), p.y(), p.z());
	    cgalPoints.push_back(cp);
	}
	
	// Reconstruct
	Polyhedron mesh;
        reconstruct(cgalPoints, mesh);

	// Convert
	TriangulatedSurface Surface = toTriangulatedSurface(mesh);
	
	return Surface;
    }
    
    triangulator_ptr_t CgalTriangulator::make()
    {
	return std::make_shared<CgalTriangulator>();
    }
}
