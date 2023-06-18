#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>

void handler (int sig)
{
    printf ("nexiting...(%d)n", sig);
    exit (0);
}

void perror_exit (char *error)
{
    perror (error);
    handler (9);
}

int main (int argc, char *argv[])
{
    struct input_event ev[64];
    int fd, rd, value, size = sizeof (struct input_event);
    char name[256] = "Unknown";
    char *device = "./keyboard_key_capture /dev/input/event1";

    if ((getuid ()) != 0)
        printf ("You are not root! This may not work...n");

    //Open Device
    if ((fd = open (device, O_RDONLY)) == -1)
        printf ("%s is not a vaild device.n", device);

    //Print Device Name
    ioctl (fd, EVIOCGNAME (sizeof (name)), name);
    printf ("Reading From : %s (%s)n", device, name);

    while (1){
        if ((rd = read (fd, ev, size * 64)) < size)
            perror_exit ("read()");

        value = ev[0].value;

        if (value != ' ' && ev[1].value == 1 && ev[1].type == 1){ // Only read the key press event
            printf ("Code[%d]n", (ev[1].code));
        }
    }

    return 0;
}

/*
#include <cstdio>
// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#include <iostream>
using namespace std;

int main(int argc, char ** argv)
{
    int serial_port = open("/dev/ttyACM0", O_RDWR);
    char read_buf [256];

    // Read bytes. The behaviour of read() (e.g. does it block?,
    // how long does it block for?) depends on the configuration
    // settings above, specifically VMIN and VTIME

    int n = read(serial_port, &read_buf, sizeof(read_buf));
    if(n>0)
        cout << read_buf<< endl; // prints !!!Hello World!!!
    else
        cout << "no data available" << endl;
    close(serial_port);
    return 0;

    char c;
    printf("\n>>");
    while(1) {
        scanf("%c", &c);
        if (c != '\n') {
            printf("%c", c);
            printf("\n>>");
        }
    }
    return 0;
}
*/