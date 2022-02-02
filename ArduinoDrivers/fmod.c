#include <fmod.hpp>
#include <iostream>
#include <fmod_errors.h>
#include <termios.h>            // kbhit()
#include <unistd.h>             // kbhit() and usleep
#include <fcntl.h>              // kbhit()
#include <stdio.h>
#include <dirent.h>				// for directory operations
#include <string.h>             //for strcpy() to copy string
#include <stdlib.h>             //for system() to execute string on terminal and return string atoi() convert string to integer
#include <sys/stat.h>           //Gets status of the file.Gives information about the link
// ***********************
// TODO 0 Include all files used for strcpy(), system(), atoi() and lstat()
// ***********************


// Macros
#define GOTO_XY(line, char) printf("\x1B[%d;%dH", line, char)
#define CLREOL printf("\x1B[K")

// USB
#define DRIVER_PATH "/sys/bus/usb/drivers/teensy/"				// TODO 1 - Change driver path if not correct//NEED TO REDO IT

// Frequency Spectrum
#define NUM_BARs 52
#define SCALING_FACTOR 130.0f
#define SAMPLE_SIZE 256

// LEDs
#define NUM_LEDs 3
#define SAMPLES_PER_LED 14
#define LED_THRESHOLD 3
#define LED_MAX 180.0f

// String/Array Sizes
#define MP3_FILENAME_SIZE 100
#define NUM_OF_MP3_FILES 100
#define CURRENT_DIRECTORYNAME_SIZE 200
#define DATA_BUFFER_SIZE 20
#define PATH_SIZE 100

// Global Variables
char mp3_files[NUM_OF_MP3_FILES][MP3_FILENAME_SIZE];			// Stores names of mp3 files in Current Working Directory
int file_index=0;

// find_device - Looks for a symbolic link of USB device at location 'DRIVER_PATH' and concatinates device address to *dir, if found.
void find_device(char *dir)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(dir)) == NULL) {//Return pointer to the directory or an error number pointer
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);
    while((entry = readdir(dp)) != NULL) {//function returns a pointer to a dirent structure or searches till it find path end os tream return null
	        lstat(entry->d_name,&statbuf);
	// Found a directory, but ignore . and .. 
	if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
		continue;
	if(S_ISLNK(statbuf.st_mode))
		if(entry->d_name[0] != 'm')
		{
			strcat(dir, entry->d_name);
			strcat(dir, "/");
			break;
		}
    }
    chdir("..");
    closedir(dp);
}

int kbhit(void)
{
        struct termios oldt, newt;
        int ch;
        int oldf;

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

        ch = getchar();

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);

        if(ch != EOF)
        {
                ungetc(ch, stdin);
                return 1;
        }
        return 0;
}

void FMODErrorCheck(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        std::cout << "FMOD error! (" << result << ") " << FMOD_ErrorString(result) << std::endl;
    }
}

