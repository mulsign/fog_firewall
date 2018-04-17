#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

namespace ns3 {


int
main(int argc, char* argv[])
{
  // setting default parameters for PointToPoint links and channels
  Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
  Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
  Config::SetDefault("ns3::QueueBase::MaxPackets", UintegerValue(20));

  // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize
  CommandLine cmd;
  cmd.Parse(argc, argv);

  // Creating nodes
  NodeContainer nodes;
  nodes.Create(10);

  // Connecting nodes using two links
  PointToPointHelper p2p;
  p2p.Install(nodes.Get(0), nodes.Get(2));
  p2p.Install(nodes.Get(1), nodes.Get(2));
  p2p.Install(nodes.Get(2), nodes.Get(3));
  p2p.Install(nodes.Get(3), nodes.Get(4));
  p2p.Install(nodes.Get(2), nodes.Get(5));
  p2p.Install(nodes.Get(4), nodes.Get(6));
  p2p.Install(nodes.Get(5), nodes.Get(6));
  p2p.Install(nodes.Get(6), nodes.Get(7));
  p2p.Install(nodes.Get(7), nodes.Get(8));
  p2p.Install(nodes.Get(7), nodes.Get(9));


  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper1;
  ndnHelper1.SetDefaultRoutes(true);
  ndnHelper1.SetOldContentStore("ns3::ndn::cs::Nocache");
  ndnHelper1.Install(nodes.Get(0));
  ndnHelper1.Install(nodes.Get(1));
  ndnHelper1.Install(nodes.Get(8));
  ndnHelper1.Install(nodes.Get(9));

  ndn::StackHelper ndnHelper2;
  ndnHelper2.SetDefaultRoute(true);

  ndnHelper2.setCsSize(1000);
  ndnHelper2.setPolicy("nfd::cs::priority_fifo");
  for(int i=2; i<8; i++){
      ndnHelper2.Install(nodes.Get(i));
  }

  // Choosing forwarding strategy
  ndn::StrategyChoiceHelper::InstallAll("/prefix", "/localhost/nfd/strategy/multicast");

  // Installing applications

  // Consumer
  ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
  // Consumer will request /prefix/0, /prefix/1, ...
  consumerHelper.SetPrefix("/prefix");
  consumerHelper.SetAttribute("Frequency", StringValue("10")); // 10 interests a second
  consumerHelper.Install(nodes.Get(0));    
  consumerHelper.Install(nodes.Get(1));                   // first node

  // Producer
  ndn::AppHelper producerHelper("ns3::ndn::Producer");
  // Producer will reply to all requests starting with /prefix
  producerHelper.SetPrefix("/prefix");
  producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper.Install(nodes.Get(8));
  producerHelper.Install(nodes.Get(9));

  Simulator::Stop(Seconds(20.0));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
