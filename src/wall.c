#include "wall.h"

static rodeo_collision_2d_world_t collision_wall_world;

void
init_wall(void)
{
	collision_wall_world = rodeo_collision_2d_world_create();
}

void
deinit_wall(void)
{
	rodeo_collision_2d_world_destroy(&collision_wall_world);
}

rodeo_collision_2d_world_t *
get_wall_world(void)
{
	return &collision_wall_world;
}

rodeo_collision_2d_world_item_t *
new_wall(
    float x,
    float y,
    float width,
    float height
)
{
	return rodeo_collision_2d_world_item_create(
			&collision_wall_world,
			(rodeo_collision_2d_world_item_t)
			{
				.x = x,
				.y = y,
				.width = width,
				.height = height
			});
}

bool
coords_inside_wall(
    float x,
    float y
)
{
	c_foreach(i, cvec_collision_2d_world_item, collision_wall_world) {
		if (x >= i.ref->x && x <= i.ref->x + i.ref->width &&
				y >= i.ref->y && y <= i.ref->y + i.ref->height) {
			return true;
		}
	}
	return false;
}
