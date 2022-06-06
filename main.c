#include <czmq.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "state.h"
#include "frame_handler.h"
#include "frame_sender.h"

#include "aodv.h"

zhashx_t * links; //list of	direct links to	nodes, keys are uint64_t addresses, values are link_t
zsock_t	* node_sink;
zpoller_t * poller;
uint8_t my_type;
uint64_t my_addr;

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

static bool establish_links(zhashx_t * links, int link_args_count, char * link_args[]);
static void node_main(void);
static bool send_frame_proc(uint64_t last_send_time);

int	main (int argc,	char * argv[]) {
	
	#define TYPE_STATIC 1
	#define TYPE_SOURCE 2
	
	
	srand(zclock_time());
	
	links = zhashx_new();
	zhashx_set_destructor(links, (zhashx_destructor_fn*) link_destroy);
	zhashx_set_key_duplicator(links, (zhashx_duplicator_fn*) address_create);
	//zlistx_set_destructor(links, (zlistx_destructor_fn*) link_destroy);
	
	int arg_ind = 1;
	//parse the node type (2 args)
	if (strcmp(argv[arg_ind],"-t")==0){
		if (strcmp(argv[arg_ind+1],"source")==0) {
			my_type = TYPE_SOURCE;
			printf("setting my type to SOURCE\n");
		}
		else if (strcmp(argv[arg_ind],"static")==0) {
			my_type = TYPE_STATIC;
			printf("setting my type to STATIC\n");
		}
		else {
			my_type = TYPE_STATIC;
			printf("default type selected: STATIC\n");
		}
	}
	//parse the args for node address
	arg_ind = 3;	
	if (strcmp(argv[arg_ind],"-a")==0){
		char * my_addr_str = argv[arg_ind+1];
		char endpoint[30];
		sprintf(endpoint, "ipc:///tmp/%s\n", my_addr_str);
		node_sink = zsock_new_pull(endpoint);

		char * ptr;
		my_addr = strtoul(my_addr_str, &ptr, 16);
		aodv_init(my_addr);
		printf("my addr: %lu\n", my_addr);
	}
	//parse the args for neighbor addresses
	arg_ind = 5;
	if (strcmp(argv[arg_ind],"-l")==0){
		establish_links(links, argc-(arg_ind+1), &argv[arg_ind+1]);
	}
	
	//set up the zpoller
	poller = zpoller_new(node_sink, NULL);
	assert(poller);
	zpoller_set_nonstop(poller, false);
	
	node_main();
	
	//shut it down
	zsock_destroy(&node_sink);
	zhashx_destroy(&links);
	
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
			if (my_type == TYPE_SOURCE){
				if (send_frame_proc(last_send_time_ms) == true){
					last_send_time_ms = zclock_time();
				}
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
			if (zmsg_size(msg) >= 2){
				uint64_t * addr = (uint64_t *)zframe_data(zmsg_first(msg)); //the address of the immediate note that send this message
				zframe_t * frame = zmsg_next(msg);
				//printf("%lu\n",zframe_size(frame));
				handle_received_frame(my_addr, *addr, links, zframe_data(frame),zframe_size(frame));
				//zframe_destroy(&frame);
				zmsg_destroy(&msg);
				
			}
		}
	}
}

static bool establish_links(zhashx_t * links, int link_args_count, char * link_args[]){
	int i;
	
	for (i=0; i<link_args_count; i+=2){
		link_t * new_link = link_new(link_args[i], atof(link_args[i+1]));
		
		//link_destroy(&new_link);
		char * ptr;
		uint64_t * addr = malloc(sizeof(uint64_t));
		*addr = strtoul(link_args[i], &ptr, 10);
		printf("new link at address %lu\n", *addr);
		zhashx_insert(links, addr , new_link);
		//zlistx_add_end(links, new_link);
	}
	
	return true;
}

#define SEND_PERIOD_MS 2000

static bool send_frame_proc(uint64_t last_send_time) {
	int i;
	
	if ((zclock_time() - last_send_time) > SEND_PERIOD_MS) {

		uint64_t next_hop;
		uint64_t dst_addr = (uint64_t)6;
		if (aodv_get_next_hop_address(&next_hop, dst_addr)){
			link_t * link = (link_t *)zhashx_lookup(links, &next_hop);
			if (link != NULL){
				uint8_t data[5] = {0,1,2,3,4};
				send_frame(my_addr, link, data, sizeof(data));
				printf("successfully sent data message!\n");
			} else {
				printf("warning: couldn't find neighbor in known links\n");	
			}
		} else {
			rreq_t rreq;
			if (aodv_generate_rreq(&rreq, dst_addr)) {
				print_rreq(&rreq);
				broadcast_frame(my_addr, links, (uint8_t *)&rreq, sizeof(rreq_t));
				printf("successfully started RREQ\n");
			} else {
				printf("couldn't generate route request!\n");	
			}
			
		}
		

	}
	return false;
	
	
	/*
	if ((zclock_time() - last_send_time) > SEND_PERIOD_MS) {
		uint8_t data[5] = {0,1,2,3,4};

		zlistx_t * addrs = zhashx_keys(links);
		int ind = rand() % zlistx_size(addrs);
		uint64_t * addr = (uint64_t *)zlistx_first(addrs);
		printf("ind %d\n", ind);
		for (i=1; i<=ind; i++){
			if (ind==0) {break;}
			addr = (uint64_t *)zlistx_next(addrs);	
			printf("%lu\n",*addr);
		}
		if (addr == NULL) {return false;}
		
		printf("sending message to %lu\n", *addr);
		void * link = zhashx_lookup(links, addr);
		
		zlistx_destroy(&addrs);
		if (link != NULL) {
			send_frame((link_t*)link, data, sizeof(data));
		}

	}
	return false;
	*/
	
	
}



