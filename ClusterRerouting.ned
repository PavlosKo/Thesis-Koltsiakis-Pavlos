import inet.node.inet.Router;
import inet.node.inet.StandardHost;
import inet.mobility.single.MassMobility;
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadio;
import inet.visualizer.intergrated.IntegratedVisualizer;
import org.car2x.veins.modules.mobility.traci.TraCIScenarioManager;
import org.car2x.veins.visualizer.roads.RoadsCanvasVisualizer;



network ClusterRerouting
{
types:
        module CarType extends StandardHost
        {
            parameters:
                int numCars;
                int k;
                int clusterIterationLimit;
                double broadcastTime;
                bool rerouteViaTraCI;
                bool logClusterAssignments;
                @display("i=misc/car2");
                app.typename = "ClusterRerouting";
                Mobility.typename = "MassMobility";
                wlan.typename = "Ieee80211ScalarRadio";
            gates:
                input upperLayerIn;
                output upperLayerOut;
        }

        module RSU extends Router
        {
            parameters:
                bool accidentWarningActive;
                @display("i=device/antennatower");
                app.typename = "ClusterRerouting";
                Mobility.typename = "StaticMobility";
                wlan.typename = "Ieee80211ScalarRadioMedium";
            gates:
                output radioOut[i];
        }

    submodules:
        radioMediumRSU: Ieee80211ScalarRadioMedium {
            @display("p=200,200");
        }
        radioMediumCar: Ieee80211ScalarRadio {
            @display("p=200,91");
        }

        car[numCars]: CarType {
            @display("p=50,50");
        }
        rsu: RSU {
            @display("p=100,100");
        }

        manager: TraCIScenarioManager {
            @display("p=37,215");
        }
        visualizer: IntegratedVisualizer {
            @display("p=102,230");
        }
        roadsCanvasVisualizer: RoadsCanvasVisualizer {
            @display("p=115,153");
        }
    connections allowunconnected:
        for i=0..10 {
            car[i].upperLayerOut --> rsu.radioIn[i];
            car[i].upperLayerIn <-- rsu.radioOut[i];
        }

        for i=0..10, for j=0..i-1 {
            car[i].upperLayerOut --> car[j].upperLayerIn;
            car[j].upperLayerOut --> car[i].upperLayerIn;
        }
}

