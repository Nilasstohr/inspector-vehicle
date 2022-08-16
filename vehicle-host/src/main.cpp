//============================================================================
// Name        : testing2.cpp
// Author      : NWS
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#include <iostream>
using namespace std;

int main() {
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
}
