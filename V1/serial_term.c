

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <ctype.h>

#define CMDLINE_DBG true
#define DEBUG false

// Global variables to store configuration parameters
#define MAX_GAME_SCORE 32
#define CFGSTR_SIZE 64
#define FILENAME_SIZE 1024
#define TTYLINE_SIZE 256
#define MESSAGE_MAX_LENGTH 5
#define DELAY 1000 // [ms]

// Flags
bool WRITE_FLAG =  false;
bool ERROR_FLAG = false;

//Device parameters
uint32_t g_serial_speed = 0;
uint32_t g_termio_speed = 0;
char g_serial_device[CFGSTR_SIZE] = "";
char g_outfile[FILENAME_SIZE] = "stdout"; 
char gecko_message[MESSAGE_MAX_LENGTH] = {0};
uint8_t dataArrayKeys[MAX_GAME_SCORE] = {0}; 
uint8_t dataArray[MAX_GAME_SCORE] = {0};
static uint8_t arrayWriter = 0;

// Game paramters
static uint8_t roundCounter = 1;
char * filename = "data.txt";
char * ST = "st", * ND = "nd", * RD = "rd", * TH = "th";

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
#if DEBUG
void printArray(){
	printf("Scores: ");
	for(uint8_t i = 0; i <MAX_GAME_SCORE ;++i)
		if (dataArray[i] > 0)
			printf("%d ", dataArray[i]);
	printf("\n");
}
#endif
int getHighScore(void){
	int max = 0;
	for(uint8_t i = 0; i < MAX_GAME_SCORE; i++){
		if(dataArray[i] > max)
			max = dataArray[i];
	}
	return max;
}
char * getPostfixForNumber(const uint8_t pNumber){
	switch(pNumber){
		case 1: return ST; break;
		case 2: return ND; break;
		case 3: return RD; break;
		default:return TH; break;
	}
}
void writeToFile(void){
	FILE * fptr = fopen(filename, "w");

	if (fptr == NULL){
		perror("Error when opening the file\n");
		exit(EXIT_FAILURE);
	}
	for(uint8_t j = 0; j < MAX_GAME_SCORE; j++){
		if(dataArray[j] > 0){
			fprintf(fptr,"%d,%d\n",dataArray[j], dataArrayKeys[j]);
		}
	}
	printf("Data has saved succesfully to data.txt!\n");
	fclose(fptr);
}

bool isCharacterValid(const char charParam){
	if (
		charParam == 'b' 	||	// Snake's direction (to the left)
		charParam == 'j' 	|| 	// Snake's direction (to the right)
		charParam == 'f' 	||	// Gecko data frame's start flag
		charParam == 'l' 	||	// Gecko's data frame end flag
		charParam == 'q' 	||	// Quit the game
		isdigit(charParam)		// Data from Gecko's data frame
	   )
	   return true;
	return false;
}
void addNewScore(const uint8_t newValue){
	if (arrayWriter >= MAX_GAME_SCORE){
		printf("Memory is full!\n");
	} else {
		dataArray[arrayWriter] = newValue;
		arrayWriter++;
	}
}
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
	printf("Help for Snake Game\n");
	printf("Command line parameters:\n");
	printf("    -h                           : Print help\n");
	printf("    -s d=port_file,s=baud_rate   : Set serial port device file and baud rate\n");
	printf("Press q in game to Quit the game\n");
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
    while((opt = getopt(argc, argv, "hs:c")) != -1)
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
			default:
				if (CMDLINE_DBG) printf("Unknown command line parameter is received\n");
				ERROR_FLAG = true;
				break;			
		}
	}


	// Controlling the snake via terminal
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
	
	// Game handler
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
        selrval = select(ser_fd+1, &rfds, NULL, NULL, &tv);
        /* Don’t rely on the value of tv now! */
        if (selrval < 0) {
            perror("Select failed");
        }
        else if (selrval == 0) {
            // Timeout...
        }
        else {
            if (FD_ISSET(STDIN_FILENO, &rfds)) {				
				c = getchar();
				while (getchar() != '\n');
				if (isCharacterValid(c)){
                	write(ser_fd,&c,1);
					usleep(DELAY/5);
				} else {
					printf("Invalid character!\n");
				}
#if DEBUG
               printf("Reading from STDIN: %x\n",c);
#endif
            }
            if (FD_ISSET(ser_fd, &rfds)) {
				read(ser_fd,&c,1);
				if (c == 'f'){
					// Gecko message frame: f_data_l
					// - f: start flag
					// - data: the current score (snake's size)
					// - l : end flag
					uint8_t iterator = 0;
					while (iterator != 4){
						read(ser_fd, &gecko_message[iterator], 1);							
						iterator++;
						// If the number bigger than 9, than 2 digits will come
						// that means, the frame's lenght is 5, not 4. We have to handle it
						// Also the iterator has reached the proper digit
						if(atoi(&gecko_message[0]) < 10 && (iterator >= 3))
							break;
					}
					printf(" %d[%s] round's score: %d\n",roundCounter, getPostfixForNumber(roundCounter), atoi(&gecko_message[0]));
					addNewScore(atoi(&gecko_message[0]));
					dataArrayKeys[roundCounter -1] = roundCounter;
					roundCounter++;

# if DEBUG
					printf("number: %d, flag: %c\n", atoi(&gecko_message[0]), gecko_message[1]);
					printf("next_num: %d\n", atoi(&gecko_message[2]));
#endif
				}  else {
                	write(STDOUT_FILENO,&c,1);
				}
#if DEBUG
               printf("Writing to STDOUT: %x\n",c);
#endif
            }
        }
        // Actual select call end
    }
	// For ignoring how the previous character had been appearing in the new line
    tcsetattr(STDIN_FILENO, TCSANOW, &term_orig);  // Restore original terminal settings
    close(ser_fd);

	if(ERROR_FLAG){
		exit(EXIT_FAILURE);
	}

	printf("High score: %d\n", getHighScore());
	char choice;
	writeToFile();
	printf("Generate png result Y/N?:\n");
	scanf("%c", &choice);
	if (choice == 'Y'){
		system("gnuplot plot_script.gp");
	}
#if DEBUG
	printArray();
#endif
	// Exiting
	exit(EXIT_SUCCESS);
}
