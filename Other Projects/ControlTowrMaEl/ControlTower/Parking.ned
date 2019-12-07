
simple Parking{
    
    parameters:
        bool expo;														// Option for constant or exponential interarrivals
        int numAirplanes = default(0);									// Airplanes number statistic
        double tp @unit(s);												// Parking time 
        @signal[numAirplanesStat](type int);							// Statisc for airplanes number 
        @statistic[airplanesStat](title="queueLength"; 
       		source="numAirplanesStat"; record=vector, timeavg, max;);
        
    gates:
        input park;
        output takeOff;
}
