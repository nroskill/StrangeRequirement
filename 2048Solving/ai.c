#include <time.h>
#include <stdlib.h>
#include "ai.h"
#include "utils.h"
#include "priority_queue.h"

struct heap h;

void initialize_ai(){
	heap_init(&h);
}

move_t get_next_move( uint8_t board[SIZE][SIZE], int max_depth, propagation_t propagation ){
	
	node_t* current = NULL, * temp, * insert, * start;
	int max_value, max_count, foot;
	
	int priority[4] = {0};
	int num_childs[4] = {0};
	move_t best_action;
	move_t direction;

	start = (node_t*)malloc(sizeof(node_t));
	start -> priority = 0;
	start -> depth = 0;
	memcpy(start -> board, board, SIZE * SIZE * sizeof(uint8_t));
	start -> parent = NULL;
	start -> move = -1;

	heap_push(&h, start);

	while(h.count){
		current = heap_delete(&h);

		if(current -> depth < max_depth)
			for(direction = 0; direction < 4; direction++){

				temp = (node_t*)malloc(sizeof(node_t));
				memcpy(temp, current, sizeof(node_t));

				if(execute_move_t(temp -> board, &temp -> priority, direction)){
					if(temp -> move == -1){
						temp -> move = direction;
					}
					temp -> depth += 1;
					if(propagation == max){
						priority[temp -> move] =  priority[temp -> move] > temp -> priority ? priority[temp -> move] : temp -> priority;
					}
					else{
						priority[temp -> move] += temp -> priority;
						num_childs[temp -> move] += 1;
					}
					if(temp -> depth < max_depth)
					for(foot = 0; foot < SIZE * SIZE; foot++){
						if(temp -> board[foot / SIZE][foot % SIZE]){
							continue;
						}
						insert = (node_t*)malloc(sizeof(node_t));
						memcpy(insert, temp, sizeof(node_t));
						insert -> board[foot / SIZE][foot % SIZE] = 1;
						heap_push(&h, insert);

						insert = (node_t*)malloc(sizeof(node_t));
						memcpy(insert, temp, sizeof(node_t));
						insert -> board[foot / SIZE][foot % SIZE] = 2;
						heap_push(&h, insert);
					}
				}
				free(temp);
			}

		free(current);
	}
	
	max_value = 0;
	max_count = 0;

	for(direction = 0; direction < 4; direction++){
		if(num_childs[direction] != 0){
			priority[direction] /= num_childs[direction];
		}
		if(priority[direction] > max_value){
			max_value = priority[direction];
			max_count = 1;
		}
		else if(priority[direction] == max_value){
			max_count++;
		}
	}

	max_count = rand()%max_count;
	best_action = 0;

	while(1){
		if(max_value == priority[best_action]){
			max_count--;
		}
		if(max_count < 0 && max_value == priority[best_action])
			break;
		best_action++;
	}

	return best_action;
}
