package producerconsumer;
simple Producer{
    
    parameters:
        bool sendInitialMessage = default(false);
        double responseTime;
        
    gates:
        input in;
        output out;
        output outNotification;
}
