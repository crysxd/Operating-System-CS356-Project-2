/*
 * pager.c
 * Author: Christian Würthner
 * Description: pager for project 2
 */

#include "pager.h"

void pager_tick() {
	/* If there are ticks to consume (meaning loading a page is in progress) */
	if(pager_consume_ticks > 0) {
		/* Consume one tick */
		pager_consume_ticks--;

		printf("[PAGER] %" PRIu64 " ticks left for current load operation\n", 
			pager_consume_ticks);

		/* If the loading process id done */
		if(pager_consume_ticks == 0) {
			/* Unblock process */
			scheduler_ready(load_request_queue_head->process);

			/* Remove request from queue */
			load_request_t *r = load_request_queue_head;
			load_request_queue_head = load_request_queue_head->next;
			free(r);

			/* If there are any other requests in the queue, handle the next */
			if(load_request_queue_head != NULL) {
				pager_perform_next_load();
			}
		}
	}
}

/* Loads the page for the current instruction of the given process */
void pager_load(pcb_t *process) {
	printf("[PAGER] Request to load page %d\n", 
		process->instruction_pointer->address);

	/* Block the currently running process */
	scheduler_block(process);

	/* Create laod request */
	load_request_t *r = malloc(sizeof(load_request_t));
	r->process = process;
	r->next = NULL;

	/* Append the request to the queue of pending requests or set as head if
	   queue is empty */
	if(load_request_queue_head == NULL) {
		load_request_queue_head = r;
	} else {
		load_request_queue_tail->next = r;
	}

	/* Set r as new tail */
	load_request_queue_tail = r;

	/* If r is the first element in the queue, this means there is currently 
	   no other request being processed -> start loading immediately */
	if(load_request_queue_head == r) {
		pager_perform_next_load();
	}

}

/* Performs the next load operation from the load request queue */
void pager_perform_next_load() {
	/* Assure that the queue is not empty */
	if(load_request_queue_head == NULL) {
		return;
	}

	/* Print status */
	printf("[PAGER] Loading page %d\n", 
		load_request_queue_head->process->instruction_pointer->address);

	/* Print page table */
	print_inverted_page_table();
	
	/* Special field needed for LRU algorithm */
	#ifdef PAGER_ALGORITHM_SC
		uint32_t absolute_oldest_page = 0;
	#endif
	
	/* Search for a free frame or the frame to replace. The replacement function
	   is based on which preprocessor directive is defined */
	uint32_t frame_to_replace = 0;
	uint32_t i = 0;
	for(i=0; i<MEMORY_FRAME_COUNT; i++) {
		/* If there is an empty frame, cancel search and use it */
		if(inverted_page_table[i].empty == true) {
			frame_to_replace = i;
			printf("[PAGER] Found empty frame at position %d\n", i);
			break;
		}

		/* If we use FIFO alogrithm, search for the oldest frame */
		#ifdef PAGER_ALGORITHM_FIFO
		if(inverted_page_table[i].time_loaded <
			inverted_page_table[frame_to_replace].time_loaded) {
			frame_to_replace = i;
		}
		#endif

		/* If we use LRU algorithm, search for the last recently used frame */
		#ifdef PAGER_ALGORITHM_LRU
		if(inverted_page_table[i].time_used < 
			inverted_page_table[frame_to_replace].time_used) {
			frame_to_replace = i;
		}
		#endif

		/* If we use SC, iterate over all frames and find the oldest one with a 
		   cleared used bit */
		#ifdef PAGER_ALGORITHM_SC
		uint32_t oldest_page = frame_to_replace;
		for(uint32_t j=0; j<MEMORY_FRAME_COUNT; j++) {
			if(inverted_page_table[j].time_loaded <
				inverted_page_table[oldest_page].time_loaded && 
				inverted_page_table[j].time_loaded >= 
				inverted_page_table[frame_to_replace].time_loaded) {
					oldest_page = j;
			}
		}

		/* Save as frame to replace */
		frame_to_replace = oldest_page;

		/* Save the frame as absolut oldest if we are in the first iteration */
		if(i == 0) {
			absolute_oldest_page = oldest_page;
		}

		/* If the used bit is not set, we found our victim */
		if(inverted_page_table[oldest_page].used == false) {
			break;
		}

		/* Clear used bit */
		inverted_page_table[frame_to_replace].used = false;
		#endif

		#ifdef PAGER_ALGORITHM_OPT
		#endif
	}

	/* If we used SC algorithm and there was no break, we must use the absolut
	   oldest page (this happens when all used bits are set) */
	#ifdef PAGER_ALGORITHM_SC
		if(i == MEMORY_FRAME_COUNT) {
			frame_to_replace = absolute_oldest_page;
		}
	#endif

	printf("[PAGER] Replacing page in frame %d\n", frame_to_replace);

	/* replace frame */
	inverted_page_table[frame_to_replace].owner_pid = 
		load_request_queue_head->process->pid;
	inverted_page_table[frame_to_replace].page_number = 
		load_request_queue_head->process->instruction_pointer->address;
	inverted_page_table[frame_to_replace].time_loaded = cpu_time + 1000;
	inverted_page_table[frame_to_replace].time_used = cpu_time + 1000;
	inverted_page_table[frame_to_replace].used = false;
	inverted_page_table[frame_to_replace].empty = false;

	/* Consume the next 1000 ticks */
	pager_consume_ticks = 10;
}