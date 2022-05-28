/**
 * @file print_util.c
 * @author Haines Todd
 * @date 4/28/2021
 * @brief a collection of helpful printer functions
 */

#include "print_util.h"

void printArrHex(const uint8_t *ptr, int len){
	int i;
	printf("[");
	for (i=0;i<len;i++){
		printf("0x%2x",*ptr); ptr++;
		if (i<(len-1)) {printf(",");}
	}
	printf("]\n");
}

void print_uint32_arr(const uint32_t *ptr, size_t len){
	int i;
	printf("[");
	for (i=0; i<len;i++){
		printf("%4u",ptr[i]);
		if (i<(len-1)) {printf(",");}
	}
	printf("]\n");
}
