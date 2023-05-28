
#include "rodeo.h"
#include <inttypes.h>
#include "input.h"
#include "player.h"
#include "bullet.h"
#include "enemies.h"
#include "wall.h"
#include "rodeo/collision.h"

const uint16_t window_width = 640;
const uint16_t window_height = 480;


cstr renderer;
float time_var;

rodeo_audio_sound_t *scratch = NULL;
rodeo_audio_music_t *music = NULL;

typedef
struct
{
	float x;
	float y;
}
summon_t;

summon_t box1_position = { .x = 100, .y = 100 };
summon_t box2_position = { .x = 100, .y = 100 };

float orc_size[] = {13.0f * 2.0f, 19.0f * 2.0f};

rodeo_vector2_t units[UINT16_MAX][2] = {0};
uint16_t num_of_units = 0;

const rodeo_color_RGBAFloat_t red =
{ 
	.colors.red = 1.0f, .colors.green = 0.0f, .colors.blue = 0.0f,
	.colors.alpha = 1.0f
};
const rodeo_color_RGBAFloat_t green =
{ 
	.colors.red = 0.0f, .colors.green = 1.0f, .colors.blue = 0.0f,
	.colors.alpha = 1.0f
};
const rodeo_color_RGBAFloat_t blue =
{ 
	.colors.red = 0.0f, .colors.green = 0.0f, .colors.blue = 1.0f,
	.colors.alpha = 1.0f
};
const rodeo_color_RGBAFloat_t pink =
{ 
	.colors.red = 1.0f, .colors.green = 0.0f, .colors.blue = 1.0f,
	.colors.alpha = 1.0f
};

const rodeo_color_RGBAFloat_t red_clear =
{ 
	.colors.red = 1.0f, .colors.green = 0.0f, .colors.blue = 0.0f,
	.colors.alpha = 0.5f
};
const rodeo_color_RGBAFloat_t green_clear =
{ 
	.colors.red = 0.0f, .colors.green = 1.0f, .colors.blue = 0.0f,
	.colors.alpha = 0.5f
};
const rodeo_color_RGBAFloat_t blue_clear =
{ 
	.colors.red = 0.0f, .colors.green = 0.0f, .colors.blue = 1.0f,
	.colors.alpha = 0.5f
};
const rodeo_color_RGBAFloat_t pink_clear =
{ 
	.colors.red = 1.0f, .colors.green = 0.0f, .colors.blue = 1.0f,
	.colors.alpha = 0.5f
};

rodeo_collision_2d_world_t world_orc;
rodeo_collision_2d_world_t world_other;

world_id orc_collision_id;
world_id box_collision_ids[2] = { 0 };

void collision_resolve(
		rodeo_collision_2d_world_item_t *a,
		rodeo_collision_2d_world_item_t *b
) {
	rodeo_log(
		rodeo_logLevel_info,
		"%d collided with %d",
		a->id.id, b->id.id
	);
	rodeo_collision_2d_world_item_destroy(b);
}

