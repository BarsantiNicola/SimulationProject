package producerconsumer;
simple Consumer{
    
    parameters:
        bool sendInitialMessage = default(false);
        double consumptionTime;
        int id;
        
    gates:
        input in;
        output out;
}
