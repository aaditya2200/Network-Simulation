#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-layout-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("STAR");

int main(int argc, char *argv[]) {
  Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (137));
  Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("15kb/s"));

  uint32_t nSpokes = 8;

  CommandLine cmd;
  cmd.AddValue ("nSpokes", "Number of nodes to place in the star", nSpokes);
  cmd.Parse(argc, argv);

  NS_LOG_INFO ("Build star topology.");
  PointToPointHelper p2p;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
  PointToPointStarHelper star (nSpokes, p2p);

  NS_LOG_INFO("Installing internet stack on all nodes");
  InternetStackHelper inetHelper;
  star.InstallStack(inetHelper);

  NS_LOG_INFO ("Assign IP Addresses.");
  star.AssignIpv4Addresses (Ipv4AddressHelper ("10.1.1.0", "255.255.255.0"));

  uint16_t port = 9;
  NS_LOG_INFO ("Create Applications.");
  OnOffHelper onoff ("ns3::TcpSocketFactory",
                     Address (InetSocketAddress (Ipv4Address ("255.255.255.255"), port)));
  onoff.SetConstantRate (DataRate ("500kb/s"));
  ApplicationContainer app = onoff.Install (star.Get(0);
  app.Start (Seconds (1.0));
  app.Stop (Seconds (10.0));

  AsciiTraceHelper ascii;
  csma.EnableAsciiAll (ascii.CreateFileStream ("lab4q3.tr"));


  NS_LOG_INFO ("Enable pcap tracing.");
  //
  // Do pcap tracing on all point-to-point devices on all nodes.
  //
  pointToPoint.EnablePcapAll ("star");

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

  return 0;
}
