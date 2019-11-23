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

package producerconsumer.simulations;

import producerconsumer.Consumer;
import producerconsumer.CentralNode;
import producerconsumer.Producer;
import producerconsumer.Dispatcher;


//
// Two instances (tic and toc) of Txc connected.
//
network ProducerConsumer
{
    @display("bgb=713,329");
    submodules:
        consumer1: Consumer {
            @display("p=78,69;i=device/pc2");
        }
        consumer2: Consumer {
            @display("p=78,233;i=device/pc2");
        }
        centralnode: CentralNode {
            @display("p=341,39;i=abstract/switch");
        }
        producer1: Producer {
            @display("p=600,77;i=device/server");
        }
        producer2: Producer {
            @display("p=600,227;i=device/server");
        }
        dispatcher: Dispatcher {
            @display("p=341,272;i=abstract/dispatcher");
        }
    connections:
        consumer1.out --> {  delay = 100ms; } --> centralnode.in++;
        consumer2.out --> {  delay = 100ms; } -->centralnode.in++;
        centralnode.out++ --> {  delay = 100ms; } --> producer1.in;
        centralnode.out++ --> {  delay = 100ms; } --> producer2.in;
        producer1.out --> {  delay = 100ms; } --> dispatcher.in++;
        producer2.out --> {  delay = 100ms; } --> dispatcher.in++;
        dispatcher.out++ --> {  delay = 100ms; } --> consumer1.in;
        dispatcher.out++ --> {  delay = 100ms; } --> consumer2.in;
}
