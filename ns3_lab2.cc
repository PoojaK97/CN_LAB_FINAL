/* Simulate a four node point-to-point network, and connect the links as follows: n0-n2, n1-n2 and n2-n3. Apply TCP agent between n0-n3 and UDP agent between n1-n3. Apply relevant applications over TCP and UDP agents by changing the parameters and determine the number of packets sent by TCP/UDP */



#include "ns3/core-module.h"

#include "ns3/network-module.h"

#include "ns3/internet-module.h"

#include "ns3/point-to-point-module.h"

#include "ns3/applications-module.h"

#include "ns3/traffic-control-module.h"

#include "ns3/flow-monitor-module.h"



// Network topology

//

//       10.1.1.0

// n0 -------------- n2

//    point-to-point      10.1.3.0

//	   TCP

//                      -------------- n3 

//	10.1.2.0

// n1 -------------- n2

//    point-to-point

//	   UDP



using namespace ns3;



/*NS_LOG_COMPONENT_DEFINE ("TrafficControlExample");



void

TcPacketsInQueueTrace (uint32_t oldValue, uint32_t newValue)

{

  std::cout << "TcPacketsInQueue " << oldValue << " to " << newValue << std::endl;

}



void

DevicePacketsInQueueTrace (uint32_t oldValue, uint32_t newValue)

{

  std::cout << "DevicePacketsInQueue " << oldValue << " to " << newValue << std::endl;

}



void

SojournTimeTrace (Time sojournTime)

{

  std::cout << "Sojourn time " << sojournTime.ToDouble (Time::MS) << "ms" << std::endl;

}*/



int

main ()

{

  double simulationTime = 10; //seconds

  std::string transportProt = "Udp"; //UDP

  std::string socketType;



  /*CommandLine cmd;

  cmd.AddValue ("transportProt", "Transport protocol to use: Tcp, Udp", transportProt);

  cmd.Parse (argc, argv);*/



  if (transportProt.compare ("Tcp") == 0)

    {

      socketType = "ns3::TcpSocketFactory";

    }

  else

    {

      socketType = "ns3::UdpSocketFactory";

    }



  NodeContainer nodes;

  nodes.Create (4); //4 Nodes



  PointToPointHelper pointToPoint;

  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10Mbps"));

  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  pointToPoint.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("1p"));



//3 Devices

  NetDeviceContainer dev02 = pointToPoint.Install (nodes.Get(0), nodes.Get(2)); //0 and 2

  NetDeviceContainer dev12 = pointToPoint.Install (nodes.Get(1), nodes.Get(2)); //1 and 2

  NetDeviceContainer dev23 = pointToPoint.Install (nodes.Get(2), nodes.Get(3)); //2 and 3



  InternetStackHelper stack;

  stack.Install (nodes);



  /*TrafficControlHelper tch;

  tch.SetRootQueueDisc ("ns3::RedQueueDisc");

  QueueDiscContainer qdiscs = tch.Install (devices);



  Ptr<QueueDisc> q = qdiscs.Get (1);

  q->TraceConnectWithoutContext ("PacketsInQueue", MakeCallback (&TcPacketsInQueueTrace));

  Config::ConnectWithoutContext ("/NodeList/1/$ns3::TrafficControlLayer/RootQueueDiscList/0/SojournTime",

                                 MakeCallback (&SojournTimeTrace));



  Ptr<NetDevice> nd = devices.Get (1);

  Ptr<PointToPointNetDevice> ptpnd = DynamicCast<PointToPointNetDevice> (nd);

  Ptr<Queue<Packet> > queue = ptpnd->GetQueue ();

  queue->TraceConnectWithoutContext ("PacketsInQueue", MakeCallback (&DevicePacketsInQueueTrace));*/



