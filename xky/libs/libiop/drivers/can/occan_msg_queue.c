#include <occan_msg_queue.h>

CANMsg empty_msg(){
	CANMsg msg ={
		.extended = 0,
		.rtr = 0,
		.sshot = 0,
		.len = 0,
		.data = {0, 0, 0, 0, 0, 0, 0, 0},
		.id = 0
	};
	return msg;
}

/* Clear all entries on the fifo and
 * control data */

uint8_t occan_fifo_clear(occan_fifo *queue ){
	int i, j;

	for(i = 0; i < queue->max; i++){
		queue->fifo[i].extended = 0;
		queue->fifo[i].rtr = 0;
		queue->fifo[i].id = 0;
		queue->fifo[i].len = 0;
		queue->fifo[i].sshot = 0;
		/* Clear all bytes of data */
		for(j = 0; j < 8; j++){
			queue->fifo[i].data[j] = 0;
		}
	}
	queue->cnt = 0;
	queue->ovcnt = 0;
	queue->last = 0;
	queue->next = 0;

	return 1;
}

/* TODO force put option for when the queue is full */
uint8_t occan_fifo_put(occan_fifo *queue, CANMsg *new, int force){
	int i;
	if(queue->max -queue->cnt < 1){
		/* queue is full */
		return 0;
	}

	queue->fifo[queue->last].extended = new->extended;
	queue->fifo[queue->last].rtr = new->rtr;
	queue->fifo[queue->last].sshot = new->sshot;
	queue->fifo[queue->last].id = new->id;
	queue->fifo[queue->last].len = new->len;
	for(i = 0; i < 8; i++){
		queue->fifo[queue->last].data[i] = new->data[i];
	}
	queue->cnt++;

	if(queue->last + 1 >= queue->max){
		queue->last = 0;
	}else{
		queue->last++;
	}
	return 1;
}

CANMsg *occan_fifo_get(occan_fifo *queue){
	if(queue->cnt == 0 ){
		/* Queue is empty */
//		return empty_msg();
		return NULL;
	}
	int tmp = queue->next;
	queue->cnt--;
	if(queue->next  >= queue->max){
		queue->next = 0;
	}
	return &(queue->fifo[tmp]);
}

uint8_t occan_fifo_full(occan_fifo *queue) {
	if(queue->max == queue->cnt){
		return 1;
	}
	return 0;
}

uint8_t occan_fifo_empty(occan_fifo *queue){
	if(queue->cnt == 0){
		return 1;
	}
	return 0;
}
