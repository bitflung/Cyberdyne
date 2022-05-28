#ifndef PC_GUI_H
#define PC_GUI_H

#include "c_adup.h"

void adup_pc_handler(msg_t *msg);

/*
 * RUN
 * 	req: R00
 * 	res: R08[3:%human image][3:%human voice][1:shoot?][1:cheat?]
 * 				0-100			0-100		1=shoot		1=cheated
 */
void run_handler(msg_t *msg);

/*
 * CAMERA
 * 	req: C00
 * 	res: C__ [16KB bytes stream payload]
 */
void cam_handler(msg_t *msg);

#endif
