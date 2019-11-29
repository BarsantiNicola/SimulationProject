
simple Sky{
    
    parameters:
        bool expo;												// Option for exponential or constant time values
        double lambda @unit(s);									// Interarrivals time 
        double tl @unit(s);										// Landing time 
        @signal[waitingTimeLanding](type double);				// Statistic for waiting time 
        @statistic[waitingTimeLanding](title="meanResponseTime"; 
        	source="waitingTimeLanding"; record=vector, mean;);
        @signal[numAirplanesStat](type int);					// Statistic for airplanes number 
        @statistic[airplanesStat](title="queueLength"; 
        	source="numAirplanesStat"; record=vector, timeavg, max;);
        
    gates:
        output runway;
        output outTakeoff;
        input inTakeoff;
        
}
