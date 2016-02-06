//
//  PvtManager.c
//  NmeaTester
//
//  Created by Andrew on 12/29/13.
//  Copyright (c) 2013 AA Engineering LLC. All rights reserved.
//

#include "PvtManager.h"

#include <stdio.h>
//#include "streamer.h"
//#import "ServerConnection.h"

//#include "ServerConnection.h"



void instrumentGnssLocation(gnss_location_t *location);



void receivedGnssLocation(gnss_location_t location)
{
    instrumentGnssLocation(&location);

    //start_stream();
}



void instrumentGnssLocation(gnss_location_t *location)
{
    //Serial.printf("location - lat:%f lon:%f alt:%f time:%ld.%.6i\n",location->latitude,location->longitude,location->altitude,location->timetag.tv_sec,location->timetag.tv_usec);

}
