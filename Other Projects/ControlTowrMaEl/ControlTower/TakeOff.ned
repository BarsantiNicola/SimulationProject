
simple TakeOff{
    
    parameters:
        bool expo;												// Option for exponential or constant time values 
        double to @unit(s);										// Takeoff time
        @signal[numAirplanesStat](type int);					// Statistic for airplanes number 
        @signal[waitingTimeTakeOff](type double);
        @statistic[waitingTimeTakeOff](title="meanResponseTime"; 
        	source="waitingTimeTakeOff"; record=vector, mean;);
        @statistic[airplanesStat](title="queueLength"; 
        	source="numAirplanesStat"; record=vector, timeavg, max;);
        
    gates:
        input park;
        input inSky;
        output takeOff;
        output outSky;
        
}
