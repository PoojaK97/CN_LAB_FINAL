/* Problem statement: Three nodes point – to – point network with duplex links between them. Set the queue size, vary the bandwidth and find the number of packets dropped.*/



#include "ns3/core-module.h"

#include "ns3/network-module.h"

#include "ns3/internet-module.h"

#include "ns3/point-to-point-module.h"

#include "ns3/applications-module.h"

#include "ns3/traffic-control-module.h"

#include "ns3/flow-monitor-module.h"





// Network topology

//

//       10.1.1.0           10.1.2.0

// n0 -------------- n1 ---------------- n2 

//    point-to-point     point-to-point





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

  std::string transportProt = "Udp";

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

  nodes.Create (3); //3 Nodes



  PointToPointHelper pointToPoint;

  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("10Mbps")); //Change bandwidth here for demo

  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  pointToPoint.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("1p"));

 

//2 DEVICES

  NetDeviceContainer dev01;

  dev01 = pointToPoint.Install (nodes.Get(0),nodes.Get(1)); //between nodes 0 and 1

  NetDeviceContainer dev12;

  dev12 = pointToPoint.Install (nodes.Get(1),nodes.Get(2)); //between nodes 1 and 2



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



//_________Address assignment for 2 interfaces



  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0"); //10.1.1.0



  Ipv4InterfaceContainer interfaces0 = address.Assign (dev01);



  address.SetBase ("10.1.2.0", "255.255.255.0"); //10.1.2.0



  Ipv4InterfaceContainer interfaces1 = address.Assign (dev12);



//__________Global Routing Helper (TO BE ADDED)

  Ipv4GlobalRoutingHelper::PopulateRoutingTables (); 



  //Flow

  uint16_t port = 7;

  Address localAddress (InetSocketAddress (Ipv4Address::GetAny (), port));

  PacketSinkHelper packetSinkHelper (socketType, localAddress);

  ApplicationContainer sinkApp = packetSinkHelper.Install (nodes.Get (2)); //Sink: node 2



  sinkApp.Start (Seconds (0.0));

  sinkApp.Stop (Seconds (simulationTime + 0.1));



  uint32_t payloadSize = 1448;

  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));



  OnOffHelper onoff (socketType, Ipv4Address::GetAny ());

  onoff.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));

  onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  //onoff.SetAttribute ("PacketSize", UintegerValue (payloadSize));

  onoff.SetAttribute ("DataRate", StringValue ("50Mbps")); //bit/s

  ApplicationContainer apps;



  InetSocketAddress rmt (interfaces1.GetAddress (1), port); //Change to 1

  //rmt.SetTos (0xb8);

  AddressValue remoteAddress (rmt);

  onoff.SetAttribute ("Remote", remoteAddress);

  apps.Add (onoff.Install (nodes.Get (0))); //Change to 0

  apps.Start (Seconds (1.0));

  apps.Stop (Seconds (simulationTime + 0.1));



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

    }

*/



//__________ADD THIS FOR LOOP

  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin(); iter!= stats.end(); ++iter) {

        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);

        std::cout<<"Flow ID: "<<iter->first<<" Source Address: "<<t.sourceAddress<<" Destination Address: "<<t.destinationAddress<<std::endl;

        std::cout<<"Tx Packets: "<<iter->second.txPackets<<std::endl;

        std::cout<<"Rx Packets: "<<iter->second.rxPackets<<std::endl;

        std::cout<<"Lost Packets: "<<iter->second.lostPackets<<std::endl;

        std::cout<<"Throughput: "<<iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds() - iter->second.timeFirstRxPacket.GetSeconds()) / 1000 <<"Kbps" <<std::endl;

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