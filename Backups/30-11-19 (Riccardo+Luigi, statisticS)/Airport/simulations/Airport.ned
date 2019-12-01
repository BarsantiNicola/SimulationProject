package airport.simulations;

import airport.Airspace;
import airport.Aerodrome;

network Airport
{
    @display("bgb=303,230");
    submodules:
        airspace: Airspace {
            @display("p=232,97");
        }
        aerodrome: Aerodrome {
            @display("p=53,105");
        }
    connections:
        airspace.out --> aerodrome.in;
        aerodrome.out --> airspace.in;
}
