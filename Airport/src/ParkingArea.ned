package airport;

simple ParkingArea{
    parameters:
        bool isParkingTimeRandom = default(false);
        double parkingTimeRate = default(4);
        bool isTakeoffTimeRandom = default(false);
        double takeoffTimeRate = default(5);
        
        @signal[checkParkedPlanes](type="long");
        @statistic[ParkedPlanesNumber](source="checkParkedPlanes"; record=vector;);
        @signal[checkDepartQueue](type="int");
        @statistic[DepartQueueDimension](source="checkDepartQueue"; record=vector;);
        @signal[airplaneTakeoff](type="double");
        @statistics[DepartQueueWaitTime](source="airplaneTakeoff"; record=vector,mean;);
                    
    gates:   
        input in;
        output out;
}
