//
// Created by PC on 12/8/2023.
//

#include "Company.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include "ArrivalEvent.h"
#include "LeaveEvent.h"
#include "Queue.h"
#include <map>
#include <algorithm>

using namespace std;

template
class Queue<Event *>;

void Company::read_file(const char *filename, Parameters &eventParameters) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
    file >> eventParameters.num_stations >> eventParameters.time_between_stations;
    file >> eventParameters.num_WBuses >> eventParameters.num_MBuses;
    file >> eventParameters.capacity_WBus >> eventParameters.capacity_MBus;
    file >> eventParameters.trips_before_checkup >> eventParameters.checkup_duration_WBus
         >> eventParameters.checkup_duration_MBus;
    file >> eventParameters.max_waiting_time >> eventParameters.get_on_off_time;

    cout << "Parameters:\n"
         << "  Number of Stations: " << eventParameters.num_stations << "\n"
         << "  Time Between Stations: " << eventParameters.time_between_stations << "\n"
         << "  Number of WBuses: " << eventParameters.num_WBuses << "\n"
         << "  Number of MBuses: " << eventParameters.num_MBuses << "\n"
         << "  Capacity of WBus: " << eventParameters.capacity_WBus << "\n"
         << "  Capacity of MBus: " << eventParameters.capacity_MBus << "\n"
         << "  Trips Before Checkup: " << eventParameters.trips_before_checkup << "\n"
         << "  Checkup Duration WBus: " << eventParameters.checkup_duration_WBus << "\n"
         << "  Checkup Duration MBus: " << eventParameters.checkup_duration_MBus << "\n"
         << "  Max Waiting Time: " << eventParameters.max_waiting_time << "\n"
         << "  Get On/Off Time: " << eventParameters.get_on_off_time << "\n";

    int num_events;
    file >> num_events;

    for (int i = 0; i < num_events; ++i) {
        char eventType;
        file >> eventType;
        if (eventType == 'A') {
            ArrivalEvent *ae = new ArrivalEvent();
            string type;
            string atime;
            int id, start, end;
            file >> type;
            ae->setPtype(type);
            //
            file >> atime;
            short hour = stoi(atime.substr(0, 2));
            short minute = stoi(atime.substr(3, 5));
            Time t(hour, minute);
            ae->setTime(t);
            file >> id;
            ae->setId(id);
            //
            file >> start;
            ae->setStart(start);
            //
            file >> end;
            ae->setAnEnd(end);
            string sptype;
            getline(file, sptype);
            if (!sptype.empty()) {
                ae->setSPtype(sptype);
            }
            cout << eventType << " " << type << " " << atime << " " << id << " " << start << " " << end << " " << sptype
                 << endl;
            eventQueue.enqueue(ae);
        } else if (eventType == 'L') {
            LeaveEvent *le = new LeaveEvent();
            string ltime;
            file >> ltime;
            short hour = stoi(ltime.substr(0, 2));
            short minute = stoi(ltime.substr(3, 5));
            le->setTime(Time(hour, minute));
            int id;
            file >> id;
            le->setId(id);
            short start;
            file >> start;
            le->setStart(start);
            cout << eventType << " " << ltime << " " << id << " " << start << endl;
            eventQueue.enqueue(le);
        }
    }
}


