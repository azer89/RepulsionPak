
/* ---------- ShapeRadiusMatching V2  ---------- */

#include "SystemParams.h"

#include "ColorPalette.h"

#include <sstream>

#include "LuaScript.h"


std::string SystemParams::_lua_file = "..\\params.lua";

SystemParams::SystemParams()
{
}

SystemParams::~SystemParams()
{
}

void SystemParams::LoadParameters()
{
	//std::string lua_file = "..\\params.lua";
	LuaScript script(_lua_file);
	
	SystemParams::_upscaleFactor   = script.get<float>("_upscaleFactor");
	SystemParams::_downscaleFactor = script.get<float>("_downscaleFactor");

	SystemParams::_target_fill_ratio = script.get<float>("_target_fill_ratio");

	//SystemParams::_function_length = script.get<int>("_function_length");
	//SystemParams::_focal_min_gap         = script.get<float>("_focal_min_gap");
	//SystemParams::_half_max_curve_length = script.get<float>("_half_max_curve_length");
	//SystemParams::_e_dist_map_offset = script.get<float>("_e_dist_map_offset");
	//SystemParams::_e_min_stop        = script.get<float>("_e_min_stop");
	//SystemParams::_e_step_length     = script.get<float>("_e_step_length");
	//SystemParams::_e_min_length      = script.get<float>("_e_min_length");
	//SystemParams::_max_lr_ray_length = script.get<float>("_max_lr_ray_length");
	//SystemParams::_shouldUDFlip      = script.get<bool>("_shouldUDFlip");

	//SystemParams::_image_folder         = script.get<std::string>("_image_folder");
	SystemParams::_container_file = script.get<std::string>("_container_file");
	//SystemParams::_manual_art_name      = script.get<std::string>("_manual_art_name");
	SystemParams::_primary_elem_dir = script.get<std::string>("_primary_elem_dir");
	SystemParams::_secondary_elem_dir = script.get<std::string>("_secondary_elem_dir");
	//SystemParams::_ornament_dir_small_2 = script.get<std::string>("_ornament_dir_small_2");
	//SystemParams::_small_ornament_dir   = script.get<std::string>("_small_ornament_dir");
	SystemParams::_output_folder = script.get<std::string>("_output_folder");

	//SystemParams::_min_blob_sum = (SystemParams::_function_length * SystemParams::_max_lr_ray_length) / 2.0f;
	// things
	//SystemParams::_get_negative_shape = script.get<bool>("_get_negative_shape");
	//SystemParams::_neg_shape_offset = script.get<float>("_neg_shape_offset");
	//SystemParams::_which_algorithm    = script.get<int>("_which_algorithm");
	// iterative algorithm
	//SystemParams::_grow_offset = script.get<float>("_grow_offset");
	//
	//SystemParams::_grow_grow_grow = script.get<float>("_grow_grow_grow");
	// iterative tolerance
	//SystemParams::_iter_tolerance = script.get<float>("_iter_tolerance");

	/*
	float SystemParams::_k_angle         = 0;
	float SystemParams::_angle_friction  = 0;
	float SystemParams::_k_edge          = 0;
	float SystemParams::_edge_friction   = 0;
	int   SystemParams::_neighbor_size   = 0; // for attaching springs
	float SystemParams::_bin_square_size = 0;
	*/
	SystemParams::_dt = script.get<float>("_dt");

	//SystemParams::_activate_attraction_force = script.get<bool>("_activate_attraction_force");
	//SystemParams::_k_angle         = script.get<float>("_k_angle");
	//SystemParams::_angle_friction  = script.get<float>("_angle_friction");
	SystemParams::_k_neg_space_edge    = script.get<float>("_k_neg_space_edge");
	SystemParams::_k_edge              = script.get<float>("_k_edge");
	SystemParams::_k_edge_small_factor = script.get<float>("_k_edge_small_factor");
	//SystemParams::_k_edge_more = script.get<float>("_k_edge_more");
	//SystemParams::_edge_friction   = script.get<float>("_edge_friction");

	//
	SystemParams::_boundary_sampling_factor = script.get<float>("_boundary_sampling_factor");
	SystemParams::_skin_offset = script.get<float>("_skin_offset");

	//SystemParams::_should_recalculate_element = script.get<bool>("_should_recalculate_element");
	//SystemParams::_manual_element_skin = script.get<bool>("_manual_element_skin");
	//SystemParams::_k_attraction                 = script.get<float>("_k_attraction");
	//SystemParams::_repulsion_friction           = script.get<float>("_repulsion_friction");
	//SystemParams::_boundary_friction            = script.get<float>("_boundary_friction");
	//SystemParams::_k_folding                    = script.get<float>("_k_folding");
	SystemParams::_k_rotate                     = script.get<float>("_k_rotate");
	SystemParams::_k_dock                     = script.get<float>("_k_dock");
	SystemParams::_k_noise                      = script.get<float>("_k_noise");
	SystemParams::_k_repulsion                  = script.get<float>("_k_repulsion");
	SystemParams::_repulsion_soft_factor        = script.get<float>("_repulsion_soft_factor");
	SystemParams::_k_overlap                    = script.get<float>("_k_overlap");	
	SystemParams::_k_boundary                   = script.get<float>("_k_boundary");
	
	SystemParams::_random_point_boundary_buffer = script.get<float>("_random_point_boundary_buffer");

	//SystemParams::_growth_scale      = script.get<float>("_growth_scale");
	//SystemParams::_neighbor_size     = script.get<int>("_neighbor_size");
	//SystemParams::_fill_ratio = script.get<float>("_fill_ratio");
	//SystemParams::_max_growth            = script.get<float>("_max_growth");
	//SystemParams::_velocity_damping        = script.get<float>("_velocity_damping");
	SystemParams::_bin_square_size   = script.get<float>("_bin_square_size");
	SystemParams::_sampling_num      = script.get<int>("_sampling_num");

	SystemParams::_growth_scale_iter = script.get<float>("_growth_scale_iter");	
	SystemParams::_growth_min_dist       = script.get<float>("_growth_min_dist");
	SystemParams::_growth_threshold_a = script.get<float>("_growth_threshold_a");
	SystemParams::_growth_threshold_b = script.get<float>("_growth_threshold_b");
	SystemParams::_growth_scale_iter_2 = script.get<float>("_growth_scale_iter_2");
	
	SystemParams::_num_element_pos       = script.get<int>("_num_element_pos");
	SystemParams::_element_initial_scale = script.get<float>("_element_initial_scale");

	SystemParams::_collission_block_radius = script.get<int>("_collission_block_radius");
	//SystemParams::_max_cg_indices = script.get<int>("_max_cg_indices");
	
	SystemParams::_velocity_cap            = script.get<float>("_velocity_cap");
	SystemParams::_relax_iter_num = script.get<int>("_relax_iter_num");

	//
	//SystemParams::_new_elem_dist = script.get<float>("_new_elem_dist");
	//SystemParams::_num_peak_threshold = script.get<float>("_num_peak_threshold");
	SystemParams::_peak_dist_stop = script.get<float>("_peak_dist_stop");
	SystemParams::_peak_gap = script.get<float>("_peak_gap");
	//SystemParams::_new_elem_time_gap = script.get<int>("_new_elem_time_gap");

	//std::cout << "OK" << "\n";
	//SystemParams::_focal_as_elements = script.get<bool>("_focal_as_elements");
	SystemParams::_focal_offset = script.get<float>("_focal_offset");

	// relax
	SystemParams::_self_intersection_threshold = script.get<float>("_self_intersection_threshold");

	// rms and drawing
	//SystemParams::_capture_time = script.get<int>("_capture_time");
	SystemParams::_svg_snapshot_capture_time = script.get<float>("_svg_snapshot_capture_time");
	SystemParams::_png_snapshot_capture_time = script.get<float>("_png_snapshot_capture_time");
	SystemParams::_sdf_capture_time          = script.get<float>("_sdf_capture_time");
	SystemParams::_rms_capture_time          = script.get<float>("_rms_capture_time");

	SystemParams::_rms_window                = script.get<float>("_rms_window");
	SystemParams::_rms_threshold             = script.get<float>("_rms_threshold");

	SystemParams::_window_title = script.get<std::string>("_window_title");

	//std::cout << "SystemParams::_k_folding " << SystemParams::_k_folding << "\n";
	SystemParams::_create_bad_packing = script.get<int>("_create_bad_packing");

	SystemParams::_seed = script.get<int>("_seed");


	SystemParams::_show_boundary         = script.get<bool>("_show_container");
	SystemParams::_show_elements         = script.get<bool>("_show_elements");
	SystemParams::_show_element_boundary = script.get<bool>("_show_skins");
	SystemParams::_show_triangles        = script.get<bool>("_show_triangles");
	SystemParams::_show_bending_springs  = script.get<bool>("_show_bending_edges");
	SystemParams::_show_shape_matching   = script.get<bool>("_show_shape_matching");

	//SystemParams::_mouse_offset_x = script.get<float>("_mouse_offset_x");
	//SystemParams::_mouse_offset_y = script.get<float>("_mouse_offset_y");
	
	//
	//std::string colors = script.get<std::string>("_colors");
	//std::vector<std::string> color_array = UtilityFunctions::Split(colors, '#');
	//for (unsigned int a = 0; a < color_array.size(); a++)
	//	{ ColorPalette::AddColorString(color_array[a]); }
	//std::string _back_color = script.get<std::string>("_back_color");
	//ColorPalette::_back_color = ColorPalette::AddColorString(_back_color);
		
	SystemParams::_output_files  = script.get<bool>("_output_files");
	SystemParams::_screen_width  = script.get<int>("_screen_width");
	SystemParams::_screen_height = script.get<int>("_screen_height");
	SystemParams::_sleep_time    = script.get<int>("_sleep_time");

	SystemParams::_should_rotate = script.get<bool>("_should_rotate");

	SystemParams::_num_element_pos_limit = script.get<int>("_num_element_pos_limit");

	//SystemParams::_shrink_fill_ratio = script.get<float>("_shrink_fill_ratio");
	//SystemParams::_shrink_transition_time = script.get<float>("_shrink_transition_time");
	//SystemParams::_noise_factor = script.get<float>("_noise_factor");

	//SystemParams::_pad_step_int = script.get<int>("_pad_step_int");
	SystemParams::_do_shape_matching = script.get<bool>("_do_shape_matching");
	SystemParams::_pad_level = script.get<float>("_pad_level");
	SystemParams::_pad_delta = script.get<float>("_pad_delta");
	//SystemParams::_rdp_epsilon = script.get<float>("_rdp_epsilon");
	SystemParams::_resample_gap_float = script.get<float>("_resample_gap_float");
	SystemParams::_resample_num        = script.get<int>("_resample_num");
	SystemParams::_container_salient_gap = script.get<float>("_container_salient_gap");
	SystemParams::_random_point_gap    = script.get<float>("_random_point_gap");
	//SystemParams::_desc_max_length_ratio = script.get<float>("_desc_max_length_ratio");
	SystemParams::_gaussian_smoothing  = script.get<int>("_gaussian_smoothing"); // size of kernel
	SystemParams::_gaussian_smoothing_element = script.get<int>("_gaussian_smoothing_element"); // size of kernel
	SystemParams::_alpha_outside       = script.get<float>("_alpha_outside");
	SystemParams::_beta_inside         = script.get<float>("_beta_inside");
	SystemParams::_score_resample_num  = script.get<int>("_score_resample_num");
	//SystemParams::_element_salient_gap = script.get<float>("_element_salient_gap");
	SystemParams::_num_nn              = script.get<int>("_num_nn");
	SystemParams::_angle_cap           = script.get<float>("_angle_cap");

	SystemParams::_container_offset   = script.get<float>("_container_offset");

	SystemParams::_num_threads = script.get<int>("_num_threads");
	//SystemParams::_num_thread_cg = script.get<int>("_num_thread_cg");
	//SystemParams::_num_thread_springs = script.get<int>("_num_thread_springs");
	//SystemParams::_num_thread_c_pt = script.get<int>("_num_thread_c_pt");
	//SystemParams::_num_thread_solve = script.get<int>("_num_thread_solve");

	// save
	//std::stringstream ss;
	//ss << "copy " << lua_file << " " << SystemParams::_save_folder << "params.lua";
	//std::cout << ss.str() << "\n";
	//std::system(ss.str().c_str());
	/*if (SystemParams::_output_files)
	{
		std::stringstream ss;
		ss << "copy " << _lua_file << " " << SystemParams::_output_folder << "params.lua";
		std::cout << ss.str() << "\n";
		std::system(ss.str().c_str());
	}*/
}


