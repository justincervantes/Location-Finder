/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: gpsprint.c - This file will consist of one complete function that will handle all of the printing 
-- out of GPS report data. One of its arguments is a pointer to a gps_data_t structure, which will be passed to
-- it from the printGPSData method and updated by the read loop described in gps-utils.c.
--
-- PROGRAM: dcgps
--
-- FUNCTIONS:
-- void printResults(struct gps_data_t *mygps, bool *gpsFixAvail)
--
--
-- DATE: November 3, 2019
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Justin Cervantes and Mikhaela Layon
--
-- PROGRAMMER: Justin Cervantes and Mikhaela Layon
--
-- NOTES:
-- The program will continue to print until a certain time, then ask the user to continue or to return to the main menu.
----------------------------------------------------------------------------------------------------------------------*/

#include <gps.h>
#include <math.h>
#include <unistd.h>

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: printResults
--
-- DATE: November 3, 2019
--
-- REVISIONS: n/a
--
-- DESIGNER: Justin Cervantes and Mikhaela Layon
--
-- PROGRAMMER: Justin Cervantes and Mikhaela Layon
--
-- INTERFACE: void printResults(struct gps_data_t *mygps, bool *gpsFixAvail)
--
-- RETURNS: void
--
-- NOTES:
-- Call this function to run a loop which prints all the satellites, their PRN, Elevation, Azimuth, SNR, and whether
-- they are used. If there is a fix detected (gps-utils.c thread), also indicate the current coordinates, otherwise 
-- show n/a.
----------------------------------------------------------------------------------------------------------------------*/
void printResults(struct gps_data_t *mygps, bool *gpsFixAvail) {
	int loopCounter = 1;
	while(loopCounter <= 5) {
		if(mygps->satellites_visible != 0) {
			printf("%d Satellites currently visible!\n", mygps->satellites_visible);
			int i;
			for(i = 0; i < mygps->satellites_visible; i++) {
				printf("PRN: %3d Elevation: %02d, Azimuth: %03d, SNR: %02d, Used: %c\n", mygps->skyview[i].PRN, mygps->skyview[i].elevation,
				mygps->skyview[i].azimuth, (int)mygps->skyview[i].ss, mygps->skyview[i].used ? 'Y' : 'N');
			}
				char scr[128];
			(void)unix_to_iso8601(mygps->fix.time, scr, sizeof(scr));
			printf("%s: ", scr);	
			(*gpsFixAvail) ? printf("Latitude: %f %c; Longitude: %f W\n\n", mygps->fix.latitude, (mygps->fix.latitude < 0) ? 'S':'N', mygps->fix.longitude) : printf("n/a\n\n");
			
			sleep(5);
			
		} else {
			printf("No satellites visible... re-trying in 5 seconds!\n");
			sleep(5);
		}
		loopCounter++;
	}
	printf("Re-select option 1 to continue printing...\n\n");
}
