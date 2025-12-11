#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	char code;
	uint8_t r, g, b, a;
} textcolour_t;

typedef struct {
	struct {
		char *name;
		char *motd;
		uint16_t port;
		bool public;
		bool offline;
		unsigned max_players;
		bool enable_whitelist;
		bool enable_old_clients;

		char **allowed_web_proxies;
		size_t num_proxies;

		char *heartbeat_url;
	} server;

	struct {
		char *name;
		unsigned width, depth, height;
		char *generator;
		bool random_seed;
		int seed;
		char *image_path;
		int image_interval;
	} map;

	struct {
		char fixed_salt[17];
		bool disable_save;
		bool dump_connection_packets;
	} debug;

	struct {
		int map_compression;
	} network;

	textcolour_t *colours;
	size_t num_colours;
} config_t;

void config_init(const char *config_path);
void config_destroy(void);

const textcolour_t *config_find_colour(char code);

extern config_t config;