float SystemParams::_upscaleFactor         = 0.0f;
float SystemParams::_downscaleFactor       = 0.0f;

//int SystemParams::_function_length         = 0;
//float SystemParams::_focal_min_gap         = 0.0f;
//float SystemParams::_half_max_curve_length = 0.0f;
//float SystemParams::_e_dist_map_offset     = 0.0f;
//float SystemParams::_e_min_stop            = 0.0f;
//float SystemParams::_e_step_length         = 0.0f;
//float SystemParams::_e_min_length          = 0.0f;
//float SystemParams::_max_lr_ray_length     = 0.0f;
//bool SystemParams::_shouldUDFlip           = false; 

float SystemParams::_target_fill_ratio = 0.0f;

//std::string SystemParams::_image_folder         = "";
std::string SystemParams::_container_file = "";
//std::string SystemParams::_manual_art_name      = "";
std::string SystemParams::_primary_elem_dir = "";
//std::string SystemParams::_small_ornament_dir   = "";
std::string SystemParams::_secondary_elem_dir = "";
//std::string SystemParams::_ornament_dir_small_2 = "";
std::string SystemParams::_output_folder = "";
// blob rejection
//float SystemParams::_min_blob_sum = 0;

// things
//bool SystemParams::_get_negative_shape = false;
//float SystemParams::_neg_shape_offset = 0;
//int SystemParams::_which_algorithm = 0;
// iterative algorithm
//float SystemParams::_grow_offset = 0;
//float SystemParams::_grow_grow_grow = 0;

