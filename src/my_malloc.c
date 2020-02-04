#include "my_malloc.h"

void search_space(int size, void * ptr) { //cherche espace libre
//return 0 s'il n'a pas trouvé de place
//ptr pointe sur le chunk après lequel il y a l'espace necessaire
}

void init_new_chunk(int size) { //creer un chunk
	void * prev_ptr;
	chunk new_chunk;

	search_space(size, prev_ptr);
	
	if (size >= 4096) {
		//init un chunk utilisé de taille size
	}
	else {
		//init un chunk utilisé de taille size
		//init un chunk non utilisé de taille ( chunk_dispo->size - size ) après le chunk
	}
	//mmap
}


void my_malloc() {
	//size min 4096
	printf("search_space();\n");
	printf("init_new_chunk();\n");
  	printf("hè salut ! J'ai alloué la mémoire :) \n");
}
