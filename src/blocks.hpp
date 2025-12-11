#pragma once
#include <stdint.h>

typedef struct map_s map_t;

typedef void (*blocktickfunc_t)(map_t *map, size_t x, size_t y, size_t z, uint8_t block);

enum {
	air,
	stone,
	grass,
	dirt,
	cobblestone,
	wood_planks,
	sapling,
	bedrock,
	water,
	water_still,
	lava,
	lava_still,
	sand,
	gravel,
	gold_ore,
	iron_ore,
	coal_ore,
	wood,
	leaves,
	sponge,
	glass,
	red_wool,
	orange_wool,
	yellow_wool,
	lime_wool,
	green_wool,
	aquagreen_wool,
	cyan_wool,
	blue_wool,
	purple_wool,
	indigo_wool,
	violet_wool,
	magenta_wool,
	pink_wool,
	black_wool,
	grey_wool,
	white_wool,
	dandelion,
	rose,
	brown_mushroom,
	red_mushroom,
	gold_block,
	iron_block,
	double_slab,
	slab,
	bricks,
	tnt,
	bookshelf,
	mossy_cobblestoe,
	obsidian,

    num_blocks
};

typedef struct blockinfo_s {
	bool solid : 1;
	bool block_light : 1;
	bool liquid : 1;
	bool op_only_place : 1;
	bool op_only_break : 1;

	uint32_t colour;

	blocktickfunc_t tickfunc;
	uint64_t ticktime;
	blocktickfunc_t random_tickfunc;
	blocktickfunc_t placefunc;
	blocktickfunc_t breakfunc;
} blockinfo_t;

extern blockinfo_t blockinfo[num_blocks];

void blocks_init(void);
