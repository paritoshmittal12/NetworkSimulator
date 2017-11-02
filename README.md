# <p align="center"> ns-3 based Network Simulator<p>
ns3 is a discrete event netowrk simulator, used to simulate topologies of wired and wireless networks. We use it to simulate one wired and one wireless plus wired network.

We wish to log and plot the network attributes and properties like throughput and Fairness index.

#### <p align="center"> Topology for the application </p>
---

	100 Mbps,20ms                 10Mbps,50ms              100 Mbps,20ms
n2__________________r1___________________r2___________________________n3


						10Mbps,100ms          
n0------------------bs1____________________bs2-------------------n1

____________   wired

------------------- wireless

**r1** and **r2** are routers and **bs1** and **bs2** are base-stations.