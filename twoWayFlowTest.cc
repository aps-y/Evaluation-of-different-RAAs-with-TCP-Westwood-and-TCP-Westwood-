/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/flow-monitor.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"
#include <fstream>
#include <iostream>

// 
//
//    Wifi 10.1.2.0
//                   AP1
//  *     *     *     *
//  |     |     |     |     10.1.1.0    AP2
// n3L   n2L   n1L   n0L -------------- n0R   n1R   n2R   n3R
//                       point-to-point  |     |     |     |
//                                       *     *     *     *
//                                         Wifi 10.1.3.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");

// This structure stores global variables, which are needed to calculate throughput and delay every second
struct DataForThpt
{
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor;
  uint64_t
      lastTotalRxBytes; //Total bytes received in all flows before the starting of current window.
  double lastDelaySum; //Total Delay sum in all flows before the starting of current window.
  uint32_t
      lastRxPackets; //Total number of received packets in all flows before starting the current window.
} data; //data is a structure variable which will store all these global variables.

double averageDelay;
std::ofstream delayStream;
std::ofstream throughputStream;

//This function is being called every 0.2 seconds, It measures delay and throughput in every 0.2s time window.
//It calculates overall throughput in that window of all flows in the network.
static void
Throughput ()
{

  data.monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier =
      DynamicCast<Ipv4FlowClassifier> (data.flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = data.monitor->GetFlowStats ();

  uint64_t total_bytes = 0;
  uint32_t total_RxPackets = 0;
  double total_DelaySum = 0;

  //Iterating through every flow
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin ();
       iter != stats.end (); ++iter)
    {

      total_bytes += iter->second.rxBytes;
      total_DelaySum += iter->second.delaySum.GetDouble ();
      total_RxPackets += iter->second.rxPackets;
    }
  uint64_t dataInLastSecond = total_bytes - data.lastTotalRxBytes;
  uint32_t RxPacketsInLastSecond = total_RxPackets - data.lastRxPackets;
  double DelaySumInLastSecond = total_DelaySum - data.lastDelaySum;

  data.lastTotalRxBytes = total_bytes;
  data.lastDelaySum = total_DelaySum;
  data.lastRxPackets = total_RxPackets;

  if (total_RxPackets != 0)
    {
      averageDelay = (total_DelaySum / 1000000) / total_RxPackets;
    }

  if (!(RxPacketsInLastSecond == 0 || DelaySumInLastSecond == 0))
    {
      NS_LOG_UNCOND ("Delay " << (DelaySumInLastSecond / RxPacketsInLastSecond) / (1000000)
                              << "ms");
      delayStream << (DelaySumInLastSecond / RxPacketsInLastSecond) / (1000000) << std::endl;
    }
  else
    {
      NS_LOG_UNCOND ("Delay " << 0 << "ms");
      delayStream << 0 << std::endl;
    }
  NS_LOG_UNCOND ("Throughput " << (dataInLastSecond * 8.0) / (1024 * 1024) << "Mbps");
  throughputStream << (dataInLastSecond * 8.0 ) / (1024 * 1024) << std::endl;

  Simulator::Schedule (Seconds (1.0), &Throughput);
}