//________3 Interfaces, assign addreses

  Ipv4AddressHelper address1;

  address1.SetBase ("10.1.1.0", "255.255.255.0"); //10.1.1.0

  Ipv4InterfaceContainer interfaces1 = address1.Assign (dev02);

  Ipv4AddressHelper address2;

  address2.SetBase ("10.1.2.0", "255.255.255.0"); //10.1.2.0

  Ipv4InterfaceContainer interfaces2 = address2.Assign (dev12);

  Ipv4AddressHelper address3;

  address3.SetBase ("10.1.3.0", "255.255.255.0"); //10.1.3.0

  Ipv4InterfaceContainer interfaces3 = address3.Assign (dev23);



//_________Global Routing Helper

  Ipv4GlobalRoutingHelper::PopulateRoutingTables();





//Two Flows, UDP and TCP

  //Udp Flow

  uint16_t port = 7;

  Address localAddress (InetSocketAddress (Ipv4Address::GetAny (), port));

  PacketSinkHelper packetSinkHelper (socketType, localAddress);

  ApplicationContainer sinkApp = packetSinkHelper.Install (nodes.Get (3)); //Sink: 3



  sinkApp.Start (Seconds (0.0));

  sinkApp.Stop (Seconds (simulationTime + 0.1));



  uint32_t payloadSize = 1448;

  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));



  OnOffHelper onoff (socketType, Ipv4Address::GetAny ());

  onoff.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));

  onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  onoff.SetAttribute ("PacketSize", UintegerValue (payloadSize));

  onoff.SetAttribute ("DataRate", StringValue ("50Mbps")); //bit/s

  ApplicationContainer apps;



  //InetSocketAddress rmt (interfaces3.GetAddress (1), port);

  //rmt.SetTos (0xb8);

  AddressValue remoteAddress (InetSocketAddress (interfaces3.GetAddress (1), port)); //10.1.3.1

  onoff.SetAttribute ("Remote", remoteAddress);

  apps.Add (onoff.Install (nodes.Get (0))); //Node 0

  apps.Start (Seconds (1.0));

  apps.Stop (Seconds (simulationTime + 0.1));



  //Tcp Flow

  uint16_t port_tcp = 9; //Change port number

  socketType = "ns3::TcpSocketFactory"; //Change socket type

	//Change variable names everywhere (BE VERY CAREFUL)

  Address localAddress_tcp (InetSocketAddress (Ipv4Address::GetAny (), port_tcp));

  PacketSinkHelper packetSinkHelper_tcp (socketType, localAddress_tcp);

  ApplicationContainer sinkApp_tcp = packetSinkHelper_tcp.Install (nodes.Get (3)); //Sink: 3



  sinkApp_tcp.Start (Seconds (0.5)); //+0.5

  sinkApp_tcp.Stop (Seconds (simulationTime + 0.1));



  //uint32_t payloadSize = 1448;

  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));



  OnOffHelper onoff_tcp (socketType, Ipv4Address::GetAny ());

  onoff_tcp.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));

  onoff_tcp.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  onoff_tcp.SetAttribute ("PacketSize", UintegerValue (payloadSize));

  onoff_tcp.SetAttribute ("DataRate", StringValue ("50Mbps")); //bit/s

  ApplicationContainer apps_tcp;



  //InetSocketAddress rmt_tcp (interfaces3.GetAddress (1), port_tcp);

  //rmt.SetTos (0xb8);

  AddressValue remoteAddress_tcp (InetSocketAddress (interfaces3.GetAddress (1), port_tcp)); //10.1.3.1

  onoff_tcp.SetAttribute ("Remote", remoteAddress_tcp);

  apps_tcp.Add (onoff_tcp.Install (nodes.Get (1))); //Node 1

  apps_tcp.Start (Seconds (1.5));//+0.5

  apps_tcp.Stop (Seconds (simulationTime + 0.1));





  FlowMonitorHelper flowmon;

  Ptr<FlowMonitor> monitor = flowmon.InstallAll();



  Simulator::Stop (Seconds (simulationTime + 5));

  Simulator::Run ();



  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());

  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();

  std::cout << std::endl << "*** Flow monitor statistics ***" << std::endl;



  /*std::cout << "  Tx Packets/Bytes:   " << stats[1].txPackets

            << " / " << stats[1].txBytes << std::endl;

  std::cout << "  Offered Load: " << stats[1].txBytes * 8.0 / (stats[1].timeLastTxPacket.GetSeconds () - stats[1].timeFirstTxPacket.GetSeconds ()) / 1000000 << " Mbps" << std::endl;

  std::cout << "  Rx Packets/Bytes:   " << stats[1].rxPackets

            << " / " << stats[1].rxBytes << std::endl;

  uint32_t packetsDroppedByQueueDisc = 0;

  uint64_t bytesDroppedByQueueDisc = 0;

  if (stats[1].packetsDropped.size () > Ipv4FlowProbe::DROP_QUEUE_DISC)

    {

      packetsDroppedByQueueDisc = stats[1].packetsDropped[Ipv4FlowProbe::DROP_QUEUE_DISC];

      bytesDroppedByQueueDisc = stats[1].bytesDropped[Ipv4FlowProbe::DROP_QUEUE_DISC];

    }

  std::cout << "  Packets/Bytes Dropped by Queue Disc:   " << packetsDroppedByQueueDisc

            << " / " << bytesDroppedByQueueDisc << std::endl;

  uint32_t packetsDroppedByNetDevice = 0;

  uint64_t bytesDroppedByNetDevice = 0;

  if (stats[1].packetsDropped.size () > Ipv4FlowProbe::DROP_QUEUE)

    {

      packetsDroppedByNetDevice = stats[1].packetsDropped[Ipv4FlowProbe::DROP_QUEUE];

      bytesDroppedByNetDevice = stats[1].bytesDropped[Ipv4FlowProbe::DROP_QUEUE];

    }

  std::cout << "  Packets/Bytes Dropped by NetDevice:   " << packetsDroppedByNetDevice

            << " / " << bytesDroppedByNetDevice << std::endl;

  std::cout << "  Throughput: " << stats[1].rxBytes * 8.0 / (stats[1].timeLastRxPacket.GetSeconds () - stats[1].timeFirstRxPacket.GetSeconds ()) / 1000000 << " Mbps" << std::endl;

  std::cout << "  Mean delay:   " << stats[1].delaySum.GetSeconds () / stats[1].rxPackets << std::endl;

  std::cout << "  Mean jitter:   " << stats[1].jitterSum.GetSeconds () / (stats[1].rxPackets - 1) << std::endl;

  auto dscpVec = classifier->GetDscpCounts (1);

  for (auto p : dscpVec)

    {

      std::cout << "  DSCP value:   0x" << std::hex << static_cast<uint32_t> (p.first) << std::dec

                << "  count:   "<< p.second << std::endl;

    }*/





//________ADD THIS

  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin(); iter!=stats.end(); ++iter) {

        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(iter->first);

        NS_LOG_UNCOND("Flow ID: "<<iter->first<<" Src Address: "<<t.sourceAddress<<" Dest. Address: "<<t.destinationAddress<<std::endl);

        NS_LOG_UNCOND("Tx Packets: "<<iter->second.txPackets<<std::endl);

  }



  Simulator::Destroy ();



  /*std::cout << std::endl << "*** Application statistics ***" << std::endl;

  double thr = 0;

  uint64_t totalPacketsThr = DynamicCast<PacketSink> (sinkApp.Get (0))->GetTotalRx ();

  thr = totalPacketsThr * 8 / (simulationTime * 1000000.0); //Mbit/s

  std::cout << "  Rx Bytes: " << totalPacketsThr << std::endl;

  std::cout << "  Average Goodput: " << thr << " Mbit/s" << std::endl;

  std::cout << std::endl << "*** TC Layer statistics ***" << std::endl;

  std::cout << q->GetStats () << std::endl;*/

  return 0;

}