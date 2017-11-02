#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/gnuplot.h"



/*

Topology for the application


	100 Mbps,20ms        10Mbps,50ms          100 Mbps,20ms
n2__________________r1___________________r2______________________n3


   				         10Mbps,100ms          
n0------------------bs1____________________bs2-------------------n1

___________________ wired

------------------- wireless
  
*/



using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("Assignment4");

int main(int argc, char *argv[])
{
	NS_LOG_INFO("Start code!");
	CommandLine cmd;
  	cmd.Parse (argc, argv);
	
  	// starting to make plt files for graph

	std :: string file1 = "packetsize vs throughput wired", file2 = "packetsize vs throughput wireless", file3 = "packetsize vs Fairness wired", file4 = "packetsize vs Fairness wireless", file5 = "3dwired", file6 = "3dwireless";
	std :: string GFile1 = file1 + ".png", GFile2 = file2 + ".png", GFile3 = file3 + ".png", GFile4 = file4 + ".png", GFile5 = file5 + ".png", GFile6 = file6 + ".png";
	std :: string PFile1 = file1 + ".plt", PFile2 = file2 + ".plt", PFile3 = file3 + ".plt", PFile4 = file4 + ".plt", PFile5 = file5 + ".plt", PFile6 = file6 + ".plt";
	std :: string plotTitle1 = "Effect of packetsize change on throughput", plotTitle2 = "Effect of packetsize change on throughput", plotTitle3 = "Effect of packetsize change on Fairness", plotTitle4 = "Effect of packetsize change on Fairness";
	std :: string plotTitle5 = "packetsize vs throughput vs Fairness";
	std :: string dataTitle  = "data rate 200Kbps/300Kbps ";

	// Instantiate the plot and set its title.
	Gnuplot plot1 (GFile1);
	plot1.SetTitle (plotTitle1);

	Gnuplot plot2 (GFile2);
	plot2.SetTitle (plotTitle2);

	Gnuplot plot3 (GFile3);
	plot3.SetTitle (plotTitle3);

	Gnuplot plot4 (GFile4);
	plot4.SetTitle (plotTitle4);

	Gnuplot plot5 (GFile5);
	plot5.SetTitle (plotTitle5);

	Gnuplot plot6 (GFile6);
	plot6.SetTitle (plotTitle5);
	// Make the graphics file, which the plot file will create when it
	// is used with Gnuplot, be a PNG file.
	plot1.SetTerminal ("png");
	plot2.SetTerminal ("png");
	plot3.SetTerminal ("png");
	plot4.SetTerminal ("png");
	plot5.SetTerminal ("png");
	plot6.SetTerminal ("png");

	// Set the labels for each axis.
	plot1.SetLegend ("Packet Size", "Throughput");
	plot2.SetLegend ("Packet Size", "Throughput");
	plot3.SetLegend ("Packet Size", "Fairness");
	plot4.SetLegend ("Packet Size", "Fairness");
	plot5.AppendExtra ("set xlabel 'Packet Size'");
	plot5.AppendExtra ("set ylabel 'throughput'");
	plot5.AppendExtra ("set zlabel 'Fairness'");
	plot6.AppendExtra ("set xlabel 'Packet Size'");
	plot6.AppendExtra ("set ylabel 'throughput'");
	plot6.AppendExtra ("set zlabel 'Fairness'");

	// Set the range for the x axis.
	plot1.AppendExtra ("set xrange [0:1600]");
	plot2.AppendExtra ("set xrange [0:1600]");
	plot3.AppendExtra ("set xrange [0:1600]");
	plot4.AppendExtra ("set xrange [0:1600]");
	plot5.AppendExtra ("set view 30, 120, 1.0, 1.0");
	plot6.AppendExtra ("set view 30, 120, 1.0, 1.0");
	// plot.AppendExtra ("set xrange [0:1600]");
	// Instantiate the dataset, set its title, and make the points be
	// plotted along with connecting lines.
	Gnuplot2dDataset dataset1, dataset2, dataset3, dataset4;

	// making 2 3D graph for wired and wireless topology
	Gnuplot3dDataset dataset5, dataset6;
	dataset1.SetTitle (dataTitle);
	dataset1.SetStyle (Gnuplot2dDataset::LINES_POINTS);
	
	dataset2.SetTitle (dataTitle);
	dataset2.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	dataset3.SetTitle (dataTitle);
	dataset3.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	dataset4.SetTitle (dataTitle);
	dataset4.SetStyle (Gnuplot2dDataset::LINES_POINTS);

	dataset5.SetTitle (dataTitle);
	dataset5.SetStyle ("with lines");

	dataset6.SetTitle (dataTitle);
	dataset6.SetStyle ("with lines");
	// Initialization Complete

	Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));  //Reno is not present in NS3

	// packetsize is varied according to the problem statement 
	uint32_t packetsize = 40;
	while( packetsize <= 1500)
	{

	
	Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (packetsize));

	// node containers are made which act as hosts and routers
	NodeContainer wirelink1;
	NodeContainer wlesslink;
	NodeContainer routers;
	NodeContainer bs;
	NodeContainer wirelink2;

	wirelink1.Create(2);	
	wirelink2.Create(2);
	wlesslink.Create(2);
	bs.Create(2);
	routers.Add(wirelink1.Get(1));
	routers.Add(wirelink2.Get(0));

	// channel settings for wired network
	PointToPointHelper ppp,p2p;
	ppp.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
	ppp.SetChannelAttribute("Delay", StringValue("20ms"));
	
	NetDeviceContainer w_link1, w_link2, router_dev;
	w_link1 = ppp.Install(wirelink1);
	w_link2 = ppp.Install(wirelink2);

	// characteristics of channel are set
	ppp.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
	ppp.SetChannelAttribute("Delay", StringValue("50ms"));
	ppp.SetQueue ("ns3::DropTailQueue", "Mode", StringValue ("QUEUE_MODE_BYTES"), "MaxBytes", UintegerValue (62500));  //packet size check
	router_dev = ppp.Install (routers);

	//channel settings for wireless network
	
	YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
	YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
	phy.SetChannel (channel.Create());

	WifiHelper wifi;
	wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

	WifiMacHelper mac;
	Ssid ssid = Ssid ("lab4-ssid");
	mac.SetType ("ns3::StaWifiMac","Ssid",SsidValue (ssid),"ActiveProbing",BooleanValue (false));

	NetDeviceContainer wl_link1, bs_dev1;
	wl_link1 =  wifi.Install (phy, mac, wlesslink.Get (0));

	mac.SetType ("ns3::ApWifiMac","Ssid", SsidValue (ssid));

	bs_dev1 = wifi.Install (phy, mac, bs.Get (0));
	// bs_dev2 = wifi.Install (phy, mac, bs.Get(1));

	// mobility factor for wireless is assigned
	MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility.Install (wlesslink.Get (0));

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (bs.Get (0));


  //	new wireless channel for different router and host in wireless topology
	YansWifiChannelHelper channel1 = YansWifiChannelHelper::Default ();
	YansWifiPhyHelper phy1 = YansWifiPhyHelper::Default ();
	phy1.SetChannel (channel1.Create());

	WifiHelper wifi1;
	wifi1.SetRemoteStationManager ("ns3::AarfWifiManager");

	WifiMacHelper mac1;
	Ssid ssid1 = Ssid ("lab4-ssid1");
	mac1.SetType ("ns3::StaWifiMac","Ssid",SsidValue (ssid1),"ActiveProbing",BooleanValue (false));

	NetDeviceContainer wl_link2, bs_dev, bs_dev2;
	wl_link2 =  wifi1.Install (phy1, mac1, wlesslink.Get (1));

	mac1.SetType ("ns3::ApWifiMac","Ssid", SsidValue (ssid1));

	bs_dev2 = wifi1.Install (phy1, mac1, bs.Get (1));

	mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility.Install (wlesslink.Get (1));

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (bs.Get (1));

  // p2p channel for 2 basestations wired connection

	p2p.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
	p2p.SetChannelAttribute("Delay", StringValue("100ms"));
	p2p.SetQueue ("ns3::DropTailQueue", "Mode", StringValue ("QUEUE_MODE_BYTES"), "MaxBytes", UintegerValue (125000));  //packet size check

	bs_dev = p2p.Install(bs);
	

	// stack implementation
	InternetStackHelper stack;
	stack.Install(wirelink1);
	stack.Install(wirelink2);
	stack.Install(wlesslink);
	stack.Install(bs);

	//IP assignment for wired
	Ipv4AddressHelper address;
	address.SetBase ("10.1.1.0","255.255.255.0");
	Ipv4InterfaceContainer w_link1_iface, w_link2_iface, router_iface;
	w_link1_iface = address.Assign (w_link1);

	address.SetBase("10.1.2.0","255.255.255.0");
	w_link2_iface = address.Assign(w_link2);

	address.SetBase("10.1.3.0","255.255.255.0");
	router_iface = address.Assign(router_dev);


	//IP assignment for wireless
	Ipv4InterfaceContainer wl_iface_server, wl_iface_client;

	address.SetBase ("10.2.1.0","255.255.255.0");
	wl_iface_client = address.Assign(wl_link1);
	address.Assign(bs_dev1);

	address.SetBase ("10.2.2.0","255.255.255.0");
	wl_iface_server = address.Assign(wl_link2);
	address.Assign(bs_dev2);

	address.SetBase ("10.2.3.0","255.255.255.0");
	address.Assign(bs_dev);


	//TCP routing for wired
	Address local_addr (InetSocketAddress(Ipv4Address::GetAny(),6666));
	PacketSinkHelper pshelper ("ns3::TcpSocketFactory", local_addr);
	ApplicationContainer wire_server = pshelper.Install (wirelink2.Get(1));
	std::cout << "\nServer address for wired connection is : \t" << w_link2_iface.GetAddress(1) << std ::endl;
	wire_server.Start (Seconds (0));
	wire_server.Stop (Seconds (10));
  	
  	AddressValue remote_addr (InetSocketAddress (w_link2_iface.GetAddress (1), 6666));

  	//onoff aplication no 1 for wired


	OnOffHelper onoff_w1 ("ns3::TcpSocketFactory", Ipv4Address::GetAny ());
  	onoff_w1.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  	onoff_w1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  	onoff_w1.SetAttribute ("PacketSize", UintegerValue (packetsize));
  	onoff_w1.SetAttribute ("DataRate", StringValue ("30Mbps")); //bit/s
  	ApplicationContainer onoffclient_w1;

  	onoff_w1.SetAttribute ("Remote", remote_addr);
  	onoffclient_w1.Add (onoff_w1.Install (wirelink1.Get(0)));
  	onoffclient_w1.Start (Seconds (0));
  	onoffclient_w1.Stop (Seconds (10));

  	//onoff aplication no 2 for wired

  	OnOffHelper onoff_w2 ("ns3::TcpSocketFactory", Ipv4Address::GetAny ());
  	onoff_w2.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  	onoff_w2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  	onoff_w2.SetAttribute ("PacketSize", UintegerValue (packetsize));
  	onoff_w2.SetAttribute ("DataRate", StringValue ("30Mbps")); //bit/s
  	ApplicationContainer onoffclient_w2;

  	onoff_w2.SetAttribute ("Remote", remote_addr);
  	onoffclient_w2.Add (onoff_w2.Install (wirelink1.Get(0)));
  	onoffclient_w2.Start (Seconds (0));
  	onoffclient_w2.Stop (Seconds (10));

	//onoff aplication no 3 for wired

  	OnOffHelper onoff_w3 ("ns3::TcpSocketFactory", Ipv4Address::GetAny ());
  	onoff_w3.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  	onoff_w3.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  	onoff_w3.SetAttribute ("PacketSize", UintegerValue (packetsize));
  	onoff_w3.SetAttribute ("DataRate", StringValue ("30Mbps")); //bit/s
  	ApplicationContainer onoffclient_w3;

  	onoff_w3.SetAttribute ("Remote", remote_addr);
  	onoffclient_w3.Add (onoff_w3.Install (wirelink1.Get(0)));
  	onoffclient_w3.Start (Seconds (0));
  	onoffclient_w3.Stop (Seconds (10));

	//onoff aplication no 4 for wired

  	OnOffHelper onoff_w4 ("ns3::TcpSocketFactory", Ipv4Address::GetAny ());
  	onoff_w4.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  	onoff_w4.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  	onoff_w4.SetAttribute ("PacketSize", UintegerValue (packetsize));
  	onoff_w4.SetAttribute ("DataRate", StringValue ("30Mbps")); //bit/s
  	ApplicationContainer onoffclient_w4;

  	onoff_w4.SetAttribute ("Remote", remote_addr);
  	onoffclient_w4.Add (onoff_w4.Install (wirelink1.Get(0)));
  	onoffclient_w4.Start (Seconds (0));
  	onoffclient_w4.Stop (Seconds (10));

  	//Tcp routing for wireless
  	Address l_addr (InetSocketAddress(Ipv4Address::GetAny(),7666));
	PacketSinkHelper pshelper1 ("ns3::TcpSocketFactory", l_addr);
	ApplicationContainer wl_server = pshelper1.Install (wlesslink.Get(1));
	std::cout << "Server address for wireless connection is : \t" << wl_iface_server.GetAddress(0) << std ::endl;
	wl_server.Start (Seconds (0));
	wl_server.Stop (Seconds (10));
  	
   	Address r_addr (InetSocketAddress (wl_iface_server.GetAddress (0), 7666));

  	
  	//onoff aplication no 1 for wireless

	OnOffHelper onoff_wl1 ("ns3::TcpSocketFactory", r_addr);
  	onoff_wl1.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  	onoff_wl1.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  	onoff_wl1.SetAttribute ("PacketSize", UintegerValue (packetsize));
  	onoff_wl1.SetAttribute ("DataRate", StringValue ("50Mbps")); //bit/s
  	ApplicationContainer onoffclient_wl1;
  	onoffclient_wl1.Add (onoff_wl1.Install (wlesslink.Get(0)));
  	onoffclient_wl1.Start (Seconds (0));
  	onoffclient_wl1.Stop (Seconds (10));

  	//onoff aplication no 2 for wireless

  	OnOffHelper onoff_wl2 ("ns3::TcpSocketFactory", r_addr);
  	onoff_wl2.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  	onoff_wl2.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  	onoff_wl2.SetAttribute ("PacketSize", UintegerValue (packetsize));
  	onoff_wl2.SetAttribute ("DataRate", StringValue ("50Mbps")); //bit/s
  	ApplicationContainer onoffclient_wl2;
  	onoffclient_wl2.Add (onoff_wl2.Install (wlesslink.Get(0)));
  	onoffclient_wl2.Start (Seconds (0));
  	onoffclient_wl2.Stop (Seconds (10));

  	//onoff aplication no 3 for wireless

  	OnOffHelper onoff_wl3 ("ns3::TcpSocketFactory", r_addr);
  	onoff_wl3.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  	onoff_wl3.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  	onoff_wl3.SetAttribute ("PacketSize", UintegerValue (packetsize));
  	onoff_wl3.SetAttribute ("DataRate", StringValue ("50Mbps")); //bit/s
  	ApplicationContainer onoffclient_wl3;
  	onoffclient_wl3.Add (onoff_wl3.Install (wlesslink.Get(0)));
  	onoffclient_wl3.Start (Seconds (0));
  	onoffclient_wl3.Stop (Seconds (10));

  	// routing is done using default function
	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
	

	// Flow monitor is used to calculate Throughput and fairness index 
	FlowMonitorHelper flowmon;
  	Ptr<FlowMonitor> monitor = flowmon.InstallAll();

	Simulator::Stop (Seconds (4));
	Simulator::Run ();


	Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
    std::cout << "Flow information for Packet size value : \t" << packetsize << "\n" << std::endl;
	double Sumx_w = 0, SumSqx_w = 0, Sumx_wl = 0, SumSqx_wl = 0;
	  int n = 0;
	  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
	    {
		  Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
	      
		  std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " : " << t.sourcePort << " -> " << t.destinationAddress << " : " << t.destinationPort << ") - ";
		  std::cout << "  Tx Bytes:   " << i->second.txBytes ;
		  std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
		  std::cout << "Throughput: \t" << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps\n\n";
		  
		  double TPut = i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024;
	      if (t.destinationPort == 6666)
	      {
	      	Sumx_w += TPut;
			SumSqx_w += TPut * TPut ;
		  }
		  else if(t.destinationPort == 7666)
		  {
		  	Sumx_wl += TPut;
			SumSqx_wl += TPut * TPut ;
		  }
		  n++;
	     }
	double FairnessIndex_w = (Sumx_w* Sumx_w)/ (4 * SumSqx_w) ;
	double FairnessIndex_wl = (Sumx_wl* Sumx_wl)/ (3 * SumSqx_wl) ;

	// data is added here to dataset which will be shown in graph
	dataset3.Add (packetsize, FairnessIndex_w*10);
	dataset4.Add (packetsize, FairnessIndex_wl*10);
	dataset1.Add (packetsize, (Sumx_w/4)*10);
	dataset2.Add (packetsize, (Sumx_wl/3)*10);
	dataset5.Add (packetsize, (Sumx_w/4)*10, FairnessIndex_w*10);
	dataset6.Add (packetsize, (Sumx_wl/3)*10, FairnessIndex_wl*10);
	
	dataset5.AddEmptyLine ();
	dataset6.AddEmptyLine ();

	std :: cout << " FairnessIndex_wired: \t" << FairnessIndex_w << std :: endl;
	std :: cout << " FairnessIndex_wireless: \t" << FairnessIndex_wl << "\n ------------------------------------------------------------------------------------------ \n";


  monitor->SerializeToXmlFile("lab-1.xml", true, true);

  Simulator::Destroy ();
  switch(packetsize)
  {
  	case 40:
  		packetsize = 44;
  		break;

  	case 44:
  		packetsize = 48;
  		break;

  	case 48:
  		packetsize = 52;
  		break;

  	case 52:
  		packetsize = 60;
  		break;

  	case 60:
  		packetsize = 552;
  		break;

  	case 552:
  		packetsize = 576;
  		break;

  	case 576:
  		packetsize = 628;
  		break;

  	case 628:
  		packetsize = 1420;
  		break;

  	case 1420:
  		packetsize = 1500;
  		break;
  	default:
  		packetsize = 1600;
  }

}

	

	//Add the dataset to the plot.
	plot1.AddDataset (dataset1);
	plot2.AddDataset (dataset2);
	plot3.AddDataset (dataset3);
	plot4.AddDataset (dataset4);
	plot5.AddDataset (dataset5);
	plot6.AddDataset (dataset6);

	// Open the plot file.
	std :: ofstream plotFile1 (PFile1.c_str());
	std :: ofstream plotFile2 (PFile2.c_str());
	std :: ofstream plotFile3 (PFile3.c_str());
	std :: ofstream plotFile4 (PFile4.c_str());
	std :: ofstream plotFile5 (PFile5.c_str());
	std :: ofstream plotFile6 (PFile6.c_str());

	// Write the plot file.
	plot1.GenerateOutput (plotFile1);
	plot2.GenerateOutput (plotFile2);
	plot3.GenerateOutput (plotFile3);
	plot4.GenerateOutput (plotFile4);
	plot5.GenerateOutput (plotFile5);
	plot6.GenerateOutput (plotFile6);

	// Close the plot file.
	plotFile1.close ();
	plotFile2.close ();
	plotFile3.close ();
	plotFile4.close ();
	plotFile5.close ();
	plotFile6.close ();

}
