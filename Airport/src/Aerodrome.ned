package airport;

module Aerodrome
{
    gates:
        input in;
        output out;

    submodules:
        parkingarea: ParkingArea {
            @display("p=139,152");
        }
        controltower: ControlTower {

            @display("p=139,67");
        }
    connections:
        parkingarea.in <-- in;
        parkingarea.out --> out;
}
