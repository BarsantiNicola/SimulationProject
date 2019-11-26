package airport;

simple ParkingArea{
    parameters:
        bool isParkingTimeRandom = default(false);
        double parkingTimeRate = default(4);
        bool isTakeoffTimeRandom = default(false);
        double takeoffTimeRate = default(5);
                    
    gates:   
        input in;
        output out;
}
