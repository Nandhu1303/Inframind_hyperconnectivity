#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/bridge-helper.h"
#include "ns3/internet-module.h"
#include "ns3/config-store-module.h"
#include "ns3/netanim-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include <vector>
#include <stdint.h>
#include <sstream>
#include <fstream>

using namespace ns3;

int main(int argc, char *argv[]){
	uint32_t nAp = 4;
        uint32_t nstas = 2; 
        LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
  	LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);



  std::vector<NetDeviceContainer> staDevices;
  std::vector<NetDeviceContainer> apDevices;
  //std::vector<Ipv4InterfaceContainer> staInterfaces;
  //std::vector<Ipv4InterfaceContainer> apInterfaces;
  std::vector<NodeContainer> staNodes;

	NodeContainer apNode;
	apNode.Create(nAp);
        
	NodeContainer serverNode;
	serverNode.Create(1);

	NodeContainer switchNode;
	switchNode.Create(1);
	
	NetDeviceContainer *backboneDevices = new NetDeviceContainer[nAp + 1];

	CsmaHelper csma, csma2;
	for(uint32_t i = 0; i < nAp; i++){ 
    		backboneDevices[i] = csma.Install (NodeContainer(apNode.Get(i), switchNode.Get(0)));
  	}
	backboneDevices[nAp] = csma.Install (NodeContainer(serverNode.Get(0), switchNode.Get(0)));

	BridgeHelper switch0;
	//BridgeHelper bridge0;
  	NetDeviceContainer switchDev; 
  	for(uint32_t i = 0; i < nAp + 1; i++){ 
    		switchDev.Add(backboneDevices[i].Get(1)); 
  	}
  	switch0.Install(switchNode.Get(0), switchDev); 


	YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
  	//wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO); 

  	YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();

  	phy.SetChannel (wifiChannel.Create ());


	Ssid ssid =  Ssid ("Ap-1");





	for(uint32_t i = 0; i < nAp; i++)
	{ 
	WifiHelper wifi1;
  	wifi1.SetStandard (WIFI_PHY_STANDARD_80211n_2_4GHZ);
	wifi1.SetRemoteStationManager ("ns3::IdealWifiManager", "RtsCtsThreshold", UintegerValue (999999));

	WifiMacHelper mac1;

	NodeContainer stationNode;
	stationNode.Create(nstas);
	//phy.Set("ChannelNumber", UintegerValue(1 + (i % 3) * 5)); 
	mac1.SetType ("ns3::StaWifiMac","Ssid", SsidValue (ssid),"ActiveProbing", BooleanValue (true));
	phy.Set ("TxPowerStart", DoubleValue (5.0)); // dBm (1.26 mW)
        phy.Set ("TxPowerEnd", DoubleValue (5.0));
	NetDeviceContainer staDevices1;
  	staDevices1 = wifi1.Install (phy, mac1, stationNode);

 	mac1.SetType ("ns3::ApWifiMac",
               "Ssid", SsidValue (ssid));
		phy.Set ("TxPowerStart", DoubleValue (15.0)); // dBm (1.26 mW)
        phy.Set ("TxPowerEnd", DoubleValue (15.0));
  	NetDeviceContainer apDevices1;
  	apDevices1 = wifi1.Install (phy, mac1, apNode.Get(i));

	      staNodes.push_back (stationNode);
      	apDevices.push_back (apDevices1);
      	//apInterfaces.push_back (apInterface);
      	staDevices.push_back (staDevices1);
      //staInterfaces.push_back (staInterface);

	}


	InternetStackHelper internetstack;
	
  	internetstack.Install(serverNode); 
  	internetstack.Install(switchNode);
	internetstack.Install (apNode);
	internetstack.Install(staNodes[0]);
	internetstack.Install(staNodes[1]);
	internetstack.Install(staNodes[2]);
	internetstack.Install(staNodes[3]);

  	Ipv4AddressHelper ip;
  	Ipv4InterfaceContainer serverInterface;
  	serverInterface = ip.Assign(backboneDevices[nAp].Get(0));

	Ipv4InterfaceContainer apInterface1;
	apInterface1 = ip.Assign(apDevices[0]);
	Ipv4InterfaceContainer apInterface2;
	apInterface2 = ip.Assign(apDevices[1]);
	Ipv4InterfaceContainer apInterface3;
	apInterface3 = ip.Assign(apDevices[2]);
	Ipv4InterfaceContainer apInterface4;
	apInterface4 = ip.Assign(apDevices[3]);

	Ipv4InterfaceContainer staInterface1;
	staInterface1 = ip.Assign(staDevices[0]);


	Ipv4InterfaceContainer staInterface2;
	staInterface2 = ip.Assign(staDevices[1]);


	Ipv4InterfaceContainer staInterface3;
	staInterface3 = ip.Assign(staDevices[2]);


	Ipv4InterfaceContainer staInterface4;
	staInterface4 = ip.Assign(staDevices[3]);

	MobilityHelper mobility;

  	mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (100.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

	mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  	mobility.Install (apNode);
        mobility.Install (switchNode);
	mobility.Install (serverNode);
	//mobility.Install(staNodes[0]);
	mobility.Install(staNodes[1]);
	mobility.Install(staNodes[2]);
	mobility.Install(staNodes[3]);

	mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
	
  	mobility.Install (staNodes[0]);
	
  

        Ptr<ConstantVelocityMobilityModel> mobile;
	mobile = (staNodes[0].Get(0))->GetObject<ConstantVelocityMobilityModel>();
	mobile->SetPosition(Vector(0.0,125.0,0.0));
	mobile->SetVelocity(Vector(15.0,0.0,0.0));

	mobile = (staNodes[0].Get(1))->GetObject<ConstantVelocityMobilityModel>();
	mobile->SetPosition(Vector(0.0,110.0,0.0));
	mobile->SetVelocity(Vector(15.0,0.0,0.0));

	/*mobile = (stationNode2.Get(0))->GetObject<ConstantVelocityMobilityModel>();
	mobile->SetPosition(Vector(145.0,145.0,0.0));
	mobile->SetVelocity(Vector(10.0,0.0,0.0));

	mobile = (stationNode2.Get(1))->GetObject<ConstantVelocityMobilityModel>();
	mobile->SetPosition(Vector(155.0,155.0,0.0));
	mobile->SetVelocity(Vector(15.0,0.0,0.0));

	mobile = (stationNode3.Get(0))->GetObject<ConstantVelocityMobilityModel>();
	mobile->SetPosition(Vector(220.0,220.0,0.0));
	mobile->SetVelocity(Vector(15.0,0.0,0.0));*/

  	UdpServerHelper srv(9);  
  	ApplicationContainer srv_apps = srv.Install (staNodes[0].Get(1)); //server = UDP recv 
  	srv_apps.Start (Seconds (0.5));
  	srv_apps.Stop (Seconds (15.0));

  	UdpClientHelper client(staInterface1.GetAddress (1), 9); // dest: IP,port
  	client.SetAttribute("MaxPackets",UintegerValue (64707202));
  	client.SetAttribute("Interval",TimeValue (Time ("0.01"))); 
  	client.SetAttribute("PacketSize",UintegerValue (1450));    
  	ApplicationContainer cln_apps = client.Install (serverNode.Get(0)); //cli = UDP send
  	cln_apps.Start (Seconds (0.5));
  	cln_apps.Stop (Seconds (60.0));
	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  	AnimationInterface anim("HANDOFF_MODIFIED-nandhini.xml");
	anim.UpdateNodeColor(apNode.Get(1),255,255,0);
	anim.UpdateNodeColor(apNode.Get(2),255,255,0);
	anim.UpdateNodeColor(apNode.Get(3),255,255,0);

	anim.UpdateNodeColor(switchNode.Get(0),0,255,0);
	anim.UpdateNodeColor(serverNode.Get(0),0,255,0);
	
	anim.UpdateNodeColor(staNodes[0].Get(0),0,255,255);
	anim.UpdateNodeColor(staNodes[0].Get(1),0,255,255);
	anim.UpdateNodeColor(staNodes[1].Get(0),0,255,255);
	anim.UpdateNodeColor(staNodes[1].Get(1),0,255,255);
	anim.UpdateNodeColor(staNodes[2].Get(0),0,255,255);
	anim.UpdateNodeColor(staNodes[2].Get(1),0,255,255);
	anim.UpdateNodeColor(staNodes[3].Get(0),0,255,255);
	anim.UpdateNodeColor(staNodes[3].Get(1),0,255,255);

	anim.UpdateNodeDescription(apNode.Get(0),"Ap-1");
        anim.SetConstantPosition(apNode.Get(0),0.0,100.0);
	anim.UpdateNodeDescription(apNode.Get(1),"Ap-2");
	anim.SetConstantPosition(apNode.Get(1),100.0,100.0);
	anim.UpdateNodeDescription(apNode.Get(2),"Ap-3");
	anim.SetConstantPosition(apNode.Get(2),200.0,100.0);
	anim.UpdateNodeDescription(apNode.Get(3),"Ap-4");
	anim.SetConstantPosition(apNode.Get(3),300.0,100.0);

	anim.UpdateNodeDescription(switchNode.Get(0),"Switch Node");
	anim.SetConstantPosition(switchNode.Get(0),100.0,50.0);
	anim.UpdateNodeDescription(serverNode.Get(0),"Server Node");
	anim.SetConstantPosition(serverNode.Get(0),100.0,0.0);

	anim.UpdateNodeDescription(staNodes[3].Get(0),"Station-4-1 node");
	anim.SetConstantPosition(staNodes[3].Get(0),0.0,0.0);
	anim.UpdateNodeDescription(staNodes[3].Get(1),"Station-4-2 node");
	anim.SetConstantPosition(staNodes[3].Get(1),0.0,0.0);

        anim.UpdateNodeDescription(staNodes[2].Get(0),"Station-3-1 node");
	anim.SetConstantPosition(staNodes[2].Get(0),0.0,0.0);
	anim.UpdateNodeDescription(staNodes[2].Get(1),"Station-3-2 node");
	anim.SetConstantPosition(staNodes[2].Get(1),0.0,0.0);
	
        anim.UpdateNodeDescription(staNodes[1].Get(0),"Station-2-1 node");
	anim.SetConstantPosition(staNodes[1].Get(0),0.0,0.0);
	anim.UpdateNodeDescription(staNodes[1].Get(1),"Station-2-2 node");
	anim.SetConstantPosition(staNodes[1].Get(1),0.0,0.0);

	anim.UpdateNodeDescription(staNodes[0].Get(0),"Station-1-1 node");
	anim.UpdateNodeDescription(staNodes[0].Get(1),"Station-1-2 node");


	//anim.SetConstantPosition(stationNode1.Get(1),0.0,525.0);
	//anim.SetConstantPosition(serverNode.Get(0),110.0,110.0);
  	//anim.SetMaxPktsPerTraceFile(9999999999999999);
	Simulator::Stop(Seconds(20.0));

  	Simulator::Run ();
  	Simulator::Destroy ();
	return 0;
}