void
main_loop(void)
{
	if(rodeo_frame_count_get() % 10 == 0)
	{
		/* for testing
		rodeo_input_scene_unregister_callback(
			*summon_units_input,
			&scene,
			register_type_e
		);

		rodeo_input_scene_register_callback(
			*summon_units_input,
			&scene,
			register_type_q
		);
		*/

		time_var = rodeo_frame_perSecond_get();
	}

	mrodeo_frame_do()
	{
		parse_player_input();
		move_player();
		rodeo_collision_2d_world_compare_self(&world_orc, collision_resolve);

		if(*(bool*)play_sound_input(NULL, NULL))
		{
			rodeo_audio_sound_play(scratch);
		}
		if(*(bool*)play_sound_input2(NULL, NULL))
		{
			rodeo_audio_music_play_fadeIn(music, 1000);
		}
		if(*(bool*)play_sound_input3(NULL, NULL))
		{
			rodeo_audio_music_stop_fadeOut(1000);
		}

		if(*(bool*)summon_units_input(NULL, NULL)) 
		{
			player_shoot(get_player_bullet_world());
		}

		rodeo_rectangle_draw(
			&(rodeo_rectangle_t){ 100, 100, 50, 50 },
			&red
		);
	   rodeo_rectangle_draw(  
	   		&(rodeo_rectangle_t){ 100, 160, 50, 50 },
	   		&green
	   );
	   rodeo_rectangle_draw(  
	   		&(rodeo_rectangle_t){ 160, 100, 50, 50 },
	   		&blue
	   );
	   rodeo_rectangle_draw(  
	   		&(rodeo_rectangle_t){ 160, 160, 50, 50 },
	   		&pink
	   );

	   rodeo_rectangle_draw(  
	   		&(rodeo_rectangle_t){ 300, 300, 50, 50 },
	   		&red_clear
	   );
	   rodeo_rectangle_draw(  
	   		&(rodeo_rectangle_t){ 310, 310, 50, 50 },
	   		&green_clear
	   );
	   rodeo_rectangle_draw(  
	   		&(rodeo_rectangle_t){ 320, 320, 50, 50 },
	   		&blue_clear
	   );
	   rodeo_rectangle_draw(  
	   		&(rodeo_rectangle_t){ 330, 330, 50, 50 },
	   		&pink_clear
	   );

	   // debug test for collisions
       /*
	   for(uint64_t i = 0; i < (sizeof(box_collision_ids) / sizeof(box_collision_ids[0])); ++i)
	   {
		   rodeo_collision_2d_world_item_t *box = rodeo_collision_2d_world_item_get_by_id(box_collision_ids[i]);
		   if(box != NULL)
		   {
		   rodeo_rectangle_draw(
				&(rodeo_rectangle_t){ box->x, box->y, box->width, box->height },
				&pink
			);
		   }
	   }
	   */

	   enemies_attempt_weapon_fire();
	   attempt_random_enemy_spawn((rodeo_rectangle_t){ 0, 0, window_width, window_height });

	   move_bullets();
	   move_enemies();
	   group_follow_target(get_player_position());
	   draw_bullets();
	   draw_player();
	   draw_enemies();
	   detect_bullet_enemy_collisions();
	   detect_bullet_wall_collisions();
	   detect_player_enemy_collisions();
	   detect_player_wall_collisions();

		rodeo_debug_text_draw(
			1,
			1,
			" using %s renderer ",
			cstr_str(
				&renderer
			)
		);

		rodeo_debug_text_draw(
			2,
			2,
			" frame count: %"PRIu64" ",
			rodeo_frame_count_get()
		);

		rodeo_debug_text_draw(
			2,
			3,
			" fps: %.2f ",
			time_var
		);
	}
}

int
main(void)
{
	register_inputs();
	
	rodeo_log(
		rodeo_logLevel_info,
		"Testing logging... It seems to work!"
	);
	rodeo_log(
		rodeo_logLevel_warning,
		"Testing warning log level... It seems to work!"
	);
	rodeo_log(
		rodeo_logLevel_error,
		"Testing error log level... It seems to work!"
	);
	mrodeo_window_do(window_height, window_width, cstr_lit("Rodeo Window"))
	{
		renderer = rodeo_renderer_name_get();
		rodeo_frame_limit_set(60);

		world_orc = rodeo_collision_2d_world_create();
		world_other = rodeo_collision_2d_world_create();

		/*
		rodeo_collision_2d_world_item_t test_collision_params = {
			.x = 320,
			.y = 240,
			.width = orc_size[0],
			.height = orc_size[1]
		};
		box_collision_ids[0] = rodeo_collision_2d_world_item_create(&world_orc, test_collision_params)->id;
		rodeo_collision_2d_world_item_t test2_collision_params = {
			.x = 0,
			.y = 240,
			.width = orc_size[0],
			.height = orc_size[1]
		};
		box_collision_ids[1] = rodeo_collision_2d_world_item_create(&world_orc, test2_collision_params)->id;
		*/

		scratch = rodeo_audio_sound_create_from_path(cstr_lit("assets/sample.wav"));
		music = rodeo_audio_music_create_from_path(cstr_lit("assets/music.ogg"));

		init_bullets();
		init_player();
		init_enemies();
		init_wall();
		//spawn_enemy(240, 240);
		//spawn_enemy(100, 100);
		//spawn_enemy(300, 100);
		//spawn_enemy(200, 330);
		new_wall(0, -10, window_width, 10);
		new_wall(0, window_height, window_width, 10);
		new_wall(-10, 0, 10, window_height);
		new_wall(window_width, 0, 10, window_height);

		rodeo_mainLoop_run(
			main_loop
		);

		deinit_player();

		rodeo_collision_2d_world_destroy(&world_orc);
		rodeo_collision_2d_world_destroy(&world_other);

		deinit_enemies();
	}

	unregister_inputs();

	return 0;
}
