/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: gps-utils.c - This file contains 3 functions. The main function in this file is the thread function
-- which is called from dcgps.c - this function is in an infinite loop which updates the gps_data_t struct 
-- constantly. The other two methods represent option choices: either print out satellites in view, or open a 
-- browser to explore the current location of the user if a gps fix is available.
--
-- PROGRAM: dcgps
--
-- FUNCTIONS:
-- void * myThreadFun(void *mygpsref)
-- void openBrowserToCurrentLoc(struct gps_data_t *mygps)
-- void printGPSData(struct gps_data_t *mygps)
--
-- DATE: November 3, 2019
--
-- REVISIONS: n/a
--
-- DESIGNER: Justin Cervantes and Mikhaela Layon
--
-- PROGRAMMER: Justin Cervantes and Mikhaela Layon
--
-- NOTES:
-- The print and browser display functions are called from a user selection window found in dcgps.c
----------------------------------------------------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <gps.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "gpsprint.h"

bool gpsFixAvail = false;
float totalKMeters = 0;
float lastLat = 0.0;
float lastLon = 0.0;

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: myThreadFun
--
-- DATE: November 3, 2019
--
-- REVISIONS: n/a
--
-- DESIGNER: Justin Cervantes and Mikhaela Layon
--
-- PROGRAMMER: Justin Cervantes 
--
-- INTERFACE: void * myThreadFun(void *mygpsref)
--
-- RETURNS: void
--
-- NOTES:
-- Runs a loop which reads gps data if there is any available. The boolean variable gpsFixAvail is updated if a fix of
-- 4 usable satellites or more are found (MODE_3D). This also supports the getCurrentGPSWanderingDistance function 
-- by using the Haversine Formula to track total distance between last known lat/long coordinates. If stationary, this 
-- method will hint at inaccuracies which may prove useful if considering calculating distances travelled while using GPSD.
-- 
-- Haversine Formula: https://www.geeksforgeeks.org/haversine-formula-to-find-distance-between-two-points-on-a-sphere/
		    
----------------------------------------------------------------------------------------------------------------------*/
void * myThreadFun(void *mygpsref) { 
    struct gps_data_t *mygps = mygpsref; // This step is needed to use the struct since mygpsref is currently a void *
    for(;;){
	
      if (gps_waiting (mygps, 500)) {
	    gps_read(mygps);
	    if(mygps->online != 0) {
		if ((mygps->status == STATUS_FIX) && 
		    (mygps->fix.mode >= MODE_2D) &&
		    !isnan(mygps->fix.latitude) && 
		    !isnan(mygps->fix.longitude)) {
		    gpsFixAvail = true;
		    
		    //Track total meters wandered due to satellite inaccuracy
		    long double one_deg = (M_PI) / 180;
		    
		    if(lastLat == 0.0 && lastLon == 0.0) {
			lastLat = mygps->fix.latitude;
			lastLon = mygps->fix.longitude;
		    }
		    float currentLat = mygps->fix.latitude;
		    float currentLon = mygps->fix.longitude;
		    
		    float lat1 = lastLat * one_deg;
		    float long1 = lastLon * one_deg;
		    float lat2 = currentLat * one_deg;
		    float long2 = currentLon * one_deg;
		    
		    long double dlong = long2 - long1;
		    long double dlat = lat2 - lat1;
		    
		    long double ans = pow(sin(dlat/2),2) 
			+ cos(lat1) 
			* cos(lat2) 
			* pow(sin(dlong/2),2);
		    ans = 2 * asin(sqrt(ans));
		    
		    long double earthRadiusKM = 6371;
		    ans = ans * earthRadiusKM;
		    
		    totalKMeters += ans;
		} else {
		    gpsFixAvail = false;
		}
	    }
	}
    }
    return NULL; 
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: openBrowserToCurrentLoc
--
-- DATE: November 3, 2019
--
-- REVISIONS: n/a
--
-- DESIGNER: Justin Cervantes
--
-- PROGRAMMER: Justin Cervantes
--
-- INTERFACE: void openBrowserToCurrentLoc(struct gps_data_t *mygps)
--
-- RETURNS: void
--
-- NOTES:
-- Call this function to open the default web browser and use the Google Maps API to get the user's current location
-- as a map marker. This method only runs once, and will resume the option menu selection once the browser closes.
-- Defauly spewage from the terminal command xdg-open has been turned off by piping the output to /dev/null.
----------------------------------------------------------------------------------------------------------------------*/
void openBrowserToCurrentLoc(struct gps_data_t *mygps) {
    
    // Set max attempts to 10 to try and perform the browser implementation
    int timeoutCounter = 1;
    int maxTries = 10;
    while(!gpsFixAvail) {
	printf("Waiting for a GPS fix... retrying connection in 5s <Attempt %d of %d>\n", timeoutCounter, maxTries);
	timeoutCounter++;
	sleep(5);
	if(timeoutCounter == 11) {
	    printf("Aborting operation due to timeout!\n"); 
	    return;
	}
    }
    printf("Program window launching... close the browser to continue the program!\n");
    float currentLat = mygps->fix.latitude;
    char buf[10];
    gcvt(currentLat,9, buf);
    
    char str[1024];
    strcpy(str, "xdg-open 'https://www.google.com/maps/search/?api=1&query=");
    strcat(str, buf);
    strcat(str, ",");
    
    float currentLon = mygps->fix.longitude;
    gcvt(currentLon,9, buf);
    strcat(str, buf);
    strcat(str,"' >/dev/null 2>/dev/null"); // Handles xdg-open spewage
    
    system(str);
    
}   
    
    
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: printGPSData
--
-- DATE: November 3, 2019
--
-- REVISIONS: n/a
--
-- DESIGNER: Justin Cervantes
--
-- PROGRAMMER: Justin Cervantes
--
-- INTERFACE: void printGPSData(struct gps_data_t *mygps)
--
-- RETURNS: void
--
-- NOTES:
-- Call this to run a print function which gives the pointer to the boolean which tracks whether a fix is available. 
----------------------------------------------------------------------------------------------------------------------*/
void printGPSData(struct gps_data_t *mygps) {
    printResults(mygps, &gpsFixAvail);
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: getCurrentGPSWanderingDistance
--
-- DATE: November 3, 2019
--
-- REVISIONS: n/a
--
-- DESIGNER: Justin Cervantes
--
-- PROGRAMMER: Justin Cervantes
--
-- INTERFACE: void getCurrentGPSWanderingDistance(struct gps_data_t *mygps)
--
-- RETURNS: void
--
-- NOTES:
-- Call this to run a print function which gives the current distance wandered in meters. 
-- This method has not been thoroughly tested! Please see the MyThreadFun for more details.
----------------------------------------------------------------------------------------------------------------------*/
void getCurrentGPSWanderingDistance(struct gps_data_t *mygps) {
    printf("\nRunning Option 3: Must have been stationary for the duration of runtime.\n");
    printf("Reporting on any implict wandering due to GPS inaccuracies.\n");
    printf("Disclaimer: Feature only updates when fix established\n");            
    printf("Total distance wandered: %f meters\n\n", totalKMeters/1000);
        
}
