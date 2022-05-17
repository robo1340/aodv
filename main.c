#include <czmq.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "state.h"

zlistx_t * links; //list of	direct links to	nodes, elements	are	sockets
zsock_t	* node_sink;
zpoller_t * poller;

//  Signal handling ------------------------------------------------
//  Call s_catch_signals() in your application at startup, and then exit
//  your main loop if s_interrupted is ever 1. Works especially well with 
//  zmq_poll.
static int s_interrupted = 0;
static void s_signal_handler (int signal_value)
{
    s_interrupted = 1;
}

static void s_catch_signals (void)
{
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
}

bool establish_links(zlistx_t * links, int link_args_count, char * link_args[]);
void node_main(void);

int	main (int argc,	char * argv[]) {
	
	links = zlistx_new();
	zlistx_set_destructor(links, (zlistx_destructor_fn*) link_destroy);
	
	if(argc >= 3){
		if (strcmp(argv[1],"-a")==0){
			char endpoint[30];
			sprintf(endpoint, "ipc:///tmp/%s\n",argv[2]);
			node_sink = zsock_new_pull(endpoint);
		}
	}
	if(argc >= 6){
		if (strcmp(argv[3],"-l")==0){
			establish_links(links, argc-4, &argv[4]);
		}
	}
	
	//set up the zpoller
	poller = zpoller_new(node_sink, NULL);
	assert(poller);
	zpoller_set_nonstop(poller, false);
	
	node_main();
	
	//shut it down
	zsock_destroy(&node_sink);
	zlistx_destroy(&links);
	
	
	
	/*
	zsock_t *push = zsock_new_push	("inproc://example");
	zsock_t *pull = zsock_new_pull	("inproc://example");
	zstr_send (push, "Hello, World");

	char *string =	zstr_recv (pull);
	puts (string);
	zstr_free (&string);

	zsock_destroy (&pull);
	zsock_destroy (&push);
	*/
	return	0;
}


//main loop entry point for the node app
void node_main(void) {
	printf("node_main()\n");
	int rc;
	int type;
	size_t len = sizeof(type);

	s_catch_signals();
	while(1) {
		void * socket = zpoller_wait(poller, 1000);
		
		if (zpoller_expired(poller)){
			//printf("poller tick\n");
			continue;
		}
		else if (zpoller_terminated(poller)){
			printf("poller interrupted\n");
			break;
		}
		if (s_interrupted) {
			break;
		}
		
		/*
		rc = zmq_getsockopt(socket, ZMQ_TYPE, &type, &len);
		if (rc != 0){
			printf("warning: failed to retrieve socket option\n");
			continue;
		}
		if (socket == frontend_socket) { //a request is coming in from the frontend
			handle_request(socket, my_state);
		}
		else if (type == ZMQ_DEALER) { //a response is coming in from the backend
			handle_response(socket, frontend_socket);
		}
		*/

	}
}

bool establish_links(zlistx_t * links, int link_args_count, char * link_args[]){
	int i;
	
	for (i=0; i<link_args_count; i+=2){
		link_t * new_link = link_new(link_args[i], atof(link_args[i+1]));
		
		//link_destroy(&new_link);
		
		zlistx_add_end(links, new_link);
	}
	
	return true;
}
