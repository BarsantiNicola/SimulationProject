package bar;

simple Owner{
	
	@signal[interArrivalTime](type=double);
	@statistic[interArrivalTimeStats](source="interArrivalTime";record=vector;);
    
    gates:
        input in;
        output out[];
}
