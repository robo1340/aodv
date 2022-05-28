#include <czmq.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "state.h"
#include "frame_handler.h"
#include "frame_sender.h"

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
	sigaction (SIGKILL, &action, NULL);
}

static bool establish_links(zlistx_t * links, int link_args_count, char * link_args[]);
static void node_main(void);
static bool send_frame_proc(uint64_t last_send_time);

int	main (int argc,	char * argv[]) {
	
	srand(zclock_time());
	
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
	
	return	0;
}


//main loop entry point for the node app
static void node_main(void) {
	printf("node_main()\n");
	int rc;
	int type;
	size_t len = sizeof(type);
	
	uint64_t last_send_time_ms = zclock_time();

	//s_catch_signals();
	while(1) {
		void * socket = zpoller_wait(poller, 1000);
		
		if (zpoller_expired(poller)){
			//printf("poller tick\n");
			if (send_frame_proc(last_send_time_ms) == true){
				last_send_time_ms = zclock_time();
			}
			continue;
		}
		else if (zpoller_terminated(poller)){
			printf("poller interrupted\n");
			break;
		}
		//if (s_interrupted) {break;}
		
		if (socket != NULL){ //handle received message
			zmsg_t * msg = zmsg_recv(socket);
			if (zmsg_size(msg) > 0){
				zframe_t * frame = zmsg_first(msg);
				handle_received_frame(zframe_data(frame),zframe_size(frame));
				//zframe_destroy(&frame);
				zmsg_destroy(&msg);
				
			}
		}

	}
}

static bool establish_links(zlistx_t * links, int link_args_count, char * link_args[]){
	int i;
	
	for (i=0; i<link_args_count; i+=2){
		link_t * new_link = link_new(link_args[i], atof(link_args[i+1]));
		
		//link_destroy(&new_link);
		
		zlistx_add_end(links, new_link);
	}
	
	return true;
}

#define SEND_PERIOD_MS 2000

static bool send_frame_proc(uint64_t last_send_time) {
	int i;
	
	if ((zclock_time() - last_send_time) > SEND_PERIOD_MS) {
		uint8_t data[5] = {0,1,2,3,4};

		int ind = rand() % zlistx_size(links);
		void * link = zlistx_first(links);
		if (link == NULL) {return false;}
		for (i=0; i<=ind; i++) {
			if (link == NULL) {return false; }
			else if (i == ind) {
				send_frame((link_t *)link,data,sizeof(data));
				return true;
			}
			link = zlistx_next(links);	
		}

	}
	return false;
	
}



