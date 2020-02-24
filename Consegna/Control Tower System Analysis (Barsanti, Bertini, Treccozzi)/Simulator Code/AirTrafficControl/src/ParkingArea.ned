package src;

//The Airport's Parking Area
simple ParkingArea
{
    parameters:

        //----------------------------------Input Parameters----------------------------------
        bool isParkingTimeRandom = default(false);
        bool isTakeoffTimeRandom = default(false);
        double parkingTime = default(10);
        double takeoffTime = default(5);

        //--------------------------Signals and Statistics Declaration--------------------------
         
        //Parked Planes
        @signal[ParkedPlanes](type="long");
        @statistic[ParkedPlanes](source="ParkedPlanes"; record = mean, vector; title = "Parked PLanes");

        //Depart Queue Size
        @signal[DepartQueueSize](type="long");
        @statistic[DepartQueueSize](source="DepartQueueSize"; record = mean, vector; title = "Depart Queue Sizes" );

        //Depart Queue Waiting Time
        @signal[DepartQueueWaitingTime](type="double");
        @statistic[DepartQueueWaitingTime](source="DepartQueueWaitingTime"; record = mean, vector; title = "Depart Queue Waiting Time");

        @display("i=misc/town2");
        
    gates:
        input in;    //From the Airspace
        output out;  //To the Airspace
}
