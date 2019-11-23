package bar;

simple Chef{
    
   	@signal[order](type=int);
	@statistic[orderStats](source="order";record=count;);

    gates:
    	input in; 
}
