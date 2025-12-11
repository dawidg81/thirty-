#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "sockets.hpp"

#define SERVER_SALT_LENGTH 64
typedef struct client_s client_t;
typedef struct map_s map_t;
typedef struct rng_s rng_t;
typedef struct namelist_s namelist_t;

typedef struct server_s {
	socket_t socket_fd;
	uint16_t port;

	time_t starttime;

	uint64_t tick;

	client_t *clients;
	size_t num_clients;
	size_t num_spawned_clients;

	map_t *map;
	rng_t *global_rng;

	char salt[SERVER_SALT_LENGTH + 1];
	double last_heartbeat;

	namelist_t *ops;
	namelist_t *banned_users;
	namelist_t *banned_ips;
	namelist_t *whitelist;
} server_t;

bool server_init(void);
void server_tick(void);
void server_shutdown(void);

void server_heartbeat(void);

void server_broadcast(const char *msg, ...) __attribute__((format(printf, 1, 2)));

void server_stop(void);

extern server_t server;
