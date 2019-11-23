// Hub Module
// It has two gate vectors, one for input and one for output.
// Whenever it receives a packet from in[i], forwards it through out[i+1]. 
// (manage index wraparound)

package mytictoc;

simple Hub{
    parameters:
    	int n;
    	bool randHub = default(false);	// Point 5
    	volatile int port;
    gates:
        input in[];
        output out[];

//    connections:
//        for i = 0..n-1{
//            in[i%n] --> out[(i+1)%n];   
//        }
}
