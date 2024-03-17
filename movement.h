/*
 * movement.h
 *
 *  Created on: Feb 5, 2023
 *      Author: joshhyde
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include "Timer.h"
#include "lcd.h"
#include "movement.h"
#include "open_interface.h"


int move_forward(oi_t *sensor_data, double distance_mm);
void turn_right(oi_t *sensor, double degrees);
void turn_left(oi_t *sensor, double degrees);
void move_backward(oi_t *sensor, double distance_mm);


#endif /* MOVEMENT_H_ */
