//Simulate a wireless network, generate traffic and analyze its performance.



//Headers from lab3 or third.cc

#include "ns3/core-module.h"

#include "ns3/point-to-point-module.h"

#include "ns3/network-module.h"

#include "ns3/applications-module.h"

#include "ns3/mobility-module.h"

#include "ns3/csma-module.h"

#include "ns3/internet-module.h"

#include "ns3/yans-wifi-helper.h"

#include "ns3/ssid.h"

//flow-monitor-module, netanim-module

#include "ns3/flow-monitor-module.h"

//#include "ns3/netanim-module.h"





using namespace ns3;



/*NS_LOG_COMPONENT_DEFINE ("WifiSimpleAdhoc");



void ReceivePacket (Ptr<Socket> socket)

{

  while (socket->Recv ())

    {

      NS_LOG_UNCOND ("Received one packet!");

    }

}



static void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize,

                             uint32_t pktCount, Time pktInterval )

{

  if (pktCount > 0)

    {

      socket->Send (Create<Packet> (pktSize));

      Simulator::Schedule (pktInterval, &GenerateTraffic,

                           socket, pktSize,pktCount - 1, pktInterval);

    }

  else

    {

      socket->Close ();

    }

}*/



int main (int argc, char *argv[])

{

  std::string phyMode ("DsssRate1Mbps");

  double rss = -80;  // -dBm

  /*uint32_t packetSize = 1000; // bytes

  uint32_t numPackets = 1;

  double interval = 1.0; // seconds

  bool verbose = false;



  CommandLine cmd;

  cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);

  cmd.AddValue ("rss", "received signal strength", rss);

  cmd.AddValue ("packetSize", "size of application packet sent", packetSize);

  cmd.AddValue ("numPackets", "number of packets generated", numPackets);

  cmd.AddValue ("interval", "interval (seconds) between packets", interval);

  cmd.AddValue ("verbose", "turn on all WifiNetDevice log components", verbose);

  cmd.Parse (argc, argv);

  // Convert to time object

  Time interPacketInterval = Seconds (interval);



  // Fix non-unicast data rate to be the same as that of unicast*/

 

//ADD 2 MORE: FragmentationThreshold and RtsCtsThreshold

  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));

  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200"));

  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode",StringValue (phyMode));



  NodeContainer c;

  c.Create (5); //5 Nodes



  // The below set of helpers will help us to put together the wifi NICs we want

  WifiHelper wifi;

  /*if (verbose)

    {

      wifi.EnableLogComponents ();  // Turn on all Wifi logging

    }*/

  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);



  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();

  // This is one parameter that matters when using FixedRssLossModel

  // set it to zero; otherwise, gain will be added

  wifiPhy.Set ("RxGain", DoubleValue (0) );

  // ns-3 supports RadioTap and Prism tracing extensions for 802.11b

  wifiPhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11_RADIO);



  YansWifiChannelHelper wifiChannel;

  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");

  // The below FixedRssLossModel will cause the rss to be fixed regardless

  // of the distance between the two stations, and the transmit power

  wifiChannel.AddPropagationLoss ("ns3::FixedRssLossModel","Rss",DoubleValue (rss));

  wifiPhy.SetChannel (wifiChannel.Create ());



  // Add a mac and disable rate control

  WifiMacHelper wifiMac;

  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",

                                "DataMode",StringValue (phyMode),

                                "ControlMode",StringValue (phyMode));

  // Set it to adhoc mode

  wifiMac.SetType ("ns3::AdhocWifiMac");

  NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, c);



  // Note that with FixedRssLossModel, the positions below are not

  // used for received signal strength.

  MobilityHelper mobility;

  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();

  positionAlloc->Add (Vector (1.0, 1.0, 0.0));

  positionAlloc->Add (Vector (2.0, 1.0, 0.0));

