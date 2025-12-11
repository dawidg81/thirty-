#include <signal.h>
#include <iostream>
#include <unistd.h>
#include <inttypes.h>
#include <getopt.h>
#include "server.hpp"
#include "sockets.hpp"
#include "blocks.hpp"
#include "commands.hpp"
#include "util.hpp"
#include "config.hpp"
#include "log.hpp"

static void signal_handler(int signum);

static bool running = true;

bool args_disable_colour = false;


int main(int argc, char *argv[]) {
	setbuf(stdout, NULL);

	const char *config_file = NULL;
	int opt;
	while ((opt = getopt(argc, argv, "Cc:")) != -1) {
		switch (opt) {
			case 'C': {
				args_disable_colour = true;
				break;
			}

			case 'c': {
				config_file = optarg;
				break;
			}

			default: {
				std::cout << "Usage: " << argv[0] << " [-C] [-c config]\n";
				return 0;
			}
		}
	}

	log_init();
	log_printf(log_info, "Thirty+");

	config_init(config_file);
	blocks_init();

#ifdef _WIN32
	{
		WSADATA wsadata;
		WSAStartup(MAKEWORD(2, 2), &wsadata);
	}
#endif

	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	if (!server_init()) {
		goto cleanup;
	}

	command_readline_init();
	log_printf(log_info, "Ready! Type 'help' for a list of commands.");

	while (running) {
		double start = get_time_s();
		server_tick();
		double end = get_time_s();
		if (end - start > 1.0 / 20.0) {
			log_printf(log_info, "Server lagged: Tick %" PRIu64 " took too long (%f ms)", server.tick - 1, (end - start) * 1000.0);
		}

		usleep(1000000 / 20);
	}

cleanup:
	command_readline_shutdown();
	server_shutdown();
	config_destroy();
	log_shutdown();

#ifdef _WIN32
	WSACleanup();
#endif

	return 0;
}

void signal_handler(int signum) {
	log_printf(log_info, "Received signal %d, will exit.", signum);
	server_stop();
}

void server_stop(void) {
	running = false;
}

