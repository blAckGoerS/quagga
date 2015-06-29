OVERVIEW
--------

* PEMP-Quagga is a modified version of Quagga routing daemon (http://www.nongnu.org/quagga/). It is an open source software router that offers peering equilibrium extensions to the basic BGP routing decision process. The goal is to improve the efficiency of inter domain routing by offering Autonomous Systems (ASs) a multipath routing coordination framework. The PEMP routing framework is meant to go beyond hot-potato routing, which often leads to bad global routing solutions, by means of strategic multipath load-balancing across multiple eBGP links. This is done by building a non-cooperative game structure over internal routing path costs (IGPs), possibly also on network management costs. The PEMP solution is such that the unilateral routing priorities are respected, while allowing to outperform basic hot-potato routing (e.g., closest exit) and to tend toward a sort of "mild-potato" routing, a better bilateral routing taking at a strategically acceptable extent the preference of the neighbor AS.
* The routing game logic applies game theory concepts to select BGP paths received by the same neighbor, paths without or with equal local-preference. The algorithmic framework is explained in detail in [1]. We make use of the routing game library available at [2].

     

HOW TO INSTALL
--------------

1> Download the software

	From git hub https://github.com/routing-games/

2> Pre installation check 

* To be fully functional, PEMP-Quagga is recommended to be installed on a Linux platform that supports multipath and policy-based routing. For instance, it has been developed and tested under Ubuntu 14.04.2 with Linux kernel 3.16

* Texinfo, gwak and automake are prerequisite packages that need to be installed

		sudo apt-get install gawk

		sudo apt-get install texinfo

		sudo apt-get install automake 

3> Configure the software 

* Turn on multipath routing support  

		./configure --enable-multipath=0

* Force Quagga router to use GNU/Linux netlink interface for communicating with the kernel 

		./configure --enable-netlink

* Enable Quagga to run as root (optional)      

		./configure --enable-user=root

		./configure --enable-group=root    

4> Install the software  

		make install 



HOW TO CONFIGURE
----------------

* To enable PEMP features, we need to include the following commands in both BGP and OSPF configuration files. 

  By default, the configuration files of BGP and OSPF is located at /usr/local/etc/bgpd.conf, /usr/local/etc/ospfd.conf respectively.  

1> BGP's configuration

   router bgp <ASN>  

    bgp router-id <IP ADDRESS> 

    bgp bestpath as-path multipath-relax

		maximum-paths ibgp <NUMBER OF PATH>

    redistribute ospf

    neighbor <IP ADDRESS> remote-as <ASN>

    neighbor <IP ADDRESS> remote-as <ASN>

    neighbor <IP ADDRESS> remote-as <ASN>

    pemp enable 

		routing-game community local-id <COMMUNITY ID> peer-id <COMMUNITY ID> configure <COORDINATION POLICY> <POTENTIAL THRESHOLD> <USE LOAD BALANCING> 

		local network <PREFIX> community <LOCAL COMMUNITY ID>

		peer network  <PREFIX> community <PEER COMMUNITY ID>


bgp bestpath as-path multipath-relax
  + Enable bgpd to support multipath routing for inter domain traffic. By default, Quagga router does not enable BGP multipath routing, we need to turn on this feature on the configuration file. 

maximum-paths ibgp <NUMBER OF PATH>
  + Specify the maximum number of paths you want to support 

redistribute ospf
  + Allow IGP path cost distribution from the IGP process (in this case is ospfd) to bgpd  

pemp enable
  + Enable peering equilibrium multipath routing support 

routing-game community local-id <local community id> peer-id <peer community id> configure <coordination policy> <potential threshold> <load balancing>
  + Establish a routing game with peering AS for traffic flow from the its local community to peer's community, this flow is defined by the pair local community ID - peer community ID. As in standard BGP, a community is a group of network prefixes. A specified PEMP community pair receives the same PEMP routing decision made, and each community is identified by a unique community ID. The command above also help to initializes the game with input parameters specifying the coordination policy being used, the value of potential threshold (used in the load-balancing distribution computation) as well as the load-balancing mode. These parameters are specified after the "configure" command.By issuing this command, any packets from the local community id destined to the peer community id will be routed according to PEMP.
  + Configuration parameter
    <local community id> accepts any integer positive number; the number should be defined by the local AS and communicated as of current practices to the peering AS 
    <peer community id> accepts any integer positive number; the number should be defined by the peering AS and communicated to the local AS to avoid duplication  
    <coordination policy> is an integer number from 0 to 3, with the following meaning in terms of equilibrium selection policy (see [1]):
      0: Nash equilibrium multipath (NEMP)
      1: Pareto-frontier
      2: Pareto-jump
      3: Unselfish jump
    <potential threshold> is any real positive number 
    <load balancing> only accepts 0 or 1 as value, in which
      0: Even load-balancing 
      1: Subflow explicit load-balancing 

* NOTE: both local AS and peering AS must agree on the value of community ID assigned for each community they manage. If AS I assigns community ID 2 for its customer network 192.168.2.0/24, so AS II (peer of AS I) must also agree that the community ID 2 is reserved for the network 192.168.2.0/24 at AS I. 

  
2> OSPF's configuration

border router-id <border router's IP address> 

	Enable IGP path cost calculation for the path between PEMP enable router and specified border router. If we do not specify the IP address of egress router here, 

	the IGP path cost value of this router could not be calculated and it would not be considered as an option in the routing game. 

     

3> Example

* Scenario: AS 6002 interconnects with AS 6004 via three peering links. Within AS 6002, router RA (192.168.2.4) connects directly with network A (192.168.2.0/24),and learns three different egress routers RA1 (192.168.1.6), RA2 (192.168.4.6), and RA3 (192.168.5.6) to forward traffic from A to any external networks advertised by AS 6004. WithinAS 6004, router RB (192.168.3.2) connects directly with network B (192.168.3.0/24), and learns three different egress routers RB1 (192.168.21.6), RB2 (192.168.24.6), and RB3 (192.168.25.6) to send traffic from A to any external networks advertised by AS 6002. AS 6002 would like makes a peering agreement with AS 6004, in which both peers agree that traffic between network A in 6002 and network B in 6004 is sensitive to delay and it deserves carefully routing. Beside that the amount of traffic that A sends to B is also quite similar to the amount of traffic sent from B to A. Network operators agree to enable PEMP routing for traffic between A and B. To do so they need to configure router RA and RB with new PEMP supporting configuration. The new configuration files are presented as follow:

+ A snapshot of BGP configuration file in router RA of AS 6002 


		router bgp 6002  

		bgp router-id 192.168.2.4 

		bgp bestpath as-path multipath-relax

		maximum-paths 3 

		maximum-paths ibgp 3

		redistribute ospf

		neighbor 192.168.1.6 remote-as 6002

		neighbor 192.168.4.6 remote-as 6002

		neighbor 192.168.5.6 remote-as 6002

		pemp enable 

			routing-game community local-id 2 peer-id 3 configure 1 2 1 

			local network 192.168.2.0/24 community 2

			peer network 192.168.3.0/24 community 3

   

+ A snapshot of OSPF configuration file in router RA of AS 6002


		router ospf 

		ospf router-id 192.168.2.4

		border router-id 192.168.1.6 

		border router-id 192.168.4.6

		border router-id 192.168.5.6

		network 192.168.1.0/24 area 0.0.0.0

		network 192.168.2.0/24 area 0.0.0.0

		network 192.168.4.0/24 area 0.0.0.0

		network 192.168.5.0/24 area 0.0.0.0


+ A snapshot of BGP configuration file in router RB of AS 6004 


		router bgp 6004           

		bgp router-id 192.168.3.2 

		bgp bestpath as-path multipath-relax

		maximum-paths 3 

		maximum-paths ibgp 3

		redistribute ospf

		neighbor 192.168.21.6 remote-as 6004

		neighbor 192.168.24.6 remote-as 6004

		neighbor 192.168.25.6 remote-as 6004

		pemp enable 

			routing-game community local-id 3 peer-id 2 configure 1 2 1 

			local network 192.168.3.0/24 community 3

			peer network 192.168.2.0/24 community 2


+ A snapshot of OSPF configuration file in router RB of AS 6004


		router ospf 

		ospf router-id 192.168.3.2

		border router-id 192.168.21.6 

		border router-id 192.168.24.6

		border router-id 192.168.25.6

		network 192.168.3.0/24 area 0.0.0.0

		network 192.168.21.0/24 area 0.0.0.0

		network 192.168.24.0/24 area 0.0.0.0

		network 192.168.25.0/24 area 0.0.0.0

     

HOW TO RUN
----------

* The PEMPR routing process require collecting interior gateway protocol routing costs to feed the MED attribute, so it requires 

  the execution of both bgpd and ospfd daemons. Zebra updates the kernel routing table and it is mandatory to run ospfd. 

  Therefore to run a PEMP enable router you need to run:  

    # zebra -d 
    # ospfd -d 
    # bgpd  -d

	

* The result of PEMP routing decision is recorded in a log file with name formatted as "routing_game" followed by "HH:MM:AM/PM" which indicates 

  the timestamp the routing game is built and the decision is made.  
 



REFERENCES

---------
[1] Stefano SECCI, Jean-Louis ROUGIER, Achille PATTAVINA, Fioravante PATRONE, Guido MAIER, "Peering Equilibrium MultiPath Routing: a game theory framework for Internet peering settlements", IEEE/ACM Transactions on Networking, Vol. 19, No. 2, pp: 419-432, April 2011. http://www-phare.lip6.fr/~secci/papers/SeRoPaPaMa-ToN-11.pdf

[2] Routing game library open source project: https://github.com/routing-games/

