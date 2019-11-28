package airport;

module Aerodrome{
	
	gates:
	    input in;
	    output out;
	
	submodules:
	    parkingarea : ParkingArea;
	    controltower : ControlTower;
	
	connections:
	    parkingarea.in <-- in;
	    parkingarea.out --> out;
}
