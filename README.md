Based on ndnsim

模拟基于雾计算的新型网络隔离技术研究

修改了转发策略实现兴趣报过滤
  基于multicast策略，修改了其afterReceiveInterest函数，增加了一步对兴趣包的检查，以确定是否继续按multicast转发；
  /src/ndnSIM/NFD/daemon/fw/firewall-strategy.cpp/hpp
修改合适的网络拓扑以模拟雾计算的边缘特性和计算力
  增减雾计算节点来验证其对网络时延的影响。
  /scratch/fog-firewall.cc  /scratch/fog-topo.txt
  /scratch/firewall.cc      /scratch/topo.txt
  