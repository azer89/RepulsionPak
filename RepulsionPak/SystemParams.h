
/* ---------- ShapeRadiusMatching V2  ---------- */

#ifndef __System_Params__
#define __System_Params__

#include <string>
//#include <vector>

//#include "AVector.h"
//#include "OpenCVWrapper.h"

class SystemParams
{
public:
	SystemParams();
	~SystemParams();

	static void LoadParameters();
	//static void SaveParameters();

public:
	static std::string _lua_file;

	static std::string _window_title;

	static float _mouse_offset_x;
	static float _mouse_offset_y;

	// because square
	static float _upscaleFactor;
	static float _downscaleFactor;

	// LR functions
	//static int _function_length; // radius function length
	//static float _max_lr_ray_length;

	//static float _focal_min_gap;
	//static float _half_max_curve_length; // maximum curve length

	//static float _e_dist_map_offset; // _e_space_offset
	//static float _e_min_stop;	
	//static float _e_step_length;
	//static float _e_min_length;

	static std::string _image_folder;
	static std::string _artName;		// Art name
	static std::string _manual_art_name; // for manual creation of a packing using Illustrator
	static std::string _ornament_dir;	// ornaments
	static std::string _ornament_dir_small_1;
	//static std::string _ornament_dir_small_2;
	//static std::string _small_ornament_dir; // not used
	static std::string _save_folder;

	//static bool _shouldUDFlip;
	//static float _min_blob_sum;
	//static bool _get_negative_shape;
	//static float _neg_shape_offset;
	//static int _which_algorithm;
	// grow offset for iterative algorithm
	//static float _grow_offset;
	//static float _grow_grow_grow;
	// iterative tolerance	
	//static float _shrink_fill_ratio;
	//static float _shrink_transition_time;
	//static float _noise_factor;

	static int _iter_counter;

	// physics
	static float _dt;

	//static bool _activate_attraction_force;
	//static float _k_attraction;

	//static float _k_angle;
	 //static float _angle_friction;
	static float _k_rotate;
	static float _k_noise;
	static float _k_neg_space_edge;
	static float _k_edge_dynamic;
	static float _k_edge;
	static float _k_edge_small_factor;
	//static float _k_edge_more;
	//static float _edge_friction;
	static float _k_repulsion;
	static float _repulsion_soft_factor;
	static float _k_overlap;
	//static float _repulsion_friction;
	static float _k_boundary;
	static float _k_dock;
	//static float _boundary_friction;
	//static float _k_folding;
	//static int   _neighbor_size; // for attaching springs
	static float _bin_square_size;
	static int   _sampling_num;
	//static float _growth_scale;
	static float _growth_scale_iter;
	static int   _collission_block_radius;
	//static float _velocity_damping;
	static float _velocity_cap;

	//static float _fill_ratio;
	static float _growth_threshold_a;
	static float _growth_threshold_b;
	static float _growth_scale_iter_2;

	static float _growth_min_dist;
	static float _max_growth;
	static float _max_growth_small;
	static int   _num_element_pos;
	static float _element_initial_scale;
	static float _random_point_boundary_buffer;
	static int   _relax_iter_num;

	static float _focal_offset;

	// element skin
	static float _boundary_sampling_factor;
	static float _skin_offset;
	//static bool _should_recalculate_element;
	//static bool _manual_element_skin;

	// relax
	static float _self_intersection_threshold;

	// new elements
	//static float _new_elem_dist;
	// attraction force
	//static float _num_peak_threshold;
	static float _peak_dist_stop;
	static float _peak_gap;
	
	//static int _new_elem_time_gap;

	// imgui
	static bool _show_collission_grid;
	static bool _show_boundary;
	static bool _show_element_boundary;
	static bool _show_closest_pairs;
	//static bool _show_mst;
	static bool _simulate_1;
	static bool _simulate_2;
	static bool _show_triangles;	
	static bool _show_elements;
	static bool _show_bending_springs;
	static bool _show_shape_matching;
	static bool _show_uni_art;

	// debug
	//static float _maxForce;
	//static float _maxFrictionForce;

	// rms and drawing
	static float _svg_snapshot_capture_time; // 1
	static float _png_snapshot_capture_time; // 2
	static float _sdf_capture_time;          // 3
	static float _rms_capture_time;          // 4
	//static int _capture_time;
	static float _rms_window;
	static float _rms_threshold;
	
	// focal
	//static bool _focal_as_elements;
	static float _skin_thickness;

	// bad packing
	static int _create_bad_packing;

	//static std::vector<MyColor> _palette_01;

	static int _seed;

	static bool _output_files;
	static int  _screen_width;
	static int  _screen_height;
	static int  _sleep_time; 

	static bool _should_rotate;
	static int  _num_element_pos_limit;

	// drrrr
	//static std::vector<std::vector<AVector>> _perlinMap;
	//static int _pad_step_int;
	static bool _do_shape_matching;

	static float _pad_level;
	static float _pad_delta;
	
	//static float _rdp_epsilon;
	static float _alpha_outside;       // score function
	static float _beta_inside;
	static int   _score_resample_num;  // score function
	static int   _gaussian_smoothing;
	static int   _gaussian_smoothing_element;
	static int   _resample_num;
	static int   _num_nn;
	static float _container_salient_gap; // gap between descriptors	
	//static float _element_salient_gap;   // gap between descriptors
	static float _random_point_gap;
	static float _angle_cap;
	static float _resample_gap_float;

	// for SCP
	static float _container_offset;
	//static float _desc_max_length_ratio;
};

#endif