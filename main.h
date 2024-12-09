#ifndef CLUSTER_REROUTING_H
#define CLUSTER_REROUTING_H
#include <omnetpp.h>
#include <csimplemodule.h>
#include <cmessage.h>
using namespace omnetpp;

class ClusterRerouting : public cSimpleModule {
  private:
    int numCars;
    int k;
    int clusterIterationLimit;
    double broadcastTime;
    bool receiveMessage;

    void rerouteAfterAccident();
    void rerouteCar(int carIndex, const char* route);

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};
#endif
