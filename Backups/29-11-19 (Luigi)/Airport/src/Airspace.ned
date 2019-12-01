package airport;

simple Airspace{
    parameters:
        bool randomInterArrival = default(false);
        double interArrivalRate = default(2);
        bool randomLandingTime = default(false);
        double landingTimeRate = default(3);
                
    gates:
        input in;
        output out;
}
