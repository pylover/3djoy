#include <stdlib.h>
#include <argp.h>

#include "common.h"
#include "cli.h"

#define ARG_TCPBACKLOG  0x1
#define ARG_UNIXBACKLOG 0x2


const char *argp_program_version = VERSION;
const char *argp_program_bug_address = "http://github.com/pylover/3djoy";
static char doc[] = "Serial Interface multiplexer";
static char args_doc[] = "OUTPUT";


/* Options definition */
static struct argp_option options[] = {
    {"input", 'i', "INPUT", 0, 
        "Input device, default: " DEFAULT_INPUT},
	{"baudrate", 'b', "BAUDRATE", 0, 
        "Baudrate, default: " STR(DEFAULT_BAUDRATE)},
    {"verbose", 'v', NULL, 0, 
        "Verbose Mode"},
	{0}
};


/* Parse a single option. */
static int parse_opt(int key, char *arg, struct argp_state *state) {
	switch (key) {
        
        case 'v':
            settings.verbose = 1;
            break;

		case 'b':
			settings.baudrate = atoi(arg);
			break;
        
        case 'i':
            settings.input = arg;
            break;

		case ARGP_KEY_ARG:
			if (state->arg_num >= 1) {
				/* Too many arguments. */
		        argp_usage(state);
			    return ARGP_ERR_UNKNOWN;
			}
			settings.output = arg;
			break;
  
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return OK;
}

static struct argp argp = {options, parse_opt, args_doc, doc};


void cliparse(int argc, char **argv) {
	int err = argp_parse(&argp, argc, argv, 0, 0, 0);
	if (err) {
        perrorf("Cannot parse arguments");
        exit(EXIT_FAILURE);
	}
}

