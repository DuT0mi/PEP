

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>

#define CMDLINE_DBG true

// Global variables to store configuration parameters
#define CFGSTR_SIZE 64
#define FILENAME_SIZE 1024
#define TTYLINE_SIZE 256
uint32_t g_serial_speed = 0;
uint32_t g_termio_speed = 0;
char g_serial_device[CFGSTR_SIZE] = "";
char g_infile[FILENAME_SIZE] = "stdin"; 
char g_outfile[FILENAME_SIZE] = "stdout"; 

// Accessing termios.h Bnum type speed definition from the command line

struct BNUM_speed {
	uint32_t speed;
	uint32_t bnum;
};

// Constants from /usr/include/asm-generic/termbits.h 
// Constants are octal in termbits.h
struct BNUM_speed g_speed[] = {
	{ 50 , B50 },
    { 75 , B75 },
	{ 110 , B110 },
	{ 134 , B134 },
	{ 150 , B150 },
	{ 200 , B200 },
	{ 300 , B300 },
	{ 600 , B600 },
	{ 1200 , B1200 },
	{ 1800 , B1800 },
	{ 2400 , B2400 },
	{ 4800 , B4800 },
	{ 9600 , B9600 },
	{ 19200 , B19200 },
	{ 38400 , B38400 },
	{ 57600 , B57600 },
	{ 115200, B115200 },
	{ 230400 , B230400 },
	{ 460800 , B460800 },
	{ 500000 , B500000 },
	{ 576000 , B576000 },
	{ 921600 , B921600 },
	{ 1000000 , B1000000 },
	{ 1152000 , B1152000 },
	{ 1500000 , B1500000 },
	{ 2000000 , B2000000 },
	{ 2500000 , B2500000 },
	{ 3000000 , B3000000 },
	{ 3500000 , B3500000 },
	{ 4000000 , B4000000 },
	{ 0 , 0 } // Null termination, keep as the last element...
};

bool set_g_speed(int speed) {
	int i = 0;
	for (i = 0; g_speed[i].speed != speed; i++)
	{
		if (g_speed[i].speed == 0) {
			return false;
		}
	}
	g_termio_speed = g_speed[i].bnum;
	return true;
}

// Print help for -h command line option
void print_help(void){
	printf("Help for PEP example\n");
	printf("Command line parameters:\n");
	printf("    -h                           : Print help\n");
	printf("    -s d=port_file,s=baud_rate   : Set serial port device file and baud rate\n");
    printf("    -i file                      : Read everything from input file\n");
	printf("    -o file                      : Write everything to output file\n");
}


