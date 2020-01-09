# Project 11 - Control tower
A control tower manages the air traffic of an airport. The airport has one runway for landing/take-off and one parking area for airplanes.
The runway can be occupied by one airplane at a time, whereas the parking area can contain one or more of them simulaneously. The parking area have infinite capacity.

Wherever an airplane reaches the airport area, it goes through the following steps:
  1. the airplane queues for landing until it receives the ok from the control tower (assume that airplanes always have eough fueld to remain in the air for an arbitrary time);
  2. the airplane performs the landing operation, which takes a time t<sub>l</sub>;
  3. the airplane remains in the parking area for a time t<sub>p</sub>;
  4. the airplane queues for take-off untile it receives the ok from the control tower;
  5. the airplane takes-off in a time t<sub>t</sub> and leaves the system.
  
When the runway is unoccopied, the control tower serves the airplane with the largest waiting time in either queues.

Consider the following workload: airplane interarrival times are IID RVs (to be described later), the values t<sub>l</sub>, t<sub>p</sub> and t<sub>o</sub> are IID RV (to be described later).

Model the system described above and study the waiting time in both landing and take-off queues with a varying workload. Study how the number of airplanes in the parking area varies in relation with t1 and tp. More in detail, at least the following scenarios must be evaluated;
  - Constant interarrival times, constant service times.
  - Exponential distribution of all the above RVs, with the same means as the previous case.
  
In all cases, it is up to the team to calibrate the scenarios so that meaningful results are obtained.

Project deliverables:
  a) Documentation (according to the standards et during the lectures)
  b) Simulation code
  c) Presentation (up to 10 slides maximum)
