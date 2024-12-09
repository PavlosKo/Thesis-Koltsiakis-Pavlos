#include "ClusterRerouting.h"
#include "BroadcastMessage.h"
#include <omnetpp.h>
#include <vector>
#include <cmath>
#include <limits>
#include <iostream>
#include <algorithm>
#include <random>
#include <csimplemodule.h>
#include <cmessage.h>
using namespace omnetpp;

Define_Module(CarType);

void ClusterRerouting::initialize() {
    // Ανάγνωση των παραμέτρων
    numCars = getParentModule()->par("numCars");           // Αριθμός οχημάτων
    k = par("k").intValue();                               // Αριθμός clusters για K-means
    clusterIterationLimit = par("clusterIterationLimit");  // Όριο επαναλήψεων του K-means

    // Χρονοπρογραμματισμός του πρώτου μηνύματος broadcast αν το όχημα είναι το πρώτο
    if (getIndex() == 0) {
        broadcastTime = par("broadcastTime").doubleValue(); // Χρόνος για μετάδοση από το πρώτο όχημα
        scheduleAt(simTime() + broadcastTime, new cMessage("broadcastMessage"));
    } else {
        receiveMessage = false;
    }
}

void ClusterRerouting::handleMessage(cMessage *msg) {
    // Έλεγχος αν το μήνυμα είναι το broadcastMessage και αν αυτό είναι το πρώτο όχημα
    if (strcmp(msg->getName(), "broadcastMessage") == 0 && getIndex() == 0) {
        BroadcastMessage *bcMessage = new BroadcastMessage("accidentWarning");
        bcMessage->setAccidentLocation("centralJunction");
        send(bcMessage, "out");
        delete msg;
    } else if (BroadcastMessage *bcMessage = dynamic_cast<BroadcastMessage *>(msg)) {
        EV << "Το όχημα " << getIndex() << " message delivered, changing route...\n";
        receiveMessage = true;
        rerouteAfterAccident();
        delete bcMessage;
    } else {
        delete msg;
    }
}

void ClusterRerouting::rerouteAfterAccident() {
    int numCars = getParentModule()->par("numCars");  // Ανάκτηση αριθμού οχημάτων

    std::vector<int> carIndices(numCars - 1);
    std::iota(carIndices.begin(), carIndices.end(), 1);

    std::vector<double> clusterCenters(k);

    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_real_distribution<double> dist(0, numCars - 1);
    for (int i = 0; i < k; ++i) {
        clusterCenters[i] = dist(rng);
    }

    std::vector<int> clusters(numCars - 1);

    for (int iter = 0; iter < clusterIterationLimit; ++iter) {
        for (int i = 1; i < numCars; ++i) {
            double minDist = std::numeric_limits<double>::max();
            int clusterIdx = 0;
            for (int j = 0; j < k; ++j) {
                double dist = std::abs(i - clusterCenters[j]);
                if (dist < minDist) {
                    minDist = dist;
                    clusterIdx = j;
                }
            }
            clusters[i - 1] = clusterIdx;
        }

        for (int j = 0; j < k; ++j) {
            double sum = 0;
            int count = 0;
            for (int i = 0; i < numCars - 1; ++i) {
                if (clusters[i] == j) {
                    sum += i + 1;
                    count++;
                }
            }
            if (count > 0) {
                clusterCenters[j] = sum / count;
            }
        }
    }

    for (int i = 1; i < numCars; ++i) {
        if (clusters[i - 1] == 0) {
            cout << "Το όχημα " << i << " επιλέγει τη Διαδρομή Α (Cluster 1)\n";
            rerouteCar(i, "RouteA");
        } else {
            cout << "Το όχημα " << i << " επιλέγει τη Διαδρομή Β (Cluster 2)\n";
            rerouteCar(i, "RouteB");
        }
    }
}
