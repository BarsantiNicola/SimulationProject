package mytictoc.simulations;

import mytictoc.Txc;
import mytictoc.Hub;

network TicHubToc{

    submodules:
        tic: Txc;
        hub: Hub;
        toc: Txc;
        
    connections:
        tic.out --> hub.in++;
        toc.out --> hub.in++;
        
        hub.out++ --> tic.in;
        hub.out++ --> toc.in;
}
