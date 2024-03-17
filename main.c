//#include "Timer.h"
#include "lcd.h"
#include "movement.h"
#include "open_interface.h"
#include <stdio.h>
#include <math.h>
#include "uart.h"
#include "servo.h"
#include "ping.h"
#include "adc.h"

void music(void) {
    unsigned char notes[] = {71, 74, 76, 76, 76, 78, 79, 79, 79, 81, 78, 78, 76, 74, 74, 76};
    unsigned char duration[] = {8, 8, 16, 16, 8, 8, 16, 16, 8, 8, 16, 16, 8, 8, 8, 8};
    //unsigned char notes[] = {83}; // D = 74.3, B = 71.5, E = 76, G = 79, F# = 78, 81 = A, B = 83, C = 84
    //unsigned char duration[] = {128};

    oi_loadSong(0, 16, notes, duration);

    //potential full song continued at phrase 2
        unsigned char notes1[] = {71, 74, 76, 76, 76, 78, 79, 79, 79, 81, 78, 78, 76, 74, 76};
        unsigned char duration1[] = {8, 8, 16, 16, 8, 8, 16, 16, 8, 8, 16, 16, 8, 8, 16};
        oi_loadSong(1, 16, notes1, duration1);
    //phrase 3
        unsigned char notes2[] = {71, 74, 76, 76, 76, 79, 81, 81, 81, 83, 84, 84, 83, 81, 83, 76};
        unsigned char duration2[] = {8, 8, 16, 16, 8, 8, 16, 16, 8, 8, 16, 16, 8, 8, 8, 16};
        oi_loadSong(2, 16, notes2, duration2);
    //phrase 4
        unsigned char notes3[] = {71.5, 78, 79, 79, 81, 83, 76, 76, 79, 78, 78, 79, 76, 78};
        unsigned char duration3[] = {8, 8, 16, 16, 16, 8, 24, 8, 8, 16, 16, 8, 8, 8};
        oi_loadSong(3, 14, notes3, duration3);
}

void play(void) {
    oi_play_song(0);
}

void play1(void) {
    oi_play_song(1);
}

void play2(void) {
    oi_play_song(2);
}

void play3(void) {
    oi_play_song(3);
}

