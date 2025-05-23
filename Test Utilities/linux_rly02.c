
RLY02 test application for linux.

Compiled using gcc
Tested on Ubuntu 16.4 LTS.

This code communicates with the RLY02 by first opening
a port to comunicate with the device using fopen().
Data can then be written to the device using fwrite()
and fflush() or read using fread(). Once finished it
then closes the open port using fclose().

By James Henderson, 2016



#include stdio.h
#include stdlib.h
#include string.h
#include unistd.h
#include termios.h
#include sysstat.h
#include fcntl.h
#include systypes.h

void displayData(void);								 Displays module id and software version
void displayHelp(void);								 Displays help data

char sbuf[2];									 char array that stores data to be written
int fd;										 Pointer to file opened by fopen()

int main(int argc, char argv[])
{

	printf(n);
	printf(RLY02 linux test applicationn);
	printf(By James Henderson, 2016n);	
	printf(nn);

	if(argc  2)								 Check how many arguments passed, if less than 2 display help
	{
		displayHelp();
	}
	else if(argc  3)							 If more than 3 then display help
	{
		displayHelp();
	}
	else
	{
	int i; 
		for(i = 1; i  argc; i++)					 Check all arguments passed
		{
			if(strcmp(argv[i], -h) == 0)				 Check to see if help text is requested 
			{
				displayHelp();
			}
			else
			{
				struct termios defaults;							 to store innitial default port settings
				struct termios config;								 These will be our new settings
				const char device = devttyACM0;
				fd = open(device, O_RDWR  O_NOCTTY);
				if(fd == -1) {
  					printf( failed to open portn );
				} else {
					if (tcgetattr(fd, &defaults)  0) perror(tcgetattr);  		 Grab snapshot settings  for port
					cfmakeraw(&config);							 make options for raw data
					config.c_cc[VMIN]  = 2;
					config.c_cc[VTIME] = 50; 							
					if (tcsetattr(fd, TCSANOW, &config)  0) perror(tcsetattr config);   	 Set options for port
					
					if(strcmp(argv[i], 00) == 0)
					{
						sbuf[0] = 0x6F;							 Byte to turn relay 1 off
						if (write(fd, sbuf, 1)  0) perror(write);	 Write data to RLY02
						if (tcdrain(fd)  0) perror(tcdrain);
						sbuf[0] = 0x70;							 Byte to turn relay 2 off
						if (write(fd, sbuf, 1)  0) perror(write);	 Write data to RLY02
						if (tcdrain(fd)  0) perror(tcdrain);
					}
					else if(strcmp(argv[i], 01) == 0)	
					{
						sbuf[0] = 0x65;			 Byte to turn relay 1 on
						if (write(fd, sbuf, 1)  0) perror(write);	 Write data to RLY02
						if (tcdrain(fd)  0) perror(tcdrain);
						sbuf[0] = 0x70;			 Byte to turn relay 2 off
						if (write(fd, sbuf, 1)  0) perror(write);	 Write data to RLY02
						if (tcdrain(fd)  0) perror(tcdrain);
					}
					else if(strcmp(argv[i], 10) == 0)
					{
						sbuf[0] = 0x6F;			 Byte to turn relay 1 off
						if (write(fd, sbuf, 1)  0) perror(write);	 Write data to RLY02
						if (tcdrain(fd)  0) perror(tcdrain);
						sbuf[0] = 0x66;			 Byte to turn relay 2 on
						if (write(fd, sbuf, 1)  0) perror(write);	 Write data to RLY02
						if (tcdrain(fd)  0) perror(tcdrain);
					}
					else if(strcmp(argv[i], 11) == 0)
					{
						sbuf[0] = 0x65;			 Byte to turn relay 1 on
						if (write(fd, sbuf, 1)  0) perror(write);	 Write data to RLY02
						if (tcdrain(fd)  0) perror(tcdrain);
						sbuf[0] = 0x66;			 Byte to turn relay 2 on
						if (write(fd, sbuf, 1)  0) perror(write);	 Write data to RLY02
						if (tcdrain(fd)  0) perror(tcdrain);
					}
					else
					{
						displayHelp();			 If argument passed is no recognised display help text
					}
					
					displayData();					

					if (tcsetattr(fd, TCSANOW, &defaults)  0) perror(tcsetattr default);	 Restore port default before closing	
				}				
		
			close(fd);						 Close the file we opened
			}
		}
	}	

	return(0);

}


void displayData(void)
{
	sbuf[0] = 0x5A;								 Send byte to return software version and module id
	if (write(fd, sbuf, 1)  0) perror(displayData write);		 Write data to RLY02
	if (tcdrain(fd)  0) perror(displayData tcdrain);
	if (read(fd, sbuf, 2)  0) perror(displayData read);			 Read data back from RLY02, module ID and software version
				
	printf(Module id %u n, sbuf[0]);
	printf(Module software version %u n, sbuf[1]);

	sbuf[0] = 0x5B;								 Send byte to return relay states
	if (write(fd, sbuf, 1)  0) perror(displayData write);		 Write data to RLY02
	if (tcdrain(fd)  0) perror(displayData tcdrain);
	if (read(fd, sbuf, 1)  0) perror(displayData read);			 Read data back from RLY02, module ID and software version

	printf(Relay states %01X nn, sbuf[0]);
}


void displayHelp(void)
{
		printf(Usage RLY02 -[option] RELAYSTATEn);			 Display help text to screen
		printf(OPTIONSn);
		printf(	-h  this screenn);
		printf(RELAYSTATEn);
		printf(	00 - All relays offn);
		printf(	01 - Relay 1 on relay 2 offn);
		printf(	10 - Relay 1 off relay 2 onn);
		printf(	11 - Relay 1 on relay 2 onnn);
}
