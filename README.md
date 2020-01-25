# Location-Finder
This program is designed for use on a Raspberry Pi 3 and requires a GPS dongle attached to the Pi. The program uses the open source gps daemon (gpsd) in order to perform 3 functions: 1) find the user's current GPS location, 2) show the users location in the Google Maps API (in-browser), and 3) estimate how much "wandering" or GPS inaccuracy has been achieved while stationary.

To run the program, run the command "make" from the source, then execute the dcgps main file by calling ./dcgps from the source code file.
