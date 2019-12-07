namespace airport;

//An extension of the cMessage class representing an airplane
message Airplane
 {
  long airplaneID;             //The Airplane ID (Note that the "messageID" field of the "cMessage" class could not be used due to internal message duplications performed by the simulator)
  double queueArrivalTime;     //Initialized when the airplane is inserted into a queue for statistic collection purposes
 }
