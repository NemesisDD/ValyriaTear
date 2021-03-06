-- Save point particle effect
-- Author: Bertram

-- The effect overall size in pixel. Used to make the map mode draw the effect on time.
effect_collision_width = 6*16;
effect_collision_height = 12*16;

systems = {}

systems[0] =
{
	emitter =
	{ -- The /32 is because of the map mode. this has to be removed by making the map mode follow the standard coords system
		x = 0/32,
		y = -64/32,
		x2 = 0, --useless when using a circle shape
		y2 = 0, --useless when using a circle shape
		center_x = 0,  --useless when using a circle shape
		center_y = 0,  --useless when using a circle shape
		x_variation = 0,
		y_variation = 0,
		radius = 70 /32,
		shape = 'circle outline',
		omnidirectional = false,
		orientation = -1.6, --up
		outer_cone = 0.0,
		inner_cone = 0,
		initial_speed = 25 /8, -- Change this when active
		initial_speed_variation = 15 /8,
		emission_rate = 100, -- Change this when active save point
		start_time = 0,
		emitter_mode = 'looping',
		spin = 'clockwise' -- change this when active
	},

	keyframes =
	{
		{  -- keyframe 1
			size_x = 0.6 /32,
			size_y = 0.6 /32,
			color={1.0, 1.0, 1.0, 0.6},
			rotation_speed = 0.0,
			size_variation_x = 0.2 /32,
			size_variation_y = 0.2 /32,
			rotation_speed_variation = 0.0,
			color_variation = {0.3, 0.0, 0.3, 0.0},
			time = 0.0
		},

		{  -- keyframe 2
			size_x = 0.3 /32,
			size_y = 0.3 /32,
			color = {1.0, 1.0, 1.0, 0.4},
			rotation_speed = 0.0,
			size_variation_x = 0.2 /32,
			size_variation_y = 0.2 /32,
			rotation_speed_variation = 0.0,
			color_variation = {0.3, 0.0, 0.3, 0.0},
			time = 1.0
		}

	},

	animation_frames =
	{
		'img/effects/outlined_circle_small.png'
	},
	animation_frame_times =
	{
		16
	},
	enabled = true,
	blend_mode = 13,
	system_lifetime = 0.3,
	particle_lifetime = 1.8,
	particle_lifetime_variation = 0.0,
	max_particles = 1000,
	damping = 1,
	damping_variation = 0,
	acceleration_x = 0,
	acceleration_y = 0,
	acceleration_variation_x = 0,
	acceleration_variation_y = 0,
	wind_velocity_x = 0,
	wind_velocity_y = 0,
	wind_velocity_variation_x = 0,
	wind_velocity_variation_y = 0,
	wave_motion_used = false,
	wave_length = .5,
	wave_length_variation = 0,
	wave_amplitude = 0,
	wave_amplitude_variation = 0,
	tangential_acceleration = 0,
	tangential_acceleration_variation = 0,
	radial_acceleration = 0,
	radial_acceleration_variation = 0,
	user_defined_attractor = false,
	attractor_falloff = 0.0,
	rotation_used = false,
	rotate_to_velocity = false,
	speed_scale_used = false,
	speed_scale = 0.005,
	min_speed_scale = 1.0,
	max_speed_scale = 20.0,
	smooth_animation = true,
	modify_stencil = false,
	stencil_op = 'INCR',
	use_stencil = false,
	scene_lighting = 0.6,
	random_initial_angle = true
}


-- inner circle
systems[1] =
{
	emitter =
	{
		x = 0/32,
		y = -64/32,
		x2 = 0, --useless when using a circle shape
		y2 = 0, --useless when using a circle shape
		center_x = 0,  --useless when using a circle shape
		center_y = 0,  --useless when using a circle shape
		x_variation = 0,
		y_variation = 0,
		radius = 80 /32,
		shape = 'circle',
		omnidirectional = false,
		orientation = -1.6, --up
		outer_cone = 0.0,
		inner_cone = 0,
		initial_speed = 10 /32,
		initial_speed_variation = 15 /32,
		emission_rate = 100,
		start_time = 0,
		emitter_mode = 'looping',
		spin = 'counter clockwise'
	},

	keyframes =
	{
		{  -- keyframe 1
			size_x = 0.6 /32,
			size_y = 0.6 /32,
			color={0.0, 1.0, 0.0, 0.6},
			rotation_speed = 0.0,
			size_variation_x = 0.2 /32,
			size_variation_y = 0.2 /32,
			rotation_speed_variation = 0.0,
			color_variation = {0.0, 0.0, 0.0, 0.0},
			time = 0.0
		},

		{  -- keyframe 2
			size_x = 0.3 /32,
			size_y = 0.3 /32,
			color = {0.5, 0.5, 1.0, 0.4},
			rotation_speed = 0.0,
			size_variation_x = 0.2 /16,
			size_variation_y = 0.2 /16,
			rotation_speed_variation = 0.0,
			color_variation = {0.0, 0.0, 0.0, 0.0},
			time = 1.0
		}

	},

	animation_frames =
	{
		'img/effects/outlined_circle_small.png'
	},
	animation_frame_times =
	{
		16
	},
	enabled = true,
	blend_mode = 13,
	system_lifetime = 0.3,
	particle_lifetime = 0.8  /8, -- same here, because of map mode
	particle_lifetime_variation = 0.0,
	max_particles = 1000,
	damping = 1,
	damping_variation = 0,
	acceleration_x = 0,
	acceleration_y = 0,
	acceleration_variation_x = 0,
	acceleration_variation_y = 0,
	wind_velocity_x = 0,
	wind_velocity_y = 0,
	wind_velocity_variation_x = 0,
	wind_velocity_variation_y = 0,
	wave_motion_used = false,
	wave_length = .5,
	wave_length_variation = 0,
	wave_amplitude = 0,
	wave_amplitude_variation = 0,
	tangential_acceleration = 300,
	tangential_acceleration_variation = 0,
	radial_acceleration = 0,
	radial_acceleration_variation = 0,
	user_defined_attractor = false,
	attractor_falloff = 0.0,
	rotation_used = false,
	rotate_to_velocity = false,
	speed_scale_used = false,
	speed_scale = 0.005,
	min_speed_scale = 1.0,
	max_speed_scale = 20.0,
	smooth_animation = true,
	modify_stencil = false,
	stencil_op = 'INCR',
	use_stencil = false,
	scene_lighting = 0.6,
	random_initial_angle = true
}