// getSongToPlay - Prints all MP3 files in current working directory and asks user a number for song
//                 to play and return number in 'file_index'
void getSongToPlay()
{
	char cwd[CURRENT_DIRECTORYNAME_SIZE];					// Holds current working directory
	const char *ext;
	DIR *dir;
	struct dirent *ent;

	// Get current working directory	
	getcwd(cwd, sizeof(cwd));

	// Open all files in current working directory
	printf("\n\n\nMP3 files in current working directory:\n");
	if ((dir = opendir (cwd)) != NULL) {
	  /* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			ext = strrchr(ent->d_name, '.');
			if (ext)		// extension exist
			{
				if (strcmp(ext+1, "mp3") == 0)
				{
					strcpy(mp3_files[file_index], ent->d_name);
					printf ("%d. %s\n", file_index+1, ent->d_name);
					file_index++;
				}
			}
		}
		closedir (dir);
	} else {
	  printf ("Can't open file.\n");
	  return;
	}
	
	// Take input from user
	printf("Which one to play? ");
	scanf("%d", &file_index);
	cwd[0] = getchar();
	//fflush(stdin);
	file_index--;
	printf("Playing %s.\n", mp3_files[file_index]);
}

int main (void)
{

	char device_path[PATH_SIZE] = "";						// Holds path for Teensy device
	char path_buffer[PATH_SIZE] = "";						// Holds paths for button and LEDs
	char path_pot[PATH_SIZE] = "";							// Holds path for potentiometer
	int fd_red, fd_green, fd_yellow, fd_button, fd_pot;		// To open different files
	float *specLeft, *specRight, *spec;						// Buffers for spectrum
	float myLEDs[NUM_LEDs];									// Average a band of frequencies
	float hzRange = (44100 / 2) / SAMPLE_SIZE;  			// Sample rate of mp3 in place of 44100
	int set_volume;
	int volume_count = 0;
	int val;
	unsigned char button_val, pot_val;
	bool isPaused;
	int maxxx=0;
	//path for red green and yellow led
	char red_path[PATH_SIZE] = "";
	char green_path[PATH_SIZE] = "";
	char yellow_path[PATH_SIZE] = "";

	FMOD_RESULT result;
	FMOD::System     *system_fmod; 							// Handle to FMOD engine
	FMOD::Sound      *sound1; 								// Sound that will be loaded and played

	// Get song to play
	getSongToPlay();										// Returns song index in file_index
	usleep (2000000);

	// Init FMOD
	FMOD::System_Create(&system_fmod);						// Create an instance of the FMOD system
	result = system_fmod->init(32, FMOD_INIT_NORMAL, 0);	// Initialise with 32 channels
	FMODErrorCheck(result);

	// Load sounds
	result = system_fmod->createStream(mp3_files[file_index], FMOD_SOFTWARE, 0, &sound1);
	FMODErrorCheck(result);

	result = sound1->setMode(FMOD_LOOP_NORMAL);
	FMODErrorCheck(result); 
   
	FMOD::Channel *channel;
	result = system_fmod->playSound(FMOD_CHANNEL_FREE, sound1, false, &channel);
	FMODErrorCheck(result);

	// Search for Teensy device
	strcat(device_path, DRIVER_PATH);
	find_device(device_path);	// Finds the first device at DRIVER_PATH and concatinate device address to 'device_path'

/********************************
					TODO 2 Begin
	- Concatinate red LED's file name to 'device_path' and open red LED's file in fd_red.
	  Note: Please don't modify 'device_path', since it would be used later in code.
	- Open similarly files of green and yellow LEDs and in 'fd_green' and 'fd_yellow' respectively.
	- Store path of button's file in 'path_buffer'.
	- Store path of potentiometer's file in 'path_pot'.
*********************************/
FILE *fptr1;
FILE *fptr2;
FILE *fptr3;
char fname[32];
iota(fd_red,fname,10);
strcpy(red_path,device_path);
strcat(red_path,fname);
//for yellow
fptr1=fopen(red_path,"w+");
if(fptr1 == NULL){
	printf("Error");
	exit(1);
}
//for green
iota(fd_green,fname,10);
strcpy(green_path,device_path);
strcat(green_path,fname);
fptr2=fopen(green_path,"w+");
if(fptr2 == NULL){
	printf("Error");
	exit(1);
}

//for yellow
iota(fd_yellow,fname,10);
strcpy(yellow_path,device_path);
strcat(yellow_path,fname);
fptr3=fopen(yellow_path,"w+");
if(fptr3 == NULL){
	printf("Error");
	exit()1;
}
//button path
iota(fd_button,fname,10);
strcpy(path_buffer,device_path);
strcat(path_buffer,fname);
//potentiometer path
iota(fd_pot,fname,10);
strcpy(path_pot,device_path);
strcat(path_pot,fname);

/********************************
					TODO 2 End
*********************************/

	// Print frequency ranges
	system("clear");
	for (int i = 0; i < NUM_BARs; i++)
	{
		printf("%4d-%4dHz :\n", (int)(i*hzRange), (int)((i+1)*hzRange));
	}

	while (!kbhit())
	{
		FMODErrorCheck(system_fmod->update()); //update FMOD, need to call this once per frame to keep the sound engine running

		if (volume_count % 50 == 0)			// Low frequency of volume update
		{
/********************************
					TODO 3 Begin
	- Open file from 'path_pot' in 'fd_pot'.
	- Read 1 byte from it in pot_val.
	- Close it.
*********************************/
fopen(path_pot,'rb');
pot_val=int fgetc (FILE *path_pot);
fclose(path_pot);




/********************************
					TODO 3 End
*********************************/
			if (set_volume != pot_val)
			{
				set_volume = pot_val;
				channel->setVolume((float) (pot_val / 255.0f));
			}
		}
		volume_count ++;
		// getSpectrum() performs the frequency analysis, see explanation below
		specLeft = new float[SAMPLE_SIZE];
		specRight = new float[SAMPLE_SIZE];
		 
		// Get spectrum for left and right stereo channels
		channel->getSpectrum(specLeft, SAMPLE_SIZE, 0, FMOD_DSP_FFT_WINDOW_RECT);
		channel->getSpectrum(specRight, SAMPLE_SIZE, 1, FMOD_DSP_FFT_WINDOW_RECT);

		spec = new float[NUM_BARs];
		
		// Take average of both stereo channels and scale to screen
		for (int i = 0; i < NUM_BARs; i++)
		{
			spec[i] = (specLeft[i] + specRight[i]) / 2;
			spec[i] *= SCALING_FACTOR;
			if (maxxx < spec[i])
				maxxx = (int)spec[i];
		}

		// Initialize myLEDs to 0
		for (int i = 0; i < NUM_LEDs; i++)
			myLEDs[i] = 0.0f;

		// Average multiple <SAMPLES_PER_LED> samples to get <NUM_LEDs> number of LEDs' values
		for (int i = 0; i < NUM_LEDs; i++)
		{
			for (int j = 0; j < SAMPLES_PER_LED; j++)
			{
				if (spec[(i*SAMPLES_PER_LED)+j] > LED_THRESHOLD)
					myLEDs[i] += spec[(i*SAMPLES_PER_LED)+j];
			}
			//printf("%f\t", myLEDs[i]);
		}
		
		// Draw bars on screen
		for (int i = 0; i < NUM_BARs; i++)
		{
			GOTO_XY(i, 14);
			CLREOL;
			for (int j = 0; j < (int)spec[i]; j++)
			{
				printf("*");
			}
		}
		fflush(stdout);
		
		// Send led values as intensity to Teensy device
		val = (int)(myLEDs[0] * LED_MAX / (float)maxxx);		// Normalize to one byte value
		if (val > LED_MAX) val = LED_MAX;
/********************************
					TODO 4 Begin
	- Write value of 'val' to red LED.
*********************************/
//fopen(path_pot,'rb');
//pot_val=int fgetc (FILE *path_pot);


fprintf(red_path,"%d",val);


/********************************
					TODO 4 End
*********************************/

		val = (int)(myLEDs[1] * LED_MAX / (float)maxxx);		// Normalize to one byte value
		if (val > LED_MAX) val = LED_MAX;


/********************************
					TODO 5 Begin
	- Write value of 'val' to yellow LED.
*********************************/

fprintf(yellow_path,"%d",val);

/********************************
					TODO 5 End
*********************************/


		val = (int)(myLEDs[2] * LED_MAX / (float)maxxx);		// Normalize to one byte value
		if (val > LED_MAX) val = LED_MAX;


/********************************
					TODO 6 Begin
	- Write value of 'val' to green LED.
*********************************/

fprintf(green_path,"%d",val);


/********************************
					TODO 6 End
*********************************/


		// Keep audio paused if button is pressed		
		do
		{
/********************************
					TODO 7 Begin
	- Open file 'path_buffer' in 'fd_button'.
	- Read 1 byte from it in button_val.
	- Close it.
*********************************/


fopen(path_buffer,'rb');
button_val=int fgetc (FILE *path_buffer);
fclose(path_buffer);





/********************************
					TODO 7 End
*********************************/
			channel->getPaused(&isPaused);
			if (button_val == 0 && !isPaused)
			{
				channel->setPaused(!isPaused);
			} else if (button_val != 0 && isPaused)
				channel->setPaused(!isPaused);
		}while (button_val == 0);

		//usleep (100000);
		delete [] spec;
		delete [] specLeft;
		delete [] specRight;

	}

/********************************
					TODO 8 Begin
	- Close files 'fd_red', 'fd_yellow' and 'fd_green'.
*********************************/


fclose(green_path);
fclose(yellow_path);
fclose(red_path);



/********************************
					TODO 8 End
*********************************/
	sound1->release();
	result = system_fmod->close();
	FMODErrorCheck(result);
	result = system_fmod->release();
	FMODErrorCheck(result);

}
