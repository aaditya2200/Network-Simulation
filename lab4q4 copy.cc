#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Lab4 q4");

int main(int argc, char *argv[]) {
  Config::SetDefault ("ns3::CsmaNetDevice::EncapsulationMode",  StringValue ("Dix"));
  CommandLine cmd;
  cmd.Parse(argc,argv);
  NS_LOG_INFO("Creating Nodes");
  NodeContainer mainContainer;
  mainContainer.Create(8);
  NodeContainer c0 = NodeContainer(mainContainer.Get(0). mainContainer.Get(1), mainContainer.Get(2));
  NodeContainer c1 = NodeContainer(mainContainer.Get(2). mainContainer.Get(3), mainContainer.Get(4));
  NodeContainer c2 = NodeContainer(mainContainer.Get(4), mainContainer.Get(5));
  NS_LOG_INFO ("Build Topology.");
  CsmaHelper csma;
  csma.SetDeviceAttribute ("DataRate", DataRateValue (DataRate (5000000)));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));

  PointToPointHelper p2p;
  p2p.SetChannelAttribute("Delay", TimeValue (MilliSeconds (2)));
  csma.SetDeviceAttribute ("DataRate", DataRateValue (DataRate (5000000)));

  NetDeviceContainer nd0 = csma.Install (c0);  // First LAN
  NetDeviceContainer nd1 = csma.Install (c1);

  NS_LOG_INFO ("Add IP Stack.");
  InternetStackHelper internet;
  internet.Install (mainContainer);

  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4Addr;
  ipv4Addr.SetBase ("10.1.1.0", "255.255.255.0");
  ipv4Addr.Assign (nd0);
  ipv4Addr.SetBase ("10.1.2.0", "255.255.255.0");
  ipv4Addr.Assign (nd1);


  Ipv4Address multicastSource ("10.1.1.1");
  Ipv4Address multicastGroup ("225.1.2.4");

  Ipv4StaticRoutingHelper multicast;

  Ptr<Node> multicastRouter = c.Get (2);  // The node in question
  Ptr<NetDevice> inputIf = nd0.Get (2);  // The input NetDevice
  NetDeviceContainer outputDevices;  // A container of output NetDevices
  outputDevices.Add (nd1.Get (0));

  Ptr<Node> sender = c.Get (0);
  Ptr<NetDevice> senderIf = nd0.Get (0);
  multicast.SetDefaultMulticastRoute (sender, senderIf);

  uint16_t multicastPort = 9;
  OnOffHelper onoff ("ns3::UdpSocketFactory",
                     Address (InetSocketAddress (multicastGroup, multicastPort)));
  onoff.SetConstantRate (DataRate ("255b/s"));
  onoff.SetAttribute ("PacketSize", UintegerValue (128));
  ApplicationContainer srcC = onoff.Install (c0.Get (0));

  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), multicastPort));

  ApplicationContainer sinkC = sink.Install (c1.Get (2));

  return 0;
}
