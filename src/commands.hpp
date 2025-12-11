#pragma once

typedef struct client_s client_t;

void command_execute(client_t *client, const char *command);

void command_readline_init(void);
void command_tick_readline(void);
void command_readline_shutdown(void);

extern client_t command_standin;
