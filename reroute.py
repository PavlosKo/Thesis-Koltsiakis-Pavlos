import traci
import traci.constants as tc
# Σύνδεση με το αρχείο του SUMO
traci.start(["sumo-gui", "-c", "ClusterRerouting.sumocfg"])
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    # Όταν φτάσει το πρώτο όχημα στο κέντρο, σταματάει
    if traci.vehicle.getRoadID("car_0") == "central_junction":
        traci.vehicle.setStop("car_0", "central_junction", duration=300) 
        for car_id in ["car_1", "car_2", "car_3", "car_4", "car_5", "car_6", "car_7", "car_8", "car_9", "car_10"]:
            # Επαναπροσδιορισμός διαδρομών ανά cluster
            if car_id in ["car_1", "car_2", "car_3", "car_4", "car_5"]:
                traci.vehicle.setRoute(car_id, ["north_road_1", "east_road_1"])  # Cluster 1
            else:
                traci.vehicle.setRoute(car_id, ["south_road_1", "west_road_1"])  # Cluster 2 
        traci.simulationStep()
traci.close()
