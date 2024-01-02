//
// Created by LENOVO on 10/12/2023.
//
#pragma once
#ifndef DATA_STRUCTURE_PROJECT_PASSENGER_H
#define DATA_STRUCTURE_PROJECT_PASSENGER_H

#include "Time.h"
#include <string>
#include "Station.h"

using namespace std;
class Passenger {
private:
    Time Arrival_Time;  // AT
    Time Move_Time;     // MT
    Time Finish_Time;   // FT
    short Get_On;
    short Get_Off;
    int Start_Station;
    int End_Station;
    string Passenger_Type;
    string Special_Type;
    int Id;
public:
    Passenger();
    const Time &getArrivalTime() const;
    void setArrivalTime(const Time &arrivalTime);
    short getGetOn() const;
    void setGetOn(short getOn);
    short getGetOff() const;
    void setGetOff(short getOff);
    int getStartStation() const;
    void setStartStation(int startStation);
    int getEndStation() const;
    void setEndStation(int endStation);

    const Time &getMoveTime() const;

    const Time &getFinishTime() const;

    void setFinishTime(const Time &finishTime);

    const string &getPassengerType() const;
    void setPassengerType(const string &passengerType);
    const string &getSpecialType() const;
    void setSpecialType(const string &specialType);
    int getId() const;
    void setId(int id);
    void setMoveTime(const Time &moveTime);

    Time calculateWaitTime() const;
    void calculateFinishTime(Time &busArrivalTime);
    Time calculateTripTime() const;
};


#endif //DATA_STRUCTURE_PROJECT_PASSENGER_H
