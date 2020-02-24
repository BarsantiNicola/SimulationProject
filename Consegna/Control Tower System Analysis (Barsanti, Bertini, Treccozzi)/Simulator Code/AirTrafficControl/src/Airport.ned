package src;

//The Airport
module Airport
{
    @display("bgb=226,219,#D3D7CF;i=misc/building;bgi=background/streetmap");

    gates:
        input in;     //Towards the Parking Area module
        output out;   //Towards the Airspace module

    submodules:
        parkingarea: ParkingArea {
            @display("p=138,121");
        }
        controltower: ControlTower {
            @display("p=57,53");
        }

    connections:
        parkingarea.in <-- in;
        parkingarea.out --> out;
}
