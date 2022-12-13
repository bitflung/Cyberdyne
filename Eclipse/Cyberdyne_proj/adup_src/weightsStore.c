#include "weightsStore.h"


/* expected usage model:
 * int id = makeWeights(<hardcoded params for each of two weights formats>);
 * if(id > 0) FAIL();
 *
 * printf(getName(id)); // print the weights object name to console
 * int kernelCnt, biasCnt;
 * getCounts(&kernelCnt, &biasCnt); // get the number of each vector type
 *
 * uint32_t *kernel;
 * uint32_t *sz;
 * for(int k=0; k<kernelCnt; k++){
 * 	kernel = getKernel(id, k, &kernel, &sz);
 * 	for(int i=0; i<sz; i++){
 * 		kernel[i]=i; // store garbage data to all kernels in weights object
 * 	}
 *
 * 	uint32_t *bias;
 * 	for(int b=0; b<biasCnt; b++){
 * 		bias = getBias(id, b, &bias, &sz);
 * 		for(int i=0; i<sz; i++){
 * 			bias[i]=i; // store junk data to all biases in weights object
 * 		}
 * 	}
 * }
 *
 * freeWeights(id); // free all memory used by this weights object
 */

weightsTracker *tracker=NULL;
extern C_ADUP *adup;

void adup_weights_handler(msg_t *msg){
	// this handler to be registered as a callback in ADUP and automatically called when we RX a msg that isn't handled by the stack itself
	//if(msg->cmd!='A') adup->error("Unsupported command prefix");
	switch(msg->cmd){
	case('M'):
		//printf("make_handler() ->\n");
		make_handler(msg);
		//printf("	<- make_handler()\n");
		break;
	case('S'):
		status_handler(msg);
		break;
	default:
		adup->error("Unsupported command prefix");
		break;
	}
	adup->TX(msg);
}

/******************************************************************************/
bool makeTracker(void){
	bool ret = true;

	if(tracker!=NULL){
		printf("exception: only one tracker allowed\n");
		return false;
	}
	else{
		tracker = malloc(sizeof(weightsTracker));
		if(tracker==NULL) return false; // never allocated any memory, nothing to free, just return

		tracker->exists = malloc(sizeof(bool)*tracker->maxEntries);
		tracker->valid = malloc(sizeof(bool)*tracker->maxEntries);
		tracker->weights = malloc(sizeof(uint32_t**)*tracker->maxEntries);

		if(tracker->exists==NULL) ret=false;
		if(tracker->valid==NULL) ret=false;

		if(ret==false){
			free(tracker->exists);
			free(tracker->valid);
			return ret;
		}

		tracker->maxEntries=4;
		for(int i=0; i<tracker->maxEntries; i++){
			tracker->weights[i]=malloc(sizeof(weightsObject));
			if(tracker->weights[i]==NULL) {
				ret=false;
				tracker->exists[i]=false;
				tracker->valid[i]=false;
			}
			else{
				tracker->exists[i]=true;
				tracker->valid[i]=false;
				tracker->weights[i]->nameSz=0;
				tracker->weights[i]->numBiases=0;
				tracker->weights[i]->numKernels=0;
				tracker->weights[i]->id=i;
			}
		}

		if(ret==false){
			for(int i=0; i<tracker->maxEntries;i++){
				free(tracker->weights[i]);
			}
			return ret;
		}

		return ret; // true if all passed
	}
}

void make_handler(msg_t *msg){
	// CMD=M
	// LEN=dynamic
	bool ret=true;
	weightsObject *w;

	uint8_t tmp[9];
	tmp[8]='\0';

	int slot=-1;
	// find a free slot in the struct
	for(int i=tracker->maxEntries; i>=0; i--){
		if(tracker->exists[i] && !tracker->valid[i]) slot=i;
	}

	if(slot<0) {
		ret=false;
	}
	else{
		w=tracker->weights[slot];

		int idx=0;
		// numKernels
		memcpy(tmp, &(msg->buf[idx]), 8);
		idx+=8;
		w->numKernels=strtol(tmp, NULL, 16);
		w->kernelSz=malloc(sizeof(uint32_t)*w->numKernels);

		if(w->kernelSz == NULL) {
			while(1); // die here if we can't allocate memory for sizes of each kernel vector
		}

		// lenKernels[]
		for(int i=0; i<w->numKernels; i++){
			memcpy(tmp, &(msg->buf[idx]), 8);
			idx+=8;
			w->kernelSz[i]=strtol(tmp, NULL, 16);
		}

		// numBiases
		memcpy(tmp, &(msg->buf[idx]), 8);
		idx+=8;
		w->numBiases=strtol(tmp, NULL, 16);
		w->biasSz=malloc(sizeof(uint32_t)*w->numBiases);

		// lenBiases[]
		for(int i=0; i<w->numKernels; i++){
			memcpy(tmp, &(msg->buf[idx]), 8);
			idx+=8;
			w->biasSz[i]=strtol(tmp, NULL, 16);
		}

		// lenName
		memcpy(tmp, &(msg->buf[idx]), 8);
		idx+=8;
		w->nameSz=strtol(tmp, NULL, 16);
		w->name=malloc(sizeof(uint8_t)*(w->nameSz + 1));

		// name[]
		memcpy(w->name, &(msg->buf[idx]), w->nameSz);
		w->name[w->nameSz]='\0';
	}

	// if ret==true then we succeeded, else we failed
	tracker->valid[slot]=ret; // valid indicates that we have allocated memory for this particular weightsObject

	// prep the return message
	sprintf(msg->buf, "%d", slot); // slot has -1 on a fail, else a positive index value. that's enough info
	msg->len = strlen(msg->buf);
}

void status_handler(msg_t *msg){
	int slot;
	uint8_t tmp[9]="";

	// TODO: return programmatic error codes here so i can fetch the status of ANYTHING without human intervention required

	// fetch slot id
	slot = strtol(msg->buf, NULL, 10);
	if(slot < 0 || slot>tracker->maxEntries){
		// return error
		sprintf(msg->buf, "!Error invalid slot ID [%d]", slot);
	}
	else if(!tracker->exists[slot]) {
		sprintf(msg->buf, "!Error no weights object struct at slot ID [%d]", slot);
	}
	else if(!tracker->valid[slot]){
		sprintf(msg->buf, "!Error weights object at slot ID [%d] not yet allocated", slot);
	}
	else {
		// collect stats on this slot id and return them
		weightsObject *w = tracker->weights[slot];
		msg->buf[0]='\0'; // null-out the msg buf

		sprintf(tmp, "%08x", w->numKernels);
		strcat(msg->buf, tmp);

		for(int i=0; i<w->numKernels; i++){
			sprintf(tmp, "%08x", w->kernelSz[i]);
			strcat(msg->buf, tmp);
		}


		sprintf(tmp, "%08x", w->numBiases);
		strcat(msg->buf, tmp);

		for(int i=0; i<w->numBiases; i++){
			sprintf(tmp, "%08x", w->biasSz[i]);
			strcat(msg->buf, tmp);
		}

		sprintf(tmp, "%08x", w->nameSz);
		strcat(msg->buf, tmp);

		strcat(msg->buf, w->name);
	}

	msg->len = strlen(msg->buf);

}
