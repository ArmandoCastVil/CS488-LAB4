/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "platform/mbed_thread.h"
#include "rtos.h"
#include "XNucleoIKS01A2.h"






// Create a DigitalOut objects for the LED
DigitalOut led(PA_5);

// Create a Serial objects to communicate via USB
Serial pc(USBTX, USBRX);


// Initialize variables
float Temp_Celsius;
float Humidity;
float Pressure;
Thread temp_thread;
Thread humi_thread;
Thread press_thread;
Thread blinky_thread;
Mutex stdio_mutex;
/* Create sensor shield object */
static XNucleoIKS01A2 *mems_expansion_board = XNucleoIKS01A2::instance(D14, D15, D4, D5);
static HTS221Sensor *hum_temp = mems_expansion_board->ht_sensor;
static LPS22HBSensor *press_temp = mems_expansion_board->pt_sensor;

//threat to display temperature
void temperature_thread() {
    while (true) {
        ThisThread::sleep_for(1000);
        stdio_mutex.lock();
        hum_temp->get_temperature(&Temp_Celsius);
        printf("temperature in Celsius   = %.2f C\r\n", Temp_Celsius);
        stdio_mutex.unlock();
    }
}
//threat to display humidity
void humidity_thread() {
    while (true) {
        ThisThread::sleep_for(1000);
        stdio_mutex.lock();
        hum_temp->get_humidity(&Humidity);
        printf("humidity = %.2f %% \r\n", Humidity);
        stdio_mutex.unlock();
    }
}
// thread to display pressure
void pressure_thread() {
    while (true) {
        ThisThread::sleep_for(1000);
        stdio_mutex.lock();
        press_temp->get_pressure(&Pressure);
        printf("pressure = %.2f mbar\r\n", Pressure); 
        stdio_mutex.unlock(); 
    }
}
//thread to blink an led
void led_thread() {
    while (true) {
        stdio_mutex.lock();
        led = !led;
        stdio_mutex.unlock();     
    }
}

int main()
{
    while(1)
    {
    printf("\r\n--- Starting new run ---\r\n");
    //start threads
    temp_thread.start(temperature_thread);
    humi_thread.start(humidity_thread);
    press_thread.start(pressure_thread);
    blinky_thread.start(led_thread);
    //sleep to reduce power consumption
    ThisThread::sleep_for(1000);
    //these work to clear the screen
    pc.printf("%c",char(27)); // ESC
    pc.printf("[2J"); // clear screen
    pc.printf("%c",char(27)); // ESC
    pc.printf("[H"); // cursor to home
    
    }
}
