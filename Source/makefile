# Resource 1 https://lists.gnu.org/archive/html/gpsd-dev/2017-11/msg00001.html
# Resource 2 https://stackoverflow.com/questions/30773098/trouble-connecting-to-gpsd
# Resource 3 https://gpsd.gitlab.io/gpsd/gpsd.html
# Resource 4 <Standard Pull Data> https://stackoverflow.com/questions/32000328/libgps-c-code-example

CC=gcc -Wall 
CLIB=-lgps -lpthread -lm -lc
# Resources on what this library is: https://www.systutorials.com/docs/linux/man/3-libgps/

dcgps: dcgps.o gps-utils.o gpsprint.o
	$(CC) -o dcgps dcgps.o gps-utils.o gpsprint.o $(CLIB)

clean:
	rm -f *.o core.* dcgps
dcgps.o:
	$(CC) -c dcgps.c 
gps-utils.o: 
	$(CC) -c gps-utils.c
gpsprint.o:
	$(CC) -c gpsprint.c
