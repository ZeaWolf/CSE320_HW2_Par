/*********************/
/* errmsg.c          */
/* for Par 3.20      */
/* Copyright 1993 by */
/* Adam M. Costello  */
/*********************/

/* This is ANSI C code. */


#include "errmsg.h"  /* Makes sure we're consistent with the declarations. */


// char errmsg[163];

static char *errmsg = NULL;

const char * const outofmem = "Out of memory.\n";

void set_error(char *msg){
	if(msg == NULL){
		return;
	}

	size_t msg_size = 0, i;

	while(*(msg+msg_size)){
		msg_size++;
	}

	if(is_error()){
		clear_error();
	}

	errmsg = malloc(msg_size+1);
	if(errmsg == NULL){
		fputs(outofmem, stderr);
		return;
	}

	for(i = 0; i < msg_size; i++){
		*(errmsg+i) = *(msg+i);
	}

	*(errmsg+msg_size) = '\0';

	return;

	// size_t msg_size = 0, i;

	// while(*(msg+msg_size)){
	// 	msg_size++;
	// }

	// if(is_error()){
	// 	while(*(errmsg+old_size)){
	// 		old_size++;
	// 	}
	// }

	// if(is_error()){
	// 	errmsg = realloc(errmsg, msg_size+old_size+1);
	// }else{
	// 	errmsg = malloc(msg_size+1);
	// }

	// // fail to allocate memory for errmsg
	// if(errmsg == NULL){
	// 	fputs(outofmem, stderr);
	// 	return;
	// }

	// for(i = 0; i < msg_size; i++){
	// 	*(errmsg+old_size+i) = *(msg+i);
	// }

	// *(errmsg+old_size+msg_size) = '\0';

	// return;
}

int is_error(){

	if(errmsg==NULL) return 0;

	return 1;
}

int report_error(FILE *file){

	if(!is_error()) return 0;

	if(fputs(errmsg, file) == EOF) return -1;

	return 0;
}

void clear_error(){

	if(is_error()){
		free(errmsg);
		errmsg = NULL;
	}
}