// element skin
float SystemParams::_boundary_sampling_factor = 0;
float SystemParams::_skin_offset = 0;
//bool SystemParams::_should_recalculate_element = false;
//bool SystemParams::_manual_element_skin = false;

// iterative tolerance
//float SystemParams::_iter_tolerance = 0;

//
int SystemParams::_iter_counter = 0;

// physics
float SystemParams::_dt = 0;

//bool SystemParams::_activate_attraction_force = false;

//float SystemParams::_k_attraction       = 0;
//float SystemParams::_k_angle            = 0;
//float SystemParams::_k_edge_more = 0;
//float SystemParams::_repulsion_friction = 0;
//float SystemParams::_k_folding = 0;
//int   SystemParams::_neighbor_size      = 0; // for attaching springs
//float SystemParams::_growth_scale       = 0.0f;
//float SystemParams::_velocity_damping = 0.0f;
float SystemParams::_k_rotate                = 0;
float SystemParams::_k_noise                 = 0;
float SystemParams::_k_neg_space_edge        = 0;
float SystemParams::_k_edge                  = 0;
//float SystemParams::_k_edge_dynamic          = 1;
float SystemParams::_k_edge_small_factor     = 0;
float SystemParams::_k_repulsion             = 0;
float SystemParams::_repulsion_soft_factor   = 0;
float SystemParams::_k_overlap               = 0;
float SystemParams::_k_boundary              = 0;
float SystemParams::_k_dock              = 0;
float SystemParams::_bin_square_size         = 0;
int   SystemParams::_sampling_num            = 0;