int main(int argc, char *argv[]){
    int opt;
    int i;
    
	// Stuff required by getopt-subopt handling for the serial port
	enum {
		DEVICE_OPT = 0,
		SPEED_OPT
	};
	
	char *const token[] = {
		[DEVICE_OPT] = "d",
		[SPEED_OPT] = "s",
		NULL
	};

    char *subopts;
    char *value;
	int errfnd = 0;
	
    if (argc <= 1 ) {
        printf("No command line parameters, does not know what to do...\n");
    }

	// Printing command line arguments if CMDLINE_DBG is set
    if (CMDLINE_DBG)
    {
        printf("The arguments supplied are (ARG_NUM: ARG_VALUE):\n");
        for(i = 0; i < argc; i++)
        {
            printf("%d: %s\n", i, argv[i]);
        }
    }

	// Handling commmad line arguments, and storing them in globals
    while((opt = getopt(argc, argv, "hs:o:i:")) != -1)
    {
		switch(opt)
		{
			case 'h':
				if (CMDLINE_DBG) printf("-h cmd line par received, printing help\n");
				print_help();
				exit(EXIT_SUCCESS);
				break;
			case 's':
				if (CMDLINE_DBG) printf("-s cmd line par received, setting serial port\n");
				subopts = optarg;
				while ((*subopts != '\0') && !errfnd) {
					switch (getsubopt(&subopts, token, &value)){
						case DEVICE_OPT:
							if (strlen(value) < CFGSTR_SIZE) {
								strcpy(g_serial_device,value);
								if (CMDLINE_DBG) printf("Serial port: %s\n",g_serial_device);
							}
							else {
								printf("Serial device file name is too long (max size is %d), exiting...\n",CFGSTR_SIZE);
								exit(EXIT_FAILURE);
							}
							break;
						case SPEED_OPT:
							g_serial_speed = atoi(value);
							if (g_serial_speed == 0) {
								printf("Serial device speed is invalid, exiting...\n");
								exit(EXIT_FAILURE);
							}
							if (set_g_speed(g_serial_speed)) {
								printf("Serial port speed: %d (%#x)\n",g_serial_speed,g_termio_speed);
								printf("B115200 %x\n",B115200);
							}
							else {
								printf("Specified serial speed is not supported by termios.h\n");
								exit(EXIT_FAILURE);
							}
							break;							
					}
				}
				break;				
			case 'i':
				if (CMDLINE_DBG) printf("-i cmd line par received, setting input file\n");
				if (strlen(optarg) < FILENAME_SIZE) {
					strcpy(g_infile,optarg);
					if (CMDLINE_DBG) printf("Input file: %s\n",g_infile);
				}
				else {
					printf("Input file name is too long (max size is %d), exiting...\n",FILENAME_SIZE);
					exit(EXIT_FAILURE);
				}				
				break;						
			case 'o':
				if (CMDLINE_DBG) printf("-f cmd line par received, setting output file\n");
				if (strlen(optarg) < FILENAME_SIZE) {
					strcpy(g_outfile,optarg);
					if (CMDLINE_DBG) printf("Output file: %s\n",g_outfile);
				}
				else {
					printf("Output file name is too long (max size is %d), exiting...\n",FILENAME_SIZE);
					exit(EXIT_FAILURE);
				}				
				break;						
			default:
				if (CMDLINE_DBG) printf("Unknown command line parameter is received\n");
				break;			
		}
	}

    struct termios gecko_ser;
    int ser_fd;
    int out_fd;
    
    memset(&gecko_ser,0,sizeof(gecko_ser));
    
    gecko_ser.c_iflag=0;
    gecko_ser.c_oflag=0;
    gecko_ser.c_cflag=CS8|CREAD|CLOCAL;
    gecko_ser.c_lflag=0;
    gecko_ser.c_cc[VMIN]=1;
    gecko_ser.c_cc[VTIME]=5;
    
    ser_fd = open(g_serial_device, O_RDWR);
    
    if (ser_fd < 0) {
        perror("Serial device open failed");
        exit(EXIT_FAILURE);
    }
    
    cfsetospeed(&gecko_ser,g_termio_speed);
    cfsetispeed(&gecko_ser,g_termio_speed);
    
    tcsetattr(ser_fd,TCSANOW,&gecko_ser); // This is where we set the actual serial port
    
    char c = 0;

    /*For ignoring how the previous character had been appearing in the new line, changing the terminal settings */
    struct termios term, term_orig;
    tcgetattr(STDIN_FILENO, &term);
    term_orig = term;
    term.c_lflag &= ~(ECHO | ICANON);  // Disable echo and canonical mode
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

	/* */
    while (c != 'q') { // q as Quit
    
    
        // Required for select
        int selrval;
        fd_set rfds;
        fd_set wfds;
        struct timeval tv;
    
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(ser_fd, &rfds);
        FD_ZERO(&wfds);
        FD_SET(STDOUT_FILENO, &wfds);
        FD_SET(ser_fd, &wfds);
        /* Wait up to five seconds. */
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        // Required for select end
    
        // Actual select call
        selrval = select(ser_fd+1, &rfds, NULL, NULL, &tv); // ser_fd + 1-re várunk, írásra nem kell mert azonnal megtörténik
        /* Don’t rely on the value of tv now! */
        if (selrval < 0) {
            perror("Select failed");
        }
        else if (selrval == 0) {
            // Timeout...
        }
        else {
            if (FD_ISSET(STDIN_FILENO, &rfds)) {
				/*Khazy thing*/
                // read(STDIN_FILENO,&c,1);

				/* Terminal bug thing*/
			    //   scanf(" %c",&c); // Should be ignore the /n
				
				/* Working */
				// For ignoring how the previous character had been appearing in the new line
				c = getchar();
				while (getchar() != '\n');
				
                write(ser_fd,&c,1);
				/* Pretty good for debugging */
              // printf("Reading from STDIN: %x\n",c);
            }
            if (FD_ISSET(ser_fd, &rfds)) {
                read(ser_fd,&c,1);
                write(STDOUT_FILENO,&c,1);
				/* Pretty good for debugging */
               // printf("Writing to STDOUT: %x\n",c);
            }
        }
        // Actual select call end
    }
	// For ignoring how the previous character had been appearing in the new line
    tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);  // Restore original terminal settings
    
    close(ser_fd);
	
	// Exiting
	exit(EXIT_SUCCESS);
}
