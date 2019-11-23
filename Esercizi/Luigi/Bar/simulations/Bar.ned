package bar.simulations;

import bar.Waitress;
import bar.Kitchen;

network Bar{
    submodules:
        waitress: Waitress;
        kitchen: Kitchen;
    connections:
        waitress.out -->  kitchen.in;
}
