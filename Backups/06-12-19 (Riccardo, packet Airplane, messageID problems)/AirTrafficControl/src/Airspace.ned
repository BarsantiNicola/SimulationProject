package airport;

//The Airspace surrounding the Airport
simple Airspace
{
    parameters:

        //----------------------------------Input Parameters----------------------------------   
        bool isInterArrivalTimeRandom = default(false);
        bool isLandingTimeRandom = default(false);
        double interArrivalTime = default(10);
        double landingTime = default(5);

  //----------------------------Statistics-related Parameters----------------------------  

        //1) Airplanes' Holding Queue Size
        @signal[HoldingQueueSize](type="long");
        @statistic[HoldingQueueSize](source="HoldingQueueSize"; record=vector; );

        //2) Airplanes' Holding Queue Waiting Time    
        @signal[HoldingQueueWaitingTime](type="double");
        @statistic[HoldingQueueWaitingTime](source="HoldingQueueWaitingTime"; record=vector; );

        //3) Airport System Response Time
        @signal[AirportResponseTime](type="double");
        @statistic[AirportResponseTime](source="AirportResponseTime"; record=vector; );

        @display("i=misc/cloud");
    gates:
        input in;    //From the Parking Area
        output out;  //To the Parking Area
}
