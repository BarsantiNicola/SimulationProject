package simulations;

import src.Airspace;
import src.Airport;

//The Airport System
network AirTrafficControl
{
    @display("bgb=339,255;bgi=background/terrain");

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
