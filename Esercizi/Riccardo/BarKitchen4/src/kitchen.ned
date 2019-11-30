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

package barkitchen;
import barkitchen.Owner;

module Kitchen
{
    parameters:
     double T = default(1.5);
    @display("bgb=327,189");
    gates:
        input in;
    submodules:
        own: Owner {

            @display("p=179,95;i=abstract/person;is=n");
        }
        c0 : Chef;
        c1 : Chef;
    connections:
        in --> own.in;
        own.out++ --> c0.in;
        own.out++ --> c1.in;

}
