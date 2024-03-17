/*
 * movement.c
 *
 *  Created on: Feb 5, 2023
 *      Author: joshhyde
 */
#include "lcd.h"
#include "movement.h"
#include "open_interface.h"
#include "Timer.h"

int move_forward(oi_t *sensor_data, double distance_mm) {         //if returns 1, small object hit
    double sum = 0;
    int temp = 0;
    oi_setWheels(150, 150);
    while(sum < distance_mm) {
        oi_update(sensor_data);
        sum += sensor_data -> distance;
        if(sensor_data -> bumpLeft) {                    //if left bumper hits something
           temp = 1;
           move_backward(sensor_data, 100);
           break;
        } else if(sensor_data -> bumpRight) {              //if right bumper hits something
            temp = 2;
            move_backward(sensor_data, 100);
            break;

            //cliff sensor if statements
        } else if(sensor_data -> cliffFrontLeftSignal < 100) {
            temp = 11;
            move_backward(sensor_data, 100);
            break;
        } else if(sensor_data -> cliffFrontRightSignal < 100) {
            temp = 22;
            move_backward(sensor_data, 100);
            break;
        } else if(sensor_data -> cliffLeftSignal < 100) {
            temp = 33;
            move_backward(sensor_data, 100);
            break;
        } else if(sensor_data -> cliffRightSignal < 100) {
            temp = 44;
            move_backward(sensor_data, 100);
            break;

            //tape sensor if statements
        } else if (sensor_data -> cliffFrontLeftSignal > 2600) {
            temp = 111;
            move_backward(sensor_data, 100);
            break;
        } else if (sensor_data -> cliffFrontRightSignal > 2600) {
            temp = 222;
            move_backward(sensor_data, 100);
            break;
        } else if (sensor_data -> cliffLeftSignal > 2600) {
            temp = 333;
            move_backward(sensor_data, 100);
            break;
        } else if (sensor_data -> cliffRightSignal > 2600) {
            temp = 444;
            move_backward(sensor_data, 100);
            break;
        }
    }
    oi_setWheels(0,0);
    return temp;
}

void turn_right(oi_t *sensor, double degrees) {
    double sum = 0;
    oi_setWheels(-100, 100);
    while(sum > degrees) {
        oi_update(sensor);
        sum += sensor -> angle;
    }
    oi_setWheels(0,0);
}

void turn_left(oi_t *sensor, double degrees) {
    double sum = 0;
    oi_setWheels(100, -100);
    while(sum < degrees) {
        oi_update(sensor);
        sum += sensor -> angle;
    }
    oi_setWheels(0,0);
}

void move_backward(oi_t *sensor, double distance_mm) {
    double sum = 0;
    oi_setWheels(-150, -150);
    while(sum > distance_mm*-1) {
        oi_update(sensor);
        sum += sensor -> distance;
    }
    oi_setWheels(0, 0);
}