float SystemParams::_growth_scale_iter       = 0.0f;
int   SystemParams::_collission_block_radius = 0;
//int   SystemParams::_max_cg_indices = 0;
float SystemParams::_velocity_cap            = 0;

//float SystemParams::_fill_ratio = 0.0f;
float SystemParams::_growth_threshold_a = 0;
float SystemParams::_growth_threshold_b = 0;
float SystemParams::_growth_scale_iter_2 = 0;

float SystemParams::_growth_min_dist              = 0.0f;
float SystemParams::_max_growth                   = 0.0f;
float SystemParams::_max_growth_small             = 0.0f;
int   SystemParams::_num_element_pos              = 0;
float SystemParams::_element_initial_scale        = 0.0f;
float SystemParams::_random_point_boundary_buffer = 0.0f;
int   SystemParams::_relax_iter_num               = 0;

// relax
float SystemParams::_self_intersection_threshold = 0.0f;

// focal
//bool SystemParams::_focal_as_elements = false;
float SystemParams::_focal_offset = 0;

//
//float SystemParams::_new_elem_dist = 0;
//float SystemParams::_num_peak_threshold = 0;
float SystemParams::_peak_dist_stop = 0;
float SystemParams::_peak_gap      = 0;

//int SystemParams::_new_elem_time_gap = 0;

