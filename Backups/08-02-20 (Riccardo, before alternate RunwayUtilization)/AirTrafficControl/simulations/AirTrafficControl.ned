package simulations;

import src.Airspace;
import src.Airport;

//The Airport System
network AirTrafficControl
{
    @display("bgb=339,255;bgi=background/terrain");

    //TODO remove? ----------------------------------
    @signal[TotalSimtime](type="double");
    @signal[RunwayUse](type="double");
    @statistic[Runwayutilization](source=sum(RunwayUse)/last(TotalSimtime); record=last; title = "Runway Utilization");
    //-----------------------------------------------

    submodules:
        airspace: Airspace {
            @display("p=248,118;is=vl");
        }

        airport: Airport {
            @display("p=71,119;is=vl");
        }

    connections:
        airspace.out --> airport.in;
        airport.out --> airspace.in;
}
