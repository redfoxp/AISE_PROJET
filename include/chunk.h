#ifndef CHUNK_H
#define CHUNK_H

#define TRUE 	1
#define FALSE 	0

typedef struct CHUNK {
	int used;
	
	int size;
	int prev_size;

	chunk * prev_chunk_stack;
	chunk * next_chunk_stack;

	chunk * prev_chunk_size;
	chunk * next_chunk_size;
} chunk;

#endif