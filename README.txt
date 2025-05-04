 This will be my stopping point for the class, will try to continue after.
WCET analysis is 80% in terms of overshooting and undershooting the cycle counts. 
reason being is loop bounds being changed can alter WCET a lot and implementing ways
detect those kinds of 'bumps' can be extremely time consuming especially for someone without
much expirience with this. Either way I learned a whole bunch about WCET just how sensitive
these kinds of systems can be and just how hard it is to implement accurate WCET analysis 
to real world test cases. As i said my test cases dont mess with the loop bound inside the 
bodt and tend to be pretty simple. I can only imagine how much more complicated it gets when
trying to implement this for sophisticated test cases.