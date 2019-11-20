package bar;

module Kitchen{
    // Esercizio 2
    double T;
    
	gates:
        input in;
    
    submodules:
        owner: Owner;
        
        //Esercizio 2
        chef0: Chef;
        chef1: Chef;
    
    connections:
        owner.in <-- in;
        
        //Esercizio 2
        owner.out++ --> chef0.in;
        owner.out++ --> chef1.in;
        
}
