#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"
#include <string>

namespace ns3 {

int
main(int argc, char* argv[])
{

  LogComponentEnable("nfd.FirewallStrategy",LOG_LEVEL_DEBUG);

  /* setting default parameters for PointToPoint links and channels
  Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
  Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
  Config::SetDefault("ns3::QueueBase::MaxPackets", UintegerValue(20));
  */
  // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize
  
  CommandLine cmd;
  cmd.Parse(argc, argv);

  /* Creating nodes
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
  */

  AnnotatedTopologyReader topologyReader("", 25);
  topologyReader.SetFileName("scratch/fog-topo.txt");
  topologyReader.Read();

  NodeContainer producerNodes;
  producerNodes.Add(Names::Find<Node>("Node12"));
  producerNodes.Add(Names::Find<Node>("Node13"));
  producerNodes.Add(Names::Find<Node>("Node14"));
  producerNodes.Add(Names::Find<Node>("Node15"));

  NodeContainer consumerNodes;
  for(int i = 16; i <= 28; i++)
  {
    std::string nodename = "Node" + std::to_string(i);
    consumerNodes.Add(Names::Find<Node>(nodename));
  }
  
  NodeContainer routeNodes;
  for(int i = 0; i <= 6; i++)
  {
    std::string nodename = "Node" + std::to_string(i);
    routeNodes.Add(Names::Find<Node>(nodename));
  }
  routeNodes.Add(Names::Find<Node>("Node29"));
  routeNodes.Add(Names::Find<Node>("Node11"));

  NodeContainer fogNodes;
  for(int i = 8; i <= 10; i++)
  {
    std::string nodename = "Node" + std::to_string(i);
    fogNodes.Add(Names::Find<Node>(nodename));
  }
  fogNodes.Add(Names::Find<Node>("Node30"));

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.SetOldContentStore("ns3::ndn::cs::Fifo","MaxSize","100");
  ndnHelper.InstallAll();

  // Choosing forwarding strategy
  
  ndn::StrategyChoiceHelper::Install(fogNodes,"/", "/localhost/nfd/strategy/firewall");
  ndn::StrategyChoiceHelper::Install(routeNodes,"/", "/localhost/nfd/strategy/multicast");
  ndn::StrategyChoiceHelper::Install(producerNodes,"/", "/localhost/nfd/strategy/multicast");
  ndn::StrategyChoiceHelper::Install(consumerNodes,"/", "/localhost/nfd/strategy/multicast");
  // Installing applications

  NodeContainer fakeConsumer;
  NodeContainer normalConsumer;
  for(int i = 16; i <= 28; i++)
  {
    std::string nodename = "Node" + std::to_string(i);
    if(i==17||i==19||i==22||i==23)
      fakeConsumer.Add(Names::Find<Node>(nodename));
    else
      normalConsumer.Add(Names::Find<Node>(nodename));
  }

  // Consumer
  ndn::AppHelper consumerHelper0("ns3::ndn::ConsumerCbr");
  // Consumer will request /prefix/0, /prefix/1, ...
  consumerHelper0.SetPrefix("/prefix");
  consumerHelper0.SetAttribute("Frequency", StringValue("200")); // 10 interests a second
  consumerHelper0.Install(normalConsumer);

  ndn::AppHelper consumerHelper1("ns3::ndn::ConsumerCbr");
  // Consumer will request /prefix/0, /prefix/1, ...
  consumerHelper1.SetPrefix("/red");
  consumerHelper1.SetAttribute("Frequency", StringValue("200")); // 10 interests a second    
  consumerHelper1.Install(fakeConsumer);                   

  // Producer
  ndn::AppHelper producerHelper0("ns3::ndn::Producer");
  // Producer will reply to all requests starting with /prefix
  producerHelper0.SetPrefix("/prefix");
  producerHelper0.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper0.Install(producerNodes);

  ndn::AppHelper producerHelper1("ns3::ndn::Producer");
  // Producer will reply to all requests starting with /prefix
  producerHelper1.SetPrefix("/red");
  producerHelper1.SetAttribute("PayloadSize", StringValue("1024"));
  producerHelper1.Install(Names::Find<Node>("Node7"));

  ndn::GlobalRoutingHelper::CalculateRoutes();

  Simulator::Stop(Seconds(20.0));
  
  ndn::AppDelayTracer::InstallAll("delays-fog200.txt");
  //ndn::L3RateTracer::InstallAll("l3-rate-firewall.txt", Seconds(1.0));
  //ndn::CsTracer::InstallAll("cs-fog102.txt");


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
