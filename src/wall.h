#include "rodeo/collision.h"

void
init_wall(void);

void
deinit_wall(void);

rodeo_collision_2d_world_t *
get_wall_world(void);

rodeo_collision_2d_world_item_t *
new_wall(
    float x,
    float y,
    float width,
    float height
);

bool
coords_inside_wall(
    float x,
    float y
);
