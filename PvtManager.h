//
//  PvtManager.h
//
//  Created by Andrew on 12/29/13.
//  Copyright (c) 2013 AA Engineering LLC. All rights reserved.
//

#ifndef PvtManager_h
#define PvtManager_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>



typedef struct {
    double latitude;
    double longitude;
    double altitude;
    double timetag;
    //struct timeval timetag;
    bool valid;
} gnss_location_t;


void receivedGnssLocation(gnss_location_t location);

#ifdef __cplusplus
}
#endif

#endif
