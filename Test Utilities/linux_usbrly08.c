/****************************************************************
*Linux example code for the USBRLY08				*
*								*
*Compiled using gcc.						*
*Tested on ubuntu 16.04 LTS					*
*								*
*Reads a binary input from the command line and sets the	*
*relays on/off to match this input.				*
*								*
*By James Henderson, 2016.					*
****************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#define P_ERROR	-1						// to check for errors when dealing with the port

int getStates(void);
void displayData(int fd);
void setRelays(int fd, int state);
void helpScreen(void);
int openPort(void);
void closePort(int fd);
void writeData(int fd, int nbytes);
void readData(int fd, int nbytes);

struct termios options;

unsigned char sbuf[10];						// Stores data to be read and written
char bin[10];							// Will be used to calculate the binary value entered

int main(int argc, char *argv[])
{
int fd;								// file descriptor of open port
int states;

	printf("**************************************\n");
	printf("USBRLY08 linux example code\n");
	printf("By James Henderson, 2016.          \n");
	printf("**************************************\n\n");
	
	if(argc != 2)						// If incorect ammount of arguments input
	{
		helpScreen();						
	}
	else	
	{	
		sprintf(bin,"%s",argv[1]);			// Store the input binary in bin[] as a char string
		states = getStates();				// Calculate the states of the relays

		if(states == P_ERROR)				// If get states has returned an error return 0 and exit
		{
			return 0;
		}
		else
		{
			fd = openPort();			// Open port
			displayData(fd);			// Get and display the module ID and software version
			setRelays(fd, states);			// Set the states of the relays
			closePort(fd);				// Close the port
		}
	}
	return 0;
}


int getStates(void)
{
int sum, x, i, b;

	sum = 0;						// will hold the value of the binary number input
	
	b = 7;
	/*
	Create the binary value input from a character array by looking
	at each element and shifting a 1 or 0 into place in a char.
	*/
	for(i = 0; i < 8; i++)
	{
		x = 0;						// Clear x before we put anthing into it

		if(bin[i] == '1')
		{
			x = 0x01 << b;				// Shift left b times
		}
		else if(bin[i] == '0')
		{
			x = 0x00 << b;
		}
		else
		{
			printf("Invalid input.\n\n");		// If neither a 1 or 0 is found display and error and return 1
			helpScreen();
			return -1;
		}

		b--;						// Will shift value by 1 less place next loop round

		sum += x;
	}
	return(sum);
}


void displayData(int fd)
{
	sbuf[0] = 0x5A;					// Sending this byte will make the USBRLY08 return its module number and the version number
	
	writeData(fd, 1);
	
	readData(fd, 2);				// Read the data back
	
	printf("Module ID  : %d\n", sbuf[0]);		// Display data
	printf("Software v : %d\n\n", sbuf[1]);
	
}

void setRelays(int fd, int state)
{	
		sbuf[0] = 0x5C;				// Set all relay states
		sbuf[1] = state;			// To value in state
		writeData(fd,2);
}


void helpScreen(void)					// Display help text
{
	printf("Usage : \n");
	printf("    usbrly08 RELAYSTATES\n");
	printf("RELAYSTATES:\n");	
	printf("    RELAYSTATES is an 8 digit binary number\n");
	printf("that represents the states that the relays \n");
	printf("are to be set to. 1 is on and 0 is off. \n\n");	
}


int openPort(void)
{
int fd;										// File descriptor for the port

	fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY );				// Open port for read and write not making it a controlling terminal
	if (fd == P_ERROR)
	{
		perror("openPort: Unable to open /dev/ttyUSB0 - ");		// If open() returns an error
	} 
	else
	{
		tcgetattr(fd, &options);
		cfmakeraw(&options);
		options.c_cc[VMIN]  = 2;
		options.c_cc[VTIME] = 50; 
		tcsetattr(fd, TCSANOW, &options);				// Set the new options for the port
	}
	return (fd);
}


void closePort(int fd)
{	
	if(close(fd) == P_ERROR)						// Close the port if it returns an error then display an error report
	{	
		perror("closePort: Unable to close /dev/ttyUSB0 - ");
	}
}


void writeData(int fd, int nbytes)
{
int bytes;
	
	bytes = write(fd, sbuf, nbytes);					// Write nbytes of data from wbuf
	if(bytes == P_ERROR)							// If write returns an error (-1)
	{
		perror("writeData: Error while trying to write data - ");	
	}
	else if(bytes != nbytes)
	{
		printf("only %u bytes written out of %u requested\n", bytes, nbytes);	
	}
}


void readData(int fd, int nbytes)
{
int bytes;

	bytes = read(fd, sbuf, nbytes);						// Read nbytes of data into rbuf
	if(bytes == P_ERROR)							// If read returns and error (-1)
	{
		perror("readData: Error while trying to read data - ");
	}
	else if(bytes != nbytes)
	{
		printf("Only %u bytes read out of %u requested\n", bytes, nbytes);
	}
}


