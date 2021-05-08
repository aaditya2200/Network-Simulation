#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Lab4 Question1");

int main(int argc, char *argv[]) {
  Config::SetDefault ("ns3::CsmaNetDevice::EncapsulationMode",
                                          StringValue ("Dix"));
  CommandLine cmd;
  cmd.Parse(argc, argv);
  NS_LOG_INFO ("Creating Nodes");
  NodeContainer mainContainer;
  mainContainer.Create(5);

  //Partitioning the container
  NodeContainer containerWithP2P = NodeContainer (mainContainer.Get(0),
                                                  mainContainer.Get(1));
  NodeContainer containerWithCSMA = NodeContainer (mainContainer.Get(1),
                                                    mainContainer.Get(2),
                                                    mainContainer.Get(3),
                                                    mainContainer.get(4));

  //Created Required nodes
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  p2p.SetChannelAttribute("Delay", StringValue("2ms"));

  //creating csma CsmaHelper

  CsmaHelper helper;
  helper.SetDeviceAttribute("DataRate", DataRateValue (DataRate (5000000)));
  helper.SetChannelAttribute("Delay", TimeValue (MilliSeconds (2)));


  // Creating NetDeviceContainer
  NetDeviceContainer p2pLink = p2p.Install(containerWithP2P);
  NetDeviceContainer csmaLink = helper.Install(containerWithCSMA);

  //Adding IP Stack
  InternetStackHelper stackAdder;
  stackAdder.Install(mainContainer);

  NS_LOG_INFO("Define IP Address");
  Ipv4AddressHelper v4Helper;
  v4Helper.SetBase("10.1.1.0", "255.255.255.0");
  v4Helper.Assign(p2pLink);
  v4Helper.SetBase("10.1.2.0", "255.255.255.0");
  v4Helper.Assign(csmaLink);

  NS_LOG_INFO("Multicast");
  Ipv4Address multicastSource ("10.1.1.1");
  Ipv4Address multicastGroup ("225.1.2.4");

  Ipv4StaticRoutingHelper v4StaticHelper;
  Ptr<Node> multicastRouter = containerWithP2P.Get(1);
  Ptr<NetDevice> inputIf = p2pLink.Get(1);
  NetDeviceContainer outputDevices;  // A container of output NetDevices
  outputDevices.Add (csmaLink.Get (0));

  v4StaticHelper.AddMulticastRoute(multicastRouter, multicastSource,
                              multicastGroup, inputIf, outputDevices);

Ptr<Node> sender = mainContainer.Get (0);
Ptr<NetDevice> senderIf = p2pLink.Get (0);
multicast.SetDefaultMulticastRoute (sender, senderIf);

NS_LOG_INFO ("Create Applications.");

uint16_t multicastPort = 9;

OnOffHelper onoff ("ns3::UdpSocketFactory",
                   Address (InetSocketAddress (multicastGroup, multicastPort)));
onoff.SetConstantRate (DataRate ("255b/s"));
onoff.SetAttribute ("PacketSize", UintegerValue (128));
ApplicationContainer srcC = mainContainer.Install (containerWithP2P.Get(0));

srcC.Start (Seconds (1.));
srcC.Stop (Seconds (10.));

// Create an optional packet sink to receive these packets
PacketSinkHelper sink ("ns3::UdpSocketFactory",
                       InetSocketAddress (Ipv4Address::GetAny (), multicastPort));

ApplicationContainer sinkC = sink.Install (containerWithCSMA.Get(2)); // Node n4
// Start the sink
sinkC.Start (Seconds (1.0));
sinkC.Stop (Seconds (10.0));

NS_LOG_INFO ("Configure Tracing.");
//
// Configure tracing of all enqueue, dequeue, and NetDevice receive events.
// Ascii trace output will be sent to the file "csma-multicast.tr"
//
AsciiTraceHelper ascii;
csma.EnableAsciiAll (ascii.CreateFileStream ("csma-multicast.tr"));

// Also configure some tcpdump traces; each interface will be traced.
// The output files will be named:
//     csma-multicast-<nodeId>-<interfaceId>.pcap
// and can be read by the "tcpdump -r" command (use "-tt" option to
// display timestamps correctly)
csma.EnablePcapAll ("csma-multicast", false);

//
// Now, do the actual simulation.
//
NS_LOG_INFO ("Run Simulation.");
Simulator::Run ();
Simulator::Destroy ();
NS_LOG_INFO ("Done.");

}
