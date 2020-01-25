/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: dcgps.c - This file contains the main function. The main function in this file calls the setup 
-- functions gps_open and gps_stream. The rest of main locks the main thread to a menu interface where the user can
-- either view the satellites and curent lat/long, open their location in a browser, show a GPS error detection 
-- service or exit the program. The main concludes by closing the gps stream and handling any dynamic memory.
--
-- PROGRAM: dcgps
--
-- FUNCTIONS:
-- int main()
--
-- DATE: November 3, 2019
--
-- REVISIONS: n/a
--
-- DESIGNER: Justin Cervantes
--
-- PROGRAMMER: Justin Cervantes
--
-- NOTES: n/a
----------------------------------------------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <gps.h>
#include <errno.h>
#include "gps-utils.h"

struct gps_data_t * mygps;

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: main
--
-- DATE: November 3, 2019
--
-- REVISIONS: n/a
--
-- DESIGNER: Justin Cervantes
--
-- PROGRAMMER: Justin Cervantes
--
-- INTERFACE: int main()
--
-- RETURNS: int
--
-- NOTES:
-- Drives the program. The main function in this file calls the setup functions gps_open and gps_stream. The rest of 
-- main locks the main thread to a menu interface where the user can either view the satellites and curent lat/long, 
-- open their location in a browser, or exit the program. The main concludes by closing the gps stream and handling any 
-- dynamic memory.
----------------------------------------------------------------------------------------------------------------------*/
int main()
{
    mygps = malloc(sizeof(struct gps_data_t));
    mygps->online = 0;
	
    // 1. Open a stream to gpsd
	if (gps_open("localhost", "2947", mygps) != 0) {
        (void)fprintf(stderr,
                     "cgps: no gpsd running or network error: %d, %s\n",
                      errno, gps_errstr(errno));
        exit(EXIT_FAILURE);
    }
    
    gps_stream(mygps, WATCH_ENABLE, NULL);
    
    
    // 2. Create a read thread
    /* This thread will continuously populate the mygps struct (read).
     * There should be no added functionality aside from populating struct
     * and setting the bool flag for allowing option 2 to start
     * */
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, myThreadFun, (void*)mygps); 
    
    
    // 3. Create a menu options loop
    char menuSelection = '0';
    bool continueMenu = true;
    bool firstTimeOpen = true;
    while(continueMenu) {
        if(!firstTimeOpen) { if(getchar() == EOF) {continue;} } // Deals with buffer flushing
        firstTimeOpen = false;
        printf("Please select one of the following menu options:\n");
        printf("[1] Read GPS data\n");
        printf("[2] View my current location in Google Maps\n");
        printf("[3] Get current GPS wandering distance (only use when stationary)\n");
        printf("[4] Exit\n");
        menuSelection = getchar();
        switch(menuSelection) {
            case '1':
                printf("Option 1 selected\n");
                printGPSData(mygps); 
                break;
            case '2':
                printf("Option 2 selected\n");
                openBrowserToCurrentLoc(mygps); 
                break;
            case '3':
                printf("Option 3 selected\n");
                getCurrentGPSWanderingDistance(mygps);
                break;
            case '4':
                continueMenu = false;
                printf("Exiting the program...\n");
                break;
            default:
                printf("Invalid response, please select a valid menu item.\n");
                break;
        }
    }
    
    // 4. Close the thread, close the stream, free the memory
    gps_stream(mygps, WATCH_DISABLE, NULL);
    gps_close(mygps);
    pthread_cancel(thread_id);
    free(mygps);
    
    return 0;
}
