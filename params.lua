
---------- RepulsionPak ----------
--- Title of the window
_window_title = "LOL XD";

------ folders ------
_container_file     = "..\\..\\repulsionpak_data\\containers\\heart.path"; --- file of the container 
_primary_elem_dir   = "..\\..\\repulsionpak_data\\animals\\";	--- folder containing primary elements
_secondary_elem_dir = "..\\..\\repulsionpak_data\\small_cat\\"; --- folder containing secondary elements (the elements to fill the remaining empty space after the simulation ends)

--- A directory where we have to save output files
_output_folder  = "..\\..\\repulsionpak_data\\output\\";

-- target of positive space area
_target_fill_ratio = 0.5;

---------------------------------------------------------------------

_do_shape_matching = false;

--- PAD
_pad_level = 5;
_pad_delta = 0.5;
---------------------------------------------------------------------

-- 0.349066 is 20 deg

_angle_cap          = 0.3;
_alpha_outside      = 1;  -- for score function (protusion)
_beta_inside        = 0.001;
_score_resample_num = 1000; -- for score function (instead of grid-based)
_num_nn             = 10; -- number of nearest neighbors in the search space

_gaussian_smoothing    = 1;    -- size of kernel
_gaussian_smoothing_element    = 1;

_resample_gap_float = 1.0;
_resample_num       = 5000; --- curve resampling (how many points)

_container_salient_gap = 100;
_random_point_gap      = 5;   --- gap of random initial placement

---------------------------------------------------------------------
--- WARNING, should be no whitespace before/after commas
--- always add a # character (for splitting)
--- except the last one
---_colors = "190,221,244#" .. --- light blue
---          "58,162,219#" ..  --- blue
---          "57,139,203#" ..  --- darkblue
---          "251,192,45#" ..  --- yellow
---         "244,124,32#" ..  --- orange
---          "242,128,170#" .. --- pink
---          "157,156,158#" .. --- gray
---          "0,0,0#" ..       --- black
---          "255,255,255";    --- white

--- background color
---_back_color = "255,255,255"

_show_container      = false;
_show_elements       = true;
_show_skins          = true;
_show_triangles      = false;
_show_bending_edges  = false;
_show_shape_matching = false;

_output_files  = true;
_screen_width  = 1200;
_screen_height = 800;
_sleep_time    = 20; --- when simulation isn't running

_should_rotate = true;

---------------------------------------------------------------------
--- _mouse_offset_x = 10;
--- _mouse_offset_y = 10;

--- random seed
_seed = -1; --- negative means random

--- initial placement
--- density of random points inside the container 
--- (not the actual number)
--- larger the number, more elements you get,
--- but the simulation time gets slower
_num_element_pos       = 1000;
_num_element_pos_limit = 100;

--- Time step for numerical integration (euler method)
_dt = 0.1;   --- do not set this higher than 0.1

--- random animation
---_shrink_fill_ratio = 1.65;
---_shrink_transition_time = 100.0;
---_noise_factor = 1.0;
--- 1. random perlin 2D
--- 2. perlin 3D
--- 3. random perlin 2D + circular
--- _noise_type = 1;
--- _noise_frequency = 0.00001;
--- _noise_map_update_time = 10000000; --- in ms (disabled)

--- Force parameters
_k_edge                = 20;	--- edge force for filling elements
_k_neg_space_edge      = 5;	--- edge force for springs
_k_edge_small_factor   = 12;
_k_repulsion           = 400;	--- repulsion force
_repulsion_soft_factor = 1.0;	--- soft factor for repulsion force
_k_overlap             = 5;	--- overlap force
_k_boundary            = 20;	--- boundary force
_k_noise               = 0;
_k_rotate              = 0;		--- 1
_k_dock                = 100;

--- _fuk_this = 0.000001;

--- to find peaks where the filling elements should be placed
--- minimum height of the peak to be considered
_peak_dist_stop = 1000.0; --- set this really high to disable filling elements
_peak_gap       = 17.0;	--- any pair of peaks should be separated by this distance

--- capping the velocity
_velocity_cap   = 5; -- [Do not edit]

--- Skin offset for focal elements
--- ignore this if you don't have focal elements
_focal_offset = 2; 

--- for preprocessing
--- skin width
_skin_offset                = 20;  
--- density of random points inside the skin
--- if the density is higher, you get more triangles
_sampling_num               = 100;
--- uniform sampling on the skin
_boundary_sampling_factor   = 1.5;  -- [Do not edit]
---_should_recalculate_element = true;

--- Grid for collision detection
--- size of a cell
_bin_square_size         = 20.0;
--- cell gap for detection, 
--- 1 means considering all cells that are 1 block away from the query (3x3)
--- 2 means considering all cells that are 2 block away from the query (5x5)
_collission_block_radius = 1;  

--_max_cg_indices = 500; -- see ASquare.h 

--- for growing
--- incremetal step for growing
_growth_scale_iter     = 0.003;
--- initial scale after placing the elements inside the target container
_element_initial_scale = 0.05; 
--- epsilon for halting the growth
_growth_min_dist       = 1; 

_growth_threshold_a = 0.02;
_growth_threshold_b = 0.0001;
_growth_scale_iter_2 = 0.00333;

--- initial placement
--- how far away from the boundary so that the protusion cannot be too far
_random_point_boundary_buffer = 300.0; 

--- relaxation to avoid self intersection 
--- how many iteration?
_relax_iter_num              = 1;   
_self_intersection_threshold = 0.3;  

--- 0 as usual
--- 1 vary the skin widths
_create_bad_packing = 0; 

--- artboard dimension (do not edit this)
--- the parameter below means the artboard size is 500x500
_upscaleFactor   = 500.0;                
_downscaleFactor = 1.0 / _upscaleFactor;

--- SVG capture time (in milisecond)
--- for example, if you set it to 1000, 
--- it creates an SVG file every 1 second
_svg_snapshot_capture_time = 10000000;  

_png_snapshot_capture_time = 5000000000;  --- PNG capture time (in milisecond)
_sdf_capture_time          = 1000000000;  --- SDF (Signed distance function) capture time (in milisecond)


--- RMS (Root of mean square) capture time
 
_rms_capture_time = 1000;  --- it is set to 1 second
_rms_window       = 50;    --- Window length is 50
_rms_threshold    = 0.02;  --- RMS stopping criteria

---------------------------------------------------------------------
--- metric
_container_offset = 0; -- for SCP


---------------------------------------------------------------------
-- THREADS
_num_thread_cg      = 4; -- collision grid
_num_thread_springs = 2;
_num_thread_c_pt    = 4; -- closest point
_num_thread_solve   = 2;