void Company::generateOutputFile(const string &filename) {
    ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        cerr << "Error opening output file: " << filename << endl;
        return;
    }

    map<Time, Queue<Passenger*>> sortedPassengers;
    while (!finishedPassengerList.isEmpty()) {
        Passenger* p = finishedPassengerList.dequeue();
        sortedPassengers[p->getFinishTime()].enqueue(p);
    }

    outputFile << "FT ID AT WT TT\n";
    long totalWaitTime = 0, totalTripTime = 0;
    int totalNP = 0, totalSP = 0, totalWP = 0, autoPromoted = 0;

    for (auto &pair : sortedPassengers) {
        while (!pair.second.isEmpty()) {
            Passenger *p = pair.second.dequeue();

            Time waitTime = p->calculateWaitTime();
            Time tripTime = p->calculateTripTime();
            totalWaitTime += waitTime.getTotalMinutes();
            totalTripTime += tripTime.getTotalMinutes();

            // Increment passenger type counters
            string type = p->getPassengerType();
            if (type == "NP") totalNP++;
            else if (type == "SP") totalSP++;
            else if (type == "WP") totalWP++;

            // Check for auto-promoted passengers
            if (p->isAutoPromoted()) autoPromoted++;

            outputFile << p->getFinishTime() << " " << p->getId() << " " << p->getArrivalTime() << " "
                       << waitTime << " " << tripTime << "\n";
        }
    }

    int totalPassengers = totalNP + totalSP + totalWP;
    double avgWaitTime = static_cast<double>(totalWaitTime) / totalPassengers;
    double avgTripTime = static_cast<double>(totalTripTime) / totalPassengers;
    double autoPromotedPercentage = static_cast<double>(autoPromoted) / totalNP * 100;

    // Bus statistics
    int totalBuses = mBusMaintenance.getSize() + wBusMaintenance.getSize()
            + mBusMovingForward.getSize() + mBusMovingBackward.getSize()
            + wBusMovingForward.getSize() + wBusMovingBackward.getSize();
    int totalMBuses = mBusMaintenance.getSize() + mBusMovingForward.getSize()
            + mBusMovingBackward.getSize();
    int totalWBuses = wBusMaintenance.getSize()
            + wBusMovingForward.getSize() + wBusMovingBackward.getSize();
    double totalBusyTime = 0, totalUtilization = 0;

    // Bus statistics
    outputFile << "passengers: " << totalPassengers << " [NP: " << totalNP
                << ", SP: " << totalSP << ", WP: " << totalWP << "]\n";
    outputFile << "passenger Avg Wait time= " << avgWaitTime << "\n";
    outputFile << "passenger Avg trip time = " << avgTripTime << "\n";
    outputFile << "Auto-promoted passengers: " << fixed << setprecision(2)
                << autoPromotedPercentage << "%\n";
    outputFile << "buses: " << totalBuses << " [WBus: "
                << totalWBuses << ", MBus: " << totalMBuses << "]\n";

    outputFile.close();
}

void Company::addBusToCheckup(Bus *bus, Parameters &eventParameters) {
    if (eventParameters.trips_before_checkup >= bus->getJourneys() &&
        (bus->getBusType() == "NP" || bus->getBusType() == "SP")) {
        mBusMaintenance.enqueue(bus);
    } else
        wBusMaintenance.enqueue(bus);
}

void Company::busFromMovingToWaiting(Bus *bus, Parameters &eventParameters) {
    //if()


}

void Company::busFromWaitingToMoving(Bus *bus, Parameters &eventParameters, Station currentStation) {

// Check if the Mbus is full and there are no waiting passengers of type SP/NP
    if ((currentStation.getWaitingNpForward().isEmpty() || currentStation.getWaitingSpForward().isEmpty()) &&
        bus->getBusType() == "MB" && bus->getBusCapacity() == eventParameters.capacity_MBus) {
        // Check if the bus is moving forward or backward and enqueue accordingly
        if (bus->isBusForward()) {
            mBusMovingForward.enqueue(bus);
        } else {
            mBusMovingBackward.enqueue(bus);
        }
    }

// Check if the Wbus is full and there are no waiting passengers of type WP
    else if ((currentStation.getWaitingNpForward().isEmpty() || currentStation.getWaitingSpForward().isEmpty()) &&
             bus->getBusType() == "WB" && bus->getBusCapacity() == eventParameters.capacity_MBus) {
        // Check if the bus is moving forward or backward and enqueue accordingly
        if (bus->isBusForward()) {
            wBusMovingForward.enqueue(bus);
        } else {
            wBusMovingBackward.enqueue(bus);
        }
    }
}

Station Company::getStation(int stationNumber) {
    return stations[stationNumber];
}

Time Company::getCurrentTime() const {
    return currentTime;
}

void Company::incrementTime(int increment) {
    currentTime = currentTime + Time(0, increment);
}

Company::Company() {
    Time t;
    currentTime = t;
}