// imgui
//bool SystemParams::_show_noise = false;
//bool SystemParams::_show_mst = false;
// SET THIS IN PARAMS.LUA
bool SystemParams::_show_boundary         = false;
bool SystemParams::_show_elements         = false;
bool SystemParams::_show_element_boundary = false;
bool SystemParams::_show_triangles        = false;
bool SystemParams::_show_bending_springs  = false;
bool SystemParams::_show_collission_grid  = false;
bool SystemParams::_show_closest_pairs    = false;
bool SystemParams::_show_shape_matching   = false;
bool SystemParams::_show_uni_art = false;

bool SystemParams::_simulate_1 = false;
bool SystemParams::_simulate_2 = false;

// debug
//float SystemParams::_maxForce = 0;
//float SystemParams::_maxFrictionForce = 0;

// rms and drawing
//int   SystemParams::_capture_time = 0;
float SystemParams::_svg_snapshot_capture_time = 0;
float SystemParams::_png_snapshot_capture_time = 0;
float SystemParams::_sdf_capture_time          = 0;
float SystemParams::_rms_capture_time          = 0;
float SystemParams::_rms_window                = 0;
float SystemParams::_rms_threshold             = 0;

std::string SystemParams::_window_title = "";

float SystemParams::_skin_thickness   = 0;
int SystemParams::_create_bad_packing = 0;
int SystemParams::_seed = 0;

float SystemParams::_mouse_offset_x = 0;
float SystemParams::_mouse_offset_y = 0;

bool SystemParams::_output_files  = true;
int  SystemParams::_screen_width  = 0;
int  SystemParams::_screen_height = 0;
int  SystemParams::_sleep_time    = 0;

bool SystemParams::_should_rotate = false;
int  SystemParams::_num_element_pos_limit = 1000000;

int SystemParams::_num_threads = 0;
//int SystemParams::_num_thread_cg = 0;
//int SystemParams::_num_thread_springs = 0;
//int SystemParams::_num_thread_c_pt = 0;
//int SystemParams::_num_thread_solve = 0;

//float SystemParams::_shrink_fill_ratio = 0.0f;
//float SystemParams::_shrink_transition_time = 0.0f;
//float SystemParams::_noise_factor = 0.0f;

//std::vector<std::vector<AVector>> SystemParams::_perlinMap = std::vector<std::vector<AVector>>();

// pyramid of arclength descriptor
//int SystemParams::_pad_step_int            = 0;
//float SystemParams::_rdp_epsilon           = 0.0f;
//float SystemParams::_resample_gap_float    = 0;
//float SystemParams::_desc_max_length_ratio = 0.0f;
bool SystemParams::_do_shape_matching = false;
float SystemParams::_pad_level           = 0.0f;
float SystemParams::_pad_delta           = 0.0f;
float SystemParams::_angle_cap = 0.0f;
float SystemParams::_alpha_outside       = 0.0f;
float SystemParams::_beta_inside = 0.0f;
int   SystemParams::_score_resample_num  = 0;
int   SystemParams::_resample_num        = 0;
float SystemParams::_resample_gap_float = 0;
float SystemParams::_container_salient_gap = 0.0f;
float SystemParams::_random_point_gap    = 0.0f;
//float SystemParams::_element_salient_gap = 0.0f;
int   SystemParams::_gaussian_smoothing  = 0;
int   SystemParams::_gaussian_smoothing_element = 0;
int   SystemParams::_num_nn = 0;

float SystemParams::_container_offset;