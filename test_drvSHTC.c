/**
 * @file   test_drvSHTC.c
 * @author Alexandre Besia, Filippos Gleglakos
 * @date   29.05.2022
 * @version 0.1
 * @brief  A Linux user space program that communicates with the drvSHTC driver.
 * It reads the temperature from the driver.
 * For this example to work the device must be called /dev/drvSHTC.
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define MAX_VALUES 100
#define BUFFER_LENGTH 256

void shift_values(float values[]);

int main(int argc, char *argv[])
{
    char receive[BUFFER_LENGTH];
    float values[MAX_VALUES];
    int value_index = 0;
    int sleep_ms = (argc > 1) ? atoi(argv[1]) : 500;
    
    float temperature = 0.f;
    
    int ret, fd;
    printf("Starting device test SenseHat...\n");
    fd = open("/dev/drvSHTC", O_RDWR);
    if (fd < 0)
    {
        perror("Failed to open the device...");
        return errno;
    }
   
    while (1)
    {
        ret = read(fd, receive, BUFFER_LENGTH);
        temperature = 175.f * ret / 65536.f - 45.f;
        printf("Temperature = %f°C\n", temperature);
        
        values[value_index] = temperature;
        if (++value_index == MAX_VALUES)
        {
            shift_values(values);
            value_index = 0;
        }
        usleep(sleep_ms);
    }
    return 0;
}

void shift_values(float values[])
{
    for (int i = 1; i < MAX_VALUES; ++i)
    {
        values[i - 1] = values[i];
    }   
}
