package producerconsumer;
simple Deliverer{
    parameters:
        bool sendInitialMessage = default(false);
        int nConsumers = default(2);
        int nProducers = default(2);
        
    gates:
        input in[nProducers];
        input freeNotification[nProducers];
        output out[nConsumers];
}
