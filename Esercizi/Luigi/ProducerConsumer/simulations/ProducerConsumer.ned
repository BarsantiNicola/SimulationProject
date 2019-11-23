package producerconsumer.simulations;

import producerconsumer.Producer;
import producerconsumer.Consumer;
import producerconsumer.Deliverer;
import producerconsumer.Dispatcher;

network ProducerConsumerNetwork{
    submodules:
        consumerA: Consumer;
        consumerB: Consumer;
        producer1: Producer;
        producer2: Producer;
        deliverer: Deliverer;
        dispatcher: Dispatcher;
        
    connections:
        consumerA.out --> deliverer.in++;
        consumerB.out --> deliverer.in++;
        deliverer.out++ --> producer1.in;
        deliverer.out++ --> producer2.in;
        producer1.outNotification --> deliverer.freeNotification++;
        producer2.outNotification --> deliverer.freeNotification++;
        producer1.out --> dispatcher.in++;
        producer2.out --> dispatcher.in++;
        dispatcher.out++ --> consumerA.in;
        dispatcher.out++ --> consumerB.in;
}
