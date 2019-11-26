package airport.simulations;

import airport.Airspace;
import airport.Aerodrome;

network Airport{
    submodules:
        airspace : Airspace;
		aerodrome : Aerodrome;
    connections:
        airspace.out --> aerodrome.in;
        aerodrome.out --> airspace.in;
}
