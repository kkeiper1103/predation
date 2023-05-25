//
// Created by kkeiper1103 on 12/17/22.
//

#ifndef PREDATION_PROFILE_H
#define PREDATION_PROFILE_H

struct HuntStatistics {
    float metersTravelled = 0;
    double timeHunted = 0;
    int shotsMade = 0;
    int shotsSucceeded = 0;
};

struct Profile {
    char name[256] = "";
    int credits = 100;

    HuntStatistics lastHunt {};
    HuntStatistics overall {};
};

#endif //PREDATION_PROFILE_H
