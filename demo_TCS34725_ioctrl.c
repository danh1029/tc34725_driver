#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h> // Include errno header

#define DEVICE_NAME "/dev/tcs34725"
#define TCS34725_IOCTL_MAGIC 't'
#define TCS34725_IOCTL_READ_C _IOR(TCS34725_IOCTL_MAGIC, 1, int)
#define TCS34725_IOCTL_READ_R _IOR(TCS34725_IOCTL_MAGIC, 2, int)
#define TCS34725_IOCTL_READ_G _IOR(TCS34725_IOCTL_MAGIC, 3, int)
#define TCS34725_IOCTL_READ_B _IOR(TCS34725_IOCTL_MAGIC, 4, int)
#define TCS34725_IOCTL_SET_GAIN _IOW(TCS34725_IOCTL_MAGIC, 5, int)
#define TCS34725_IOCTL_SET_ATIME _IOW(TCS34725_IOCTL_MAGIC, 6, int)

int main(){

    int fd;
    int color_data;
    // Gain is setting the sensor sensitivity:  0x00, 0x01, 0x02, 0x03 equal to 1x 4x 16x 60x
    int gain = 0x02; 
    // integration time = (256 - atime ) *2.4, short time get data quick, long time more accurate
    int atime = 0xD5; 

    // Open
    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return errno;
    }
    printf("Device opened successfully\n");
    ioctl(fd, TCS34725_IOCTL_SET_GAIN, &gain);
    ioctl(fd, TCS34725_IOCTL_SET_ATIME, &atime);
    

    // Read Clear
    if (ioctl(fd, TCS34725_IOCTL_READ_C, &color_data) < 0) {
        perror("Failed to read Clear Color Data");    
        close(fd);
        return errno;
    } 
    else {
        printf("Read Clear Color Data: %d ", color_data);
    }

    // Read Red
    if (ioctl(fd, TCS34725_IOCTL_READ_R, &color_data) < 0) {
        perror("Failed to read Red Color Data");  
        close(fd);
        return errno;
    } 
    else {
        printf("Read Red Color Data: %d ", color_data);
    }

    // Read Green
    if (ioctl(fd, TCS34725_IOCTL_READ_G, &color_data) < 0) {
        perror("Failed to read Green Color Data");
        close(fd);
        return errno;
    } 
    else {
        printf("Read Green Color Data: %d ", color_data);
    }

    // Read Blue
    if (ioctl(fd, TCS34725_IOCTL_READ_B, &color_data) < 0) {
        perror("Failed to read Blue Color Data");
        close(fd);
        return errno;
    } 
    else {
        printf("Read Blue Color Data: %d ", color_data);
    }

    // Close the device
    close(fd);
    return 0;
}