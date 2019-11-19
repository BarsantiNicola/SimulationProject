package mytictoc;

//
// Immediately sends out any message it receives. It can optionally generate
// a message at the beginning of the simulation, to bootstrap the process.
//
simple Txc{
    parameters:
        bool sendInitialMessage = default(false);
        double procTime = uniform(0,5);
        bool enable = default(true);
        // Point 3
       	@signal[Msg](type=long);
        @statistic[countMsg](source="Msg";record=vector;);
        
        //Point 4
       	@signal[_interProcTime](type=double);
        @statistic[_interProcTimeStats](source="_interProcTime";record=vector;);
    gates:
        input in;
        output out;
}
