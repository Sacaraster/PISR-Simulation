from Classes.VehicleClass import Vehicle

def main():
    
    print "TEST ROUTING CLASS"
    vehA = Vehicle(0, 100, 1, 0, 22, 85, 1)
    print vehA.ID
    print vehA.t_terminate
    routing_data = ['MD2WRP', 4, [1,1,1]]
    vehA.add_routing(routing_data)
    # print vehA.routing.beta
    vehA.routing.get_next_task()

    print ''

    vehB = Vehicle(0, 200, 1, 0, 22, 85, 1)
    print vehB.ID
    routing_data = ['Manual', [1,2,3]]
    vehB.add_routing(routing_data)
    print vehB.routing.current_stop
    vehB.routing.get_next_task()

    print ''

    print "TEST PATHING CLASS"
    pathing_data = ['Euclidean', 'Task Vector']
    vehA.add_pathing(pathing_data)
    newPath = vehA.pathing.get_path()
    print newPath

    pathing_data = ['Tripath', 'Task Vector']
    vehB.add_pathing(pathing_data)
    newPath = vehB.pathing.get_path()
    print newPath

    print ''

    print "TEST COMMUNICATION CLASS"
    comm_data = ['Destination']
    vehA.add_comm(comm_data)
    vehA.comm.talk()

    comm_data = ['None']
    vehB.add_comm(comm_data)
    vehB.comm.talk()

    print ''

    print "TEST DATABASE CLASS"
    database_items = [['Age_Tracker', 3],['Vehicle_Tracker', 2]]
    vehA.add_database(database_items)
    print "Age tracker", vehA.database.age_tracker
    print "Vehicle tracker\n", vehA.database.vehicle_tracker
    vehA.database.task_completed()
    print "New age tracker", vehA.database.age_tracker



if __name__ == '__main__':

    main()