int main(void)
{

    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);

    lcd_init();
    uart_init();
    ping_init();
    servo_init();
    adc_init();



    int i = 0;

    char init[30] = "";
    char data[180] = "";
    char newData[180] = "";
    char da[180] = "";
    int detected = 0;
    int l = 0;
    int done = 0;
    int j = 0;


    //used to detect start of project with putty and a m character being sent
    while (detected == 0)
    {
        //lcd_printf("ogh856256");
        char c = (char) uart_receive_nonblocking();                       //
        if (c == 'm')
        {
            //lcd_printf("123");
            detected = 1;
        }
    }

        //start of scan initialization
        scan:

        i = 0;

        detected = 0;
        l = 0;
        for (l = 0; l <= sizeof(init); l++)
        {
            uart_sendChar(init[l]);                      //
        }
        int q = 0;

        //different variables
        int r = 0;                    //amount of objects
        int count = 0;                //used to go through all 3 of the scan points and then averaged
        float avg = 0;                  //averaged ir values of a specifc scan
        int edge1[10] = { 0 };        //beginning degree of an object
        int edge2[10] = { 0 };        //ending degree of an object
        int mid[10] = { 0 };          //middle degree of an object
        int dist[10] = { 0 };         //middle degree distance to an object
        int obj = 0;                  //detects an object
        double theta[10] = { 0.0 };   //theta degree value of an objects midpoint to edge
        double opp[10] = { 0.0 };     //linear width of object
        int temp = 0;
        int last = 0;
        double adc[10] = { 0.0 };

        //e
        sprintf(init, "Degrees      PING       adc\n");
        for (j = 0; j < 60; j++)
        {
            uart_sendChar(init[j]);                                    //
        }

        servo_move(40);
        timer_waitMillis(1000);
        while (q <= 180)
        {

            count = 0;

            servo_move(q + 40);
            //timer_waitMillis(5);
            count++;

            avg = ping_getDistance();
            uint16_t val = adc_read();
            double dista = 137000000 * pow(val, -2.14);
            dista += 8;
            //double dista = 155000 * pow(val, -1.2);

            sprintf(data, "\r %d          %.2f        %.2lf\r\n", q, avg, dista);

            //values to detect if there is an object present
            if (dista < 53 && (6.5 >= abs(dista - avg)))
            {
                if (detected == 0)
                {
                    edge1[r] = q;
                }
                detected = 1;
                edge2[r] = q;
                obj = 1;
            }
            else
            {
                //else if there was an object last scan but not this scan
                if (obj == 1 && (dista > 56 || avg > 56) && (10.5 <= abs(dista - avg)))
                {
                    obj = 0;
                    detected = 0;

                    temp = (edge2[r] - edge1[r]) / 2;
                    if (temp != 0)
                    {
                        mid[r] = temp + edge1[r];
                        servo_move(mid[r] + 40);
                        timer_waitMillis(500);
                        // linear width    ang = (edge2-edge1) / 2       tan(ang) = opp/getScan.sound_dist      tan theta = opposte/adjacent
                        theta[r] = ((edge2[r] - edge1[r]) / 2.0)
                                * (3.14 / 180.0);
                        i = 0;
                        j = 0;

                        dist[r] = ping_getDistance();
                        opp[r] = tan(theta[r]) * dist[r] * 2.0;

                        adc[r] = 137000000 * pow(adc_read(), -2.14);
                        //double dista = 137000000 * pow(adc_read(), -2.14);

                        r += 1;
                        i = 0;
                        j = 0;
                        servo_move(q + 40);
                        timer_waitMillis(200);
                    }
                }
            }
            j = 0;
            for (j = 0; j < 60; j++)
            {
                uart_sendChar(data[j]);                                   //
            }
            q += 1;
        }
        if(obj == 1) {
            temp = (edge2[r] - edge1[r]) / 2;
            if (temp != 0 && temp > 4)
            {
                mid[r] = temp + edge1[r];
                servo_move(mid[r] + 40);
                timer_waitMillis(500);
                theta[r] = ((edge2[r] - edge1[r]) / 2.0) * (3.14 / 180.0);
                i = 0;
                j = 0;

                dist[r] = ping_getDistance();
                opp[r] = tan(theta[r]) * dist[r] * 2.0;

                adc[r] = 137000000 * pow(adc_read(), -2.14);
                //double dista = 137000000 * pow(adc_read(), -2.14);

                r += 1;
                i = 0;
                j = 0;
                servo_move(q + 40);
                timer_waitMillis(200);
            }
        }

        for (i = 0; i < r; i++)                                       //
        {
            servo_move(mid[i] + 40);
            timer_waitMillis(1000);
            sprintf(newData, "\n\rdeg %d\n\rdis %d\n\rlinear width %.2lf\n\radc %.2lf\n\r", mid[i], dist[i], opp[i], adc[i]);
            for (j = 0; j < 60; j++)
            {
                uart_sendChar(newData[j]);                             //
            }
        }


    //start of labproject code
    done = 0;
    int lastMove; //1 = forward, 2 = backward, 3 = left, 4 = right
    int retVal = 0;
    while (done == 0) {
        char c = (char) uart_receive();                       //
        if (c == 'm') //rescan
        {
            goto scan;
        } else if (c == 'w') {                   //move forward
            if(lastMove == 2) {
                retVal = move_forward(sensor_data, 60);
            } else {
                retVal = move_forward(sensor_data, 100);     //5 cm
            }
            lastMove = 1;
        } else if (c == 'a') {                   //turn left
            if(lastMove == 4) {
                turn_left(sensor_data, 2);
            } else {
                turn_left(sensor_data, 10);       //10 degrees
            }
            timer_waitMillis(500);
            lastMove = 3;
        } else if (c == 's') {                    //go backward
            if(lastMove == 1) {
                move_backward(sensor_data, 60);
            } else {
                move_backward(sensor_data, 100);    //5 cm
            }
            timer_waitMillis(500);
            lastMove = 2;
        } else if (c == 'd') {                  //turn right
            if(lastMove == 3) {
                turn_right(sensor_data, -2);
            } else {
                turn_right(sensor_data, -10);        //10 degrees
            }
            timer_waitMillis(500);
            lastMove = 4;
        } else if (c == 'o') {

            //implement spinning or music
            lcd_printf("Argghh! We Found the Treasure!");
            turn_right(sensor_data, -360);
            music();
                        play();
                        timer_waitMillis(3000);
                        //music();
                        play1();
                        timer_waitMillis(3000);
                        //music();
                        play2();
                        timer_waitMillis(3000);
                        //music();
                        play3();
            timer_waitMillis(3000);
            done = 1;
        }

                                                                   //11 -> frontleft, 22 -> frontright, 33 -> left, 44 -> right       for the cliff sensor return values
        sprintf(da, "\n\rreturnValue: %d\n\r", retVal);            //retVal = 1, 2 -> small object, retVal = 11, 22, 33, 44 -> cliff. retVal = 111, 222, 333, 444 -> tape
        for (j = 0; j < 60; j++)
        {
             uart_sendChar(da[j]);
        }
    }
    oi_free(sensor_data);

    return 0;
}