int
main (int argc, char *argv[])
{
  bool verbose = true;
  bool logAverages = false;

  //Number of STA(Stations)
  int nWifi = 4;

  bool tracing = false;

  // maxBytes =0 means unlimited bytes
  uint32_t maxBytes = 0;

  // TCP protocol
  std::string transport_prot = "TcpWestwood";

  //Error Model -> Default = NistErrorRateModel
  std::string errorModelType = "ns3::NistErrorRateModel";

  //RAA algorithm (WifiManager Class) -> Default = MinstrelHt
  std::string raaAlgo = "Onoe";

  //Variables to set rates of various channels in topology, Refer base topology structure.
  // uint32_t csmaRate = 150;
  // uint32_t csmaDelay = 9000;
  uint32_t p2pRate = 15;
  uint32_t p2pDelay = 5;
  int rngRun =1;
  //Command-Line argument to make it interactive.
  CommandLine cmd (__FILE__);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("logAverages", "write averages into a file if true",
                logAverages); //Use this only along with runExperiments.sh
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);
  cmd.AddValue ("raa", "Arf/ Aarf/ Aarfcd/ Onoe/ Minstrel", raaAlgo);
  cmd.AddValue ("tcp", "TcpWestwood / TcpWestwoodPlus", transport_prot);
  cmd.AddValue ("maxBytes", "Max number of Bytes to be sent", maxBytes);
  cmd.AddValue ("p2pRate", "Mbps", p2pRate);
  cmd.AddValue ("p2pDelay", "MilliSeconds", p2pDelay);
  cmd.AddValue("rngRun","int",rngRun);

  cmd.Parse (argc, argv);

  std::string raa_name = raaAlgo;
  raaAlgo = "ns3::" + raaAlgo + "WifiManager";

  //Store values of Throughput and delay in respective files for plotting graph
  delayStream.open ("Delay_" + raa_name + "_" + transport_prot + "_" + std::to_string (nWifi)+"_" + std::to_string(rngRun)+
                    ".csv");
  throughputStream.open ("Throughput_" + raa_name + "_" + transport_prot + "_" +
                         std::to_string (nWifi)+"_" + std::to_string(rngRun) + ".csv");

  std::string tcp_name = transport_prot;
  transport_prot = std::string ("ns3::") + transport_prot;
  // The underlying restriction of 18 is due to the grid position
  // allocator's configuration; the grid layout will exceed the
  // bounding box if more than 18 nodes are provided.
  if (nWifi > 18)
    {
      std::cout << "nWifi should be 18 or less; otherwise grid layout exceeds the bounding box"
                << std::endl;
      return 1;
    }

  if (transport_prot.compare ("ns3::TcpWestwoodPlus") == 0)
    {
      // TcpWestwoodPlus is not an actual TypeId name; we need TcpWestwood here
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType",
                          TypeIdValue (TcpWestwood::GetTypeId ()));
      // the default protocol type in ns3::TcpWestwood is WESTWOOD
      Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
    }
  else
    {
      TypeId tcpTid;
      NS_ABORT_MSG_UNLESS (TypeId::LookupByNameFailSafe (transport_prot, &tcpTid),
                           "TypeId " << transport_prot << " not found");
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType",
                          TypeIdValue (TypeId::LookupByName (transport_prot)));
    }

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue (std::to_string (p2pRate) + "Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue (std::to_string (p2pDelay) + "ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  Ptr<RateErrorModel> errModelLeft = CreateObject<RateErrorModel> ();
  errModelLeft->SetAttribute ("ErrorRate", DoubleValue (0.00005));
  p2pDevices.Get (0)->SetAttribute ("ReceiveErrorModel", PointerValue (errModelLeft));

  Ptr<RateErrorModel> errModelRight = CreateObject<RateErrorModel> ();
  errModelRight->SetAttribute ("ErrorRate", DoubleValue (0.00005));
  p2pDevices.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (errModelRight));
  

  // ----------------- Left Half -------------- //
  std::cout << "creating left half" << std::endl;
  NodeContainer wifiStaNodesLeft;
  wifiStaNodesLeft.Create (nWifi);
  NodeContainer wifiApNodeLeft = p2pNodes.Get (0);

  YansWifiChannelHelper channelLeft = YansWifiChannelHelper::Default ();

  //Delay model
  channelLeft.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");

  //Loss model
  channelLeft.AddPropagationLoss ("ns3::LogDistancePropagationLossModel", "Exponent",
                                  DoubleValue (0.3), "ReferenceLoss", DoubleValue (4.0));

  YansWifiPhyHelper phyLeft = YansWifiPhyHelper ();
  phyLeft.SetChannel (channelLeft.Create ());

  //Error Model
  phyLeft.SetErrorRateModel (errorModelType);

  WifiHelper wifiLeft;

  //Setting Wifi Standard
  wifiLeft.SetStandard (WIFI_STANDARD_80211g);

  //Setting Raa Algorithm
  wifiLeft.SetRemoteStationManager (raaAlgo);

  WifiMacHelper macLeft;
  Ssid ssidLeft = Ssid ("ns-3-ssid");
  macLeft.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssidLeft), "ActiveProbing",
                   BooleanValue (false));

  NetDeviceContainer staDevicesLeft;
  staDevicesLeft = wifiLeft.Install (phyLeft, macLeft, wifiStaNodesLeft);

  macLeft.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (ssidLeft));

  NetDeviceContainer apDevicesLeft;
  apDevicesLeft = wifiLeft.Install (phyLeft, macLeft, wifiApNodeLeft);

  MobilityHelper mobilityLeft;

  mobilityLeft.SetPositionAllocator ("ns3::GridPositionAllocator", "MinX", DoubleValue (0.0),
                                     "MinY", DoubleValue (0.0), "DeltaX", DoubleValue (5.0),
                                     "DeltaY", DoubleValue (10.0), "GridWidth", UintegerValue (3),
                                     "LayoutType", StringValue ("RowFirst"));

  //Bounds for the Rectangle Grid
  mobilityLeft.SetMobilityModel ("ns3::RandomWalk2dMobilityModel", "Speed",
                                 StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                                 "Bounds", RectangleValue (Rectangle (-100, 100, -100, 100)));
  mobilityLeft.Install (wifiStaNodesLeft);

  //Setting Mobility model
  mobilityLeft.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityLeft.Install (wifiApNodeLeft);

  std::cout << "created left half" << std::endl;

  // --------------- Right Half ------------------ //

  std::cout << "creating right half" << std::endl;

  NodeContainer wifiStaNodesRight;
  wifiStaNodesRight.Create (nWifi);
  NodeContainer wifiApNodeRight = p2pNodes.Get (1);

  YansWifiChannelHelper channelRight = YansWifiChannelHelper::Default ();

  //Delay model
  channelRight.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");

  //Loss model
  channelRight.AddPropagationLoss ("ns3::LogDistancePropagationLossModel", "Exponent",
                                   DoubleValue (0.3), "ReferenceLoss", DoubleValue (4.0));

  YansWifiPhyHelper phyRight = YansWifiPhyHelper ();
  phyRight.SetChannel (channelRight.Create ());

  //Error Model
  phyRight.SetErrorRateModel (errorModelType);

  WifiHelper wifiRight;

  //Setting Wifi Standard
  wifiRight.SetStandard (WIFI_STANDARD_80211g);

  //Setting Raa Algorithm
  wifiRight.SetRemoteStationManager (raaAlgo);

  WifiMacHelper macRight;
  Ssid ssidRight = Ssid ("ns-3-ssid");
  macRight.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssidRight), "ActiveProbing",
                    BooleanValue (false));

  NetDeviceContainer staDevicesRight;
  staDevicesRight = wifiRight.Install (phyRight, macRight, wifiStaNodesRight);

  macRight.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (ssidRight));

  NetDeviceContainer apDevicesRight;
  apDevicesRight = wifiRight.Install (phyRight, macRight, wifiApNodeRight);

  MobilityHelper mobilityRight;

  mobilityRight.SetPositionAllocator ("ns3::GridPositionAllocator", "MinX", DoubleValue (30.0),
                                      "MinY", DoubleValue (0.0), "DeltaX", DoubleValue (5.0),
                                      "DeltaY", DoubleValue (10.0), "GridWidth", UintegerValue (3),
                                      "LayoutType", StringValue ("RowFirst"));

  //Bounds for the Rectangle Grid
  mobilityRight.SetMobilityModel ("ns3::RandomWalk2dMobilityModel", "Speed",
                                  StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"),
                                  "Bounds", RectangleValue (Rectangle (-100, 100, -100, 100)));
  mobilityRight.Install (wifiStaNodesRight);

  //Setting Mobility model
  mobilityRight.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobilityRight.Install (wifiApNodeRight);

  std::cout << "created right half" << std::endl;

  InternetStackHelper stack;
  stack.Install (wifiApNodeLeft);
  stack.Install (wifiStaNodesLeft);
  stack.Install (wifiApNodeRight);

  Config::SetDefault ("ns3::TcpL4Protocol::SocketType",
                          TypeIdValue (TcpNewReno::GetTypeId ()));
  stack.Install (wifiStaNodesRight);

  std::cout << "installed internet stack" << std::endl;

  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  std::cout << "assigning address left half" << std::endl;
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiStaInterfacesLeft, wifiApInterfacesLeft;
  wifiStaInterfacesLeft = address.Assign (staDevicesLeft);
  wifiApInterfacesLeft = address.Assign (apDevicesLeft);

  std::cout << "assigning address right half" << std::endl;
  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiStaInterfacesRight, wifiApInterfacesRight;
  wifiStaInterfacesRight = address.Assign (staDevicesRight);
  wifiApInterfacesRight = address.Assign (apDevicesRight);

  NS_LOG_INFO ("Create Applications.");

  // Creating a BulkSendApplication and install it on one of the wifi-nodes(except AP)

  uint16_t port = 8808; // random port for TCP server listening.

  //Setting packetsize (Bytes)
  uint32_t packetSize = 1024;

  //making half flows left to right
  int nLRflows = ceil((double)nWifi/2);
  std::cout<<"nLRflows- "<<nLRflows<<std::endl;

  // ----------- creating source for left half ------------ //
  std::cout << "creating source for left half" << std::endl;

  ApplicationContainer sourceAppsLeft;
  for (int i = 0; i < nLRflows; i++)
    {

      BulkSendHelper sourceLeft ("ns3::TcpSocketFactory",
                                 InetSocketAddress (wifiStaInterfacesRight.GetAddress (i), port));
      std::cout << "here" << std::endl;

      // Set the amount of data to send in bytes.  Zero is unlimited.
      sourceLeft.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
      sourceLeft.SetAttribute ("SendSize", UintegerValue (packetSize));

      sourceAppsLeft.Add (sourceLeft.Install (wifiStaNodesLeft.Get (i)));
    }
  sourceAppsLeft.Start (Seconds (1.0));
  sourceAppsLeft.Stop (Seconds (100.0));

  // ---------- creating source for right half ----------- //
  std::cout << "creating source for right half" << std::endl;

  ApplicationContainer sourceAppsRight;
  for (int i = nLRflows; i < nWifi; i++)
    {
      BulkSendHelper sourceRight ("ns3::TcpSocketFactory",
                                  InetSocketAddress (wifiStaInterfacesLeft.GetAddress (i), port));
      // Set the amount of data to send in bytes.  Zero is unlimited.
      sourceRight.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
      sourceRight.SetAttribute ("SendSize", UintegerValue (packetSize));
      sourceAppsRight.Add (sourceRight.Install (wifiStaNodesRight.Get (i)));
    }
  sourceAppsRight.Start (Seconds (1.0));
  sourceAppsRight.Stop (Seconds (100.0));

  // Creating a PacketSinkApplication and install it on one of the CSMA nodes
  std::cout << "creating sink for left half" << std::endl;

  ApplicationContainer sinkAppsLeft;
  for (int i = nLRflows; i < nWifi; i++)
    {
      PacketSinkHelper sinkLeft ("ns3::TcpSocketFactory",
                                 InetSocketAddress (wifiStaInterfacesLeft.GetAddress (i), port));
      sinkAppsLeft.Add (sinkLeft.Install (wifiStaNodesLeft.Get (i)));
    }
  sinkAppsLeft.Start (Seconds (0.0));
  sinkAppsLeft.Stop (Seconds (100.0));

  std::cout << "creating sink for right half" << std::endl;

  ApplicationContainer sinkAppsRight;
  for (int i = 0; i < nLRflows; i++)
    {
      PacketSinkHelper sinkRight ("ns3::TcpSocketFactory",
                                 InetSocketAddress (wifiStaInterfacesRight.GetAddress (i), port));
      sinkAppsRight.Add (sinkRight.Install (wifiStaNodesRight.Get (i)));
    }
  sinkAppsRight.Start (Seconds (0.0));
  sinkAppsRight.Stop (Seconds (100.0));

  std::cout << "populating routing tables" << std::endl;

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  //Initialisation of global variable which are used for Throughput and Delay Calculation.
  NodeContainer staNodes;
  for(int i=0;i<nLRflows;i++){
    staNodes.Add(wifiStaNodesLeft.Get(i));
    staNodes.Add(wifiStaNodesRight.Get(i));
  }
  
  data.monitor = data.flowmon.Install(staNodes);
  data.lastTotalRxBytes = 0;
  data.lastRxPackets = 0;
  data.lastDelaySum = 0;
  Simulator::Schedule (Seconds (1.0), &Throughput);

  Simulator::Stop (Seconds (101.0));

  if (tracing == true)
    {
      pointToPoint.EnablePcapAll ("third_p2p");
      phyLeft.EnablePcap ("third_phy_left", apDevicesLeft.Get (0));
      phyRight.EnablePcap ("third_phy_right", apDevicesRight.Get (0));
    }

  AnimationInterface anim ("twoWayFlow_anim.xml"); // Mandatory
  for (uint32_t i = 0; i < wifiStaNodesLeft.GetN (); ++i)
    {
      anim.UpdateNodeDescription (wifiStaNodesLeft.Get (i), "STA_L"); // Optional
      anim.UpdateNodeColor (wifiStaNodesLeft.Get (i), 255, 0, 0); // Optional
    }
  for (uint32_t i = 0; i < wifiApNodeLeft.GetN (); ++i)
    {
      anim.UpdateNodeDescription (wifiApNodeLeft.Get (i), "AP_L"); // Optional
      anim.UpdateNodeColor (wifiApNodeLeft.Get (i), 0, 255, 0); // Optional
    }
  for (uint32_t i = 0; i < wifiStaNodesRight.GetN (); ++i)
    {
      anim.UpdateNodeDescription (wifiStaNodesRight.Get (i), "STA_R"); // Optional
      anim.UpdateNodeColor (wifiStaNodesRight.Get (i), 255, 0, 0); // Optional
    }
  for (uint32_t i = 0; i < wifiApNodeRight.GetN (); ++i)
    {
      anim.UpdateNodeDescription (wifiApNodeRight.Get (i), "AP_R"); // Optional
      anim.UpdateNodeColor (wifiApNodeRight.Get (i), 0, 255, 0); // Optional
    }

  Simulator::Run ();
  Simulator::Destroy ();

  delayStream.close ();
  throughputStream.close ();

  

  // Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (sinkApps.Get (0));
  // std::cout << "Total Bytes Received: " << sink1->GetTotalRx () << std::endl;
  // std::cout << "Average Throughput: " << sink1->GetTotalRx () * 8.0 / (4 * 1024 * 1024) << " Mbps"
  //           << std::endl;
  // std::cout << "Average Delay: " << averageDelay << "ms" << std::endl;

  if (logAverages)
    {
      std::ofstream outfile;
      outfile.open ("averages.txt", std::ios_base::app); // append instead of overwrite
      outfile << nWifi << " " << tcp_name << " " << raa_name << std::endl;
      uint32_t total_Bytes_rcvd=0;
      
      for(int i=0;i<nLRflows;i++)
      {
          // Ptr<PacketSink> sinkL = DynamicCast<PacketSink> (sinkAppsLeft.Get (i));
          // total_Bytes_rcvd+= sinkL->GetTotalRx();
          Ptr<PacketSink> sinkR = DynamicCast<PacketSink> (sinkAppsRight.Get (i));
          total_Bytes_rcvd+= sinkR->GetTotalRx();

      }
      std::cout<<"here--------"<<std::endl;
      outfile<<"RndRun = "<<rngRun<<std::endl;
      outfile << "Average Throughput: " << total_Bytes_rcvd * 8.0 / (100 * 1024 * 1024) << " Mbps"
      << std::endl;
      outfile << "Average Delay: " << averageDelay << "ms" << std::endl;
      outfile.close ();
    }

  return 0;
}
