
network Airport
{
    parameters:
        @display("bgb=763,400");
        bool expo;											// Option for constant or exponential time values 		

    submodules:

        sky: Sky {
            expo = expo;
            @display("p=144,117;is=vl;i=misc/cloud");
        }

        runway: Runway {
            @display("p=369,118;i=old/xconn");
        }

        parking: Parking {
            expo = expo;
            @display("p=643,118;i=block/table2");
        }

        takeOff: TakeOff {
            expo = expo;
            @display("p=369,300;i=block/departure");
        }

    connections:
        runway.park --> parking.park;
        parking.takeOff --> takeOff.park;
        takeOff.takeOff --> runway.takeOff;
        sky.runway --> runway.sky;
        sky.outTakeoff --> takeOff.inSky;
        takeOff.outSky --> sky.inTakeoff;
}
