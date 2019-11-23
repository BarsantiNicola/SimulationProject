//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package tichubtoc.simulations;

import tichubtoc.Txc;
import tichubtoc.Hub;

//
// Two instances (tic and toc) of Txc connected.
//
network Tictoc
{
    @display("bgb=826,315");
    submodules:
        tic: Txc {
            @display("p=59,77;i=device/pc2");
        }
        toc: Txc {
            @display("p=685,92;i=device/pc2");
        }
        hub: Hub {
            @display("p=372.4625,166;i=abstract/switch");
        }
    connections:
        tic.out --> {  delay = 100ms; } --> hub.in++;
        toc.out --> {  delay = 100ms; } --> hub.in++;

        hub.out++ --> {  delay = 100ms; } --> tic.in;
        hub.out++ --> {  delay = 100ms; } --> toc.in;
}
