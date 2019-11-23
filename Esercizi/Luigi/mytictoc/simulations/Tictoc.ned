package mytictoc.simulations;

import mytictoc.Txc;

network Tictoc{
    submodules:
        tic: Txc;
        toc: Txc;
    connections:
        tic.out --> {delay = 100ms;} --> toc.in;
        tic.in <-- {delay = 100ms;} <-- toc.out;
}