//ADD 3 MORE AND CHANGE ORDER

  positionAlloc->Add (Vector (3.0, 2.0, 0.0));

  positionAlloc->Add (Vector (4.0, 3.0, 0.0));

  positionAlloc->Add (Vector (5.0, 2.0, 0.0));

//______________

  mobility.SetPositionAllocator (positionAlloc);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  mobility.Install (c);



  InternetStackHelper internet;

  internet.Install (c);



  Ipv4AddressHelper ipv4;

  //NS_LOG_INFO ("Assign IP Addresses.");

  ipv4.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer i = ipv4.Assign (devices);



  /*TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");

  Ptr<Socket> recvSink = Socket::CreateSocket (c.Get (0), tid);

  InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), 80);

  recvSink->Bind (local);

  recvSink->SetRecvCallback (MakeCallback (&ReceivePacket));



  Ptr<Socket> source = Socket::CreateSocket (c.Get (1), tid);

  InetSocketAddress remote = InetSocketAddress (Ipv4Address ("255.255.255.255"), 80);

  source->SetAllowBroadcast (true);

  source->Connect (remote);



  // Tracing

  wifiPhy.EnablePcap ("wifi-simple-adhoc", devices);



  // Output what we are doing

  NS_LOG_UNCOND ("Testing " << numPackets  << " packets sent with receiver rss " << rss );



  Simulator::ScheduleWithContext (source->GetNode ()->GetId (),

                                  Seconds (1.0), &GenerateTraffic,

                                  source, packetSize, numPackets, interPacketInterval);*/

  //Add

Ipv4GlobalRoutingHelper::PopulateRoutingTables ();



//UDP FLOW_____________________________________________________________

  uint16_t port = 7;

  Address localAddress (InetSocketAddress (Ipv4Address::GetAny (), port));

  PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory", localAddress);

  ApplicationContainer sinkApp = packetSinkHelper.Install (c.Get (4)); //Sink: 4



  sinkApp.Start (Seconds (0.0));

  sinkApp.Stop (Seconds (10 + 0.1));



  uint32_t payloadSize = 1448;

  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));



  OnOffHelper onoff ("ns3::UdpSocketFactory", Ipv4Address::GetAny ());

  onoff.SetAttribute ("OnTime",  StringValue ("ns3::ConstantRandomVariable[Constant=1]"));

  onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  onoff.SetAttribute ("PacketSize", UintegerValue (payloadSize));

  onoff.SetAttribute ("DataRate", StringValue ("50Mbps")); //bit/s

  ApplicationContainer apps;



  AddressValue remoteAddress (InetSocketAddress (i.GetAddress (3), port)); //CHANGE

 //AddressValue remoteAddress (InetSocketAddress (Ipv4Address ("255.255.255.255"), port));

  onoff.SetAttribute ("Remote", remoteAddress);

  apps.Add (onoff.Install (c.Get (0))); //CHANGE

  apps.Start (Seconds (1.0));

  apps.Stop (Seconds (10 + 0.1));





 //Enable Tracing using flowmonitor

  FlowMonitorHelper flowmon;

  Ptr<FlowMonitor> monitor = flowmon.InstallAll();



  Simulator::Stop (Seconds (10.0));



//ADD THIS _______________________________________Add visualization using Netanim

  //AnimationInterface anim ("ex4.xml"); 





  Simulator::Run ();



// Print per flow statistics

  monitor->CheckForLostPackets ();

  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());

  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();



  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin (); iter != stats.end (); ++iter)

    {

  Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (iter->first);

      NS_LOG_UNCOND("Flow ID: " << iter->first << " Src Addr " << t.sourceAddress << " Dst Addr " << t.destinationAddress);

      NS_LOG_UNCOND("Tx Packets = " << iter->second.txPackets);

      NS_LOG_UNCOND("Rx Packets = " << iter->second.rxPackets);

      NS_LOG_UNCOND("Throughput: " << iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds()) / 1024  << " Kbps");

    }



  Simulator::Destroy ();



  return 0;

}