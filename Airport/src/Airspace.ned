package airport;

simple Airspace{
    parameters:
        bool randomInterArrival = default(false);
        double interArrivalRate = default(2);
        bool randomLandingTime = default(false);
        double landingTimeRate = default(3);
        
        @signal[checkHoldingQueue](type="int");
        @statistic[HoldingQueueDimension](source="checkHoldingQueue"; record=vector;);
        @signal[airplaneLanding](type="double");
        @statistics[HoldingQueueWaitTime](source="airplaneLanding"; record=vector,mean;);
                
    gates:
        input in;
        output out;
}
