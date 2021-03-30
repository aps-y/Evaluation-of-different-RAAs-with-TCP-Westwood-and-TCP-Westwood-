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
#include "ns3/netanim-module.h"
#include "ns3/ssid.h"
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"
#include <fstream>
#include <iostream>

// This file is modified from /examples/tutorials/third.cc
// Default Network Topology
//
//   Wifi 10.1.3.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1   n2   n3   n4
//                   point-to-point  |    |    |    |
//                                   *    *    *    *
//                                     Wifi 10.1.2.0

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
  NS_LOG_UNCOND ("Throughput " << (dataInLastSecond * 8.0 * 5) / (1024 * 1024) << "Mbps");
  throughputStream << (dataInLastSecond * 8.0 * 5) / (1024 * 1024) << std::endl;

  Simulator::Schedule (Seconds (0.2), &Throughput);
}

int
main (int argc, char *argv[])
{
  bool verbose = true;

  //Number of STA(Stations)
  uint32_t nWifi = 10;

  bool tracing = true;
  uint32_t maxBytes = 0;

  // TCP protocol
  std::string transport_prot = "TcpWestwood";

  //Error Model -> Default = NistErrorRateModel
  std::string errorModelType = "ns3::NistErrorRateModel";

  //RAA algorithm (WifiManager Class) -> Default = MinstrelHt
  std::string raaAlgo = "MinstrelHt";

  //Variables to set rates of various channels in topology, Refer base topology structure.
  uint32_t p2pRate = 50;
  uint32_t p2pDelay = 10;

  //Command-Line argument to make it interactive.
  CommandLine cmd (__FILE__);
  cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);
  cmd.AddValue ("raa", "Arf/ Aarf/ Aarfcd/ Onoe/ Minstrel", raaAlgo);
  cmd.AddValue ("tcp", "TcpWestwood / TcpWestwoodPlus", transport_prot);
  cmd.AddValue ("maxBytes", "Max number of Bytes to be sent", maxBytes);
  cmd.AddValue ("p2pRate", "Mbps", p2pRate);
  cmd.AddValue ("p2pDelay", "MilliSeconds", p2pDelay);

  cmd.Parse (argc, argv);

  std::string raa_name = raaAlgo;
  raaAlgo = "ns3::" + raaAlgo + "WifiManager";

  //Store values of Throughput and delay in respective files for plotting graph
  delayStream.open ("Delay_" + raa_name + "_" + transport_prot + "_" + std::to_string (nWifi) + ".csv");
  throughputStream.open ("Throughput_" + raa_name + "_" + transport_prot + "_" + std::to_string (nWifi) + ".csv");

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
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
      // the default protocol type in ns3::TcpWestwood is WESTWOOD
      Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
    }
  else
    {
      TypeId tcpTid;
      NS_ABORT_MSG_UNLESS (TypeId::LookupByNameFailSafe (transport_prot, &tcpTid), "TypeId " << transport_prot << " not found");
      Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (transport_prot)));
    }

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue (std::to_string (p2pRate) + "Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue (std::to_string (p2pDelay) + "ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  NodeContainer wifiStaNodes_L;
  wifiStaNodes_L.Create (nWifi);
  NodeContainer wifiApNode_L = p2pNodes.Get (0);

  NodeContainer wifiStaNodes_R;
  wifiStaNodes_R.Create (nWifi);
  NodeContainer wifiApNode_R = p2pNodes.Get (1);

  YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();

  //Delay model
  channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");

  //Loss model
  channel.AddPropagationLoss ("ns3::LogDistancePropagationLossModel", "Exponent", DoubleValue (0.3),
                              "ReferenceLoss", DoubleValue (4.0));

  YansWifiPhyHelper phy_L = YansWifiPhyHelper ();
  phy_L.SetChannel (channel.Create ());
  //Error Model
  phy_L.SetErrorRateModel (errorModelType);

  YansWifiPhyHelper phy_R = YansWifiPhyHelper ();
  phy_R.SetChannel (channel.Create ());
  //Error Model
  phy_R.SetErrorRateModel (errorModelType);

  WifiHelper wifi;

  //Setting Wifi Standard
  wifi.SetStandard (WIFI_STANDARD_80211g);

  //Setting Raa Algorithm
  wifi.SetRemoteStationManager (raaAlgo);

  WifiMacHelper mac_L;
  Ssid ssid_L = Ssid ("ns-3-ssid-L");
  mac_L.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid_L), "ActiveProbing", BooleanValue (false));

  WifiMacHelper mac_R;
  Ssid ssid_R = Ssid ("ns-3-ssid-R");
  mac_R.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid_R), "ActiveProbing", BooleanValue (false));

  NetDeviceContainer staDevices_L;
  staDevices_L = wifi.Install (phy_L, mac_L, wifiStaNodes_L);
  NetDeviceContainer staDevices_R;
  staDevices_R = wifi.Install (phy_R, mac_R, wifiStaNodes_R);

  mac_L.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (ssid_L));
  mac_R.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (ssid_R));

  NetDeviceContainer apDevices_L, apDevices_R;
  apDevices_L = wifi.Install (phy_L, mac_L, wifiApNode_L);
  apDevices_R = wifi.Install (phy_R, mac_R, wifiApNode_R);

  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator", "MinX", DoubleValue (0.0), "MinY",
                                 DoubleValue (0.0), "DeltaX", DoubleValue (5.0), "DeltaY",
                                 DoubleValue (10.0), "GridWidth", UintegerValue (3), "LayoutType",
                                 StringValue ("RowFirst"));

  //Bounds for the Rectangle Grid
  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel", "Speed",
                             StringValue ("ns3::ConstantRandomVariable[Constant=1.0]"), "Bounds",
                             RectangleValue (Rectangle (-100, 100, -100, 100)));
  mobility.Install (wifiStaNodes_L);
  mobility.Install (wifiStaNodes_R);

  //Setting Mobility model
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode_L);
  mobility.Install (wifiApNode_R);

  InternetStackHelper stack;
  stack.Install (wifiApNode_L);
  stack.Install (wifiStaNodes_L);
  stack.Install (wifiApNode_R);
  stack.Install (wifiStaNodes_R);

  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiInterfaces_L;
  wifiInterfaces_L=address.Assign (staDevices_L);
  address.Assign (apDevices_L);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiInterfaces_R;
  wifiInterfaces_R=address.Assign (staDevices_R);
  address.Assign (apDevices_R);

  NS_LOG_INFO ("Create Applications.");

  // Creating a BulkSendApplication and install it on one of the wifi-nodes(except AP)

  uint16_t port = 8808; // random port for TCP server listening.

  //Setting packetsize (Bytes)
  uint32_t packetSize = 1024;


  BulkSendHelper source_L ("ns3::TcpSocketFactory",
                         InetSocketAddress (wifiInterfaces_R.GetAddress (0), port));
  // Set the amount of data to send in bytes.  Zero is unlimited.
  source_L.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  source_L.SetAttribute ("SendSize", UintegerValue (packetSize));
  ApplicationContainer sourceApps_L;
  for (int i = 0; i < int (nWifi); i++)
    {
      sourceApps_L.Add (source_L.Install (wifiStaNodes_L.Get (i)));
    }
  sourceApps_L.Start (Seconds (2.0));
  sourceApps_L.Stop (Seconds (6.0));

  BulkSendHelper source_R ("ns3::TcpSocketFactory",
                         InetSocketAddress (wifiInterfaces_L.GetAddress (0), port));
  // Set the amount of data to send in bytes.  Zero is unlimited.
  source_R.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  source_R.SetAttribute ("SendSize", UintegerValue (packetSize));
  ApplicationContainer sourceApps_R;
  for (int i = 0; i < int (nWifi); i++)
    {
      sourceApps_R.Add (source_R.Install (wifiStaNodes_R.Get (i)));
    }
  sourceApps_R.Start (Seconds (2.0));
  sourceApps_R.Stop (Seconds (6.0));

  // Creating a PacketSinkApplication and install it on one of the CSMA nodes

  PacketSinkHelper sink_L ("ns3::TcpSocketFactory",
                         InetSocketAddress (wifiInterfaces_L.GetAddress (0), port));
  ApplicationContainer sinkApps_L = sink_L.Install (wifiStaNodes_L.Get (0));
  sinkApps_L.Start (Seconds (1.0));
  sinkApps_L.Stop (Seconds (6.0));

  PacketSinkHelper sink_R ("ns3::TcpSocketFactory",
                         InetSocketAddress (wifiInterfaces_R.GetAddress (0), port));
  ApplicationContainer sinkApps_R = sink_R.Install (wifiStaNodes_R.Get (0));
  sinkApps_R.Start (Seconds (1.0));
  sinkApps_R.Stop (Seconds (6.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  //Initialisation of global variable which are used for Throughput and Delay Calculation.
  data.monitor = data.flowmon.InstallAll ();
  data.lastTotalRxBytes = 0;
  data.lastRxPackets = 0;
  data.lastDelaySum = 0;
  Simulator::Schedule (Seconds (1.0), &Throughput);

  Simulator::Stop (Seconds (7.0));

  if (tracing == true)
    {
      pointToPoint.EnablePcapAll ("Assignment2_p2p");
      phy_L.EnablePcap ("Assignment2_phy_L", apDevices_L.Get (0));
      phy_R.EnablePcap ("Assignment2_phy_R", apDevices_R.Get (0));
    }

  AnimationInterface anim ("assignment2_anim.xml"); // Mandatory
  for (uint32_t i = 0; i < wifiStaNodes_L.GetN (); ++i)
    {
      anim.UpdateNodeDescription (wifiStaNodes_L.Get (i), "STA_L"); // Optional
      anim.UpdateNodeColor (wifiStaNodes_L.Get (i), 255, 0, 0); // Optional
    }
  for (uint32_t i = 0; i < wifiApNode_L.GetN (); ++i)
    {
      anim.UpdateNodeDescription (wifiApNode_L.Get (i), "AP_L"); // Optional
      anim.UpdateNodeColor (wifiApNode_L.Get (i), 0, 255, 0); // Optional
    }
  for (uint32_t i = 0; i < wifiStaNodes_R.GetN (); ++i)
    {
      anim.UpdateNodeDescription (wifiStaNodes_R.Get (i), "STA_R"); // Optional
      anim.UpdateNodeColor (wifiStaNodes_R.Get (i), 255, 0, 0); // Optional
    }
  for (uint32_t i = 0; i < wifiApNode_R.GetN (); ++i)
    {
      anim.UpdateNodeDescription (wifiApNode_R.Get (i), "AP_R"); // Optional
      anim.UpdateNodeColor (wifiApNode_R.Get (i), 0, 255, 0); // Optional
    }

  anim.EnablePacketMetadata (); // Optional
  anim.EnableIpv4RouteTracking ("assignment2_routing.xml", Seconds (0), Seconds (5), Seconds (0.25)); //Optional
  anim.EnableWifiMacCounters (Seconds (0), Seconds (10)); //Optional
  anim.EnableWifiPhyCounters (Seconds (0), Seconds (10)); //Optional

  Simulator::Run ();
  Simulator::Destroy ();

  delayStream.close ();
  throughputStream.close ();

  Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (sinkApps_L.Get (0));
  Ptr<PacketSink> sink2 = DynamicCast<PacketSink> (sinkApps_R.Get (0));
  std::cout << "Total Bytes Received: " << (sink1->GetTotalRx ()+sink2->GetTotalRx ()) << std::endl;
  std::cout << "Average Throughput: " << (sink1->GetTotalRx ()+sink2->GetTotalRx ()) * 8.0 / (4 * 1024 * 1024) << " Mbps"
            << std::endl;
  std::cout << "Average Delay: " << averageDelay << "ms" << std::endl;

  return 0;
}