package producerconsumer;
simple Dispatcher{
    parameters:
        bool sendInitialMessage = default(false);
        int nConsumers = default(2);
        int nProducers = default(2);
        
    gates:
        input in[nConsumers];
        output out[nProducers];
}
