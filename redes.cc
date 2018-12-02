/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"

#include <iostream>
#include <fstream>
#include <string>
#ifndef DEBUG
#define DEBUG true
#endif

using namespace ns3;


NodeContainer nodes;
/*
	A1 --- AP1 --- B1==AUX
*/


std::ofstream myfile;


Ptr<PacketSink> B1; 
double vazao;
uint64_t ultimoRX=0;
void calcVazao(){
	vazao = (B1->GetTotalRx () - ultimoRX) *  8.0 / 1e6;     
	ultimoRX = B1->GetTotalRx ();
  	std::cout <<vazao<< std::endl;
  	myfile <<vazao<< std::endl;
	Simulator::Schedule (MilliSeconds (1000), &calcVazao);
}

int main(int argc, char *argv[]){

        // Variáveis e seus valores default
	std::string tcpVariant =  "TcpNewReno";
        uint32_t payloadSize = 1472; 
        std::string dataRate = "100Mbps";
	double total = 60.0;
	int round = 1;
        uint32_t nWifi = 3;

        // Add a program argument        
	CommandLine cmd;
        cmd.AddValue ("nWifi", "Number of wifi STA devices", nWifi);
	cmd.AddValue ("time", "Duração do teste", total);
	cmd.AddValue ("dataRate", "dataRate", dataRate);
  	cmd.AddValue ("round","Round",round);
        cmd.AddValue ("payloadSize", "Payload size in bytes", payloadSize);
  	cmd.Parse (argc, argv);

        // Semente e rodada
	RngSeedManager::SetSeed(2018);  
	RngSeedManager::SetRun(round); 

        // Define um arquivo de saída
	std::string filename="Resultado_"+tcpVariant+"_"+dataRate+"_"+std::to_string(total)+"_"+std::to_string(round);
	std::cout << filename << std::endl;
	myfile.open(filename);

        // Adiciona o prefixo ns3:: ao tcpVariant
	tcpVariant = std::string ("ns3::") + tcpVariant;

	//Cria os nós: nodes contem [A1,AP1,B1]
	NodeContainer nodes;
	nodes.Create(3);

	YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
	channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");

	YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();

	phy.SetChannel (channel.Create ());
	
	WifiMacHelper mac;

	WifiHelper wifi;
	
        //ToDo: deixar standard dinâmico
        wifi.SetStandard (WIFI_PHY_STANDARD_80211n_5GHZ);
	wifi.SetRemoteStationManager ("ns3::IdealWifiManager");
	
	NetDeviceContainer devices;

	mac.SetType ("ns3::StaWifiMac");
	devices=(wifi.Install (phy, mac, nodes.Get(0)));
	devices.Add(wifi.Install (phy, mac, nodes.Get(1)));
	mac.SetType ("ns3::ApWifiMac");
	devices.Add(wifi.Install (phy, mac, nodes.Get(2)));


	if(DEBUG)std::cout<<"Devices = "<<devices.GetN()<<"\n";

        //Na verdade não preciso dessa comparação, mas não quis mexer.
	if (tcpVariant.compare ("ns3::TcpWestwoodPlus") == 0){ 
		Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
		Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
	}else{
		TypeId tcpTid;
		NS_ABORT_MSG_UNLESS (TypeId::LookupByNameFailSafe (tcpVariant, &tcpTid), "TypeId " << tcpVariant << " not found");
		Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (tcpVariant)));
	}

	InternetStackHelper stack;
	stack.Install(nodes);

	Ipv4AddressHelper address;
	address.SetBase ("192.168.1.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces = address.Assign (devices);

	Ipv4GlobalRoutingHelper::PopulateRoutingTables();
        
        //Define a posição de cada nó. Todos são fixos, não tem movimentação.
	MobilityHelper mobility;
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
	positionAlloc->Add (Vector (0.0, 0.0, 0.0));
	positionAlloc->Add (Vector (0.1, 0.0, 0.0));
	positionAlloc->Add (Vector (0.2, 0.0, 0.0));
	mobility.SetPositionAllocator (positionAlloc);
	mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	mobility.Install(nodes);

        //portas usadas tanto para o source quanto para 
	uint16_t port = 8080; 

        
        // BulkSendApplication - Vou enviar o máximo de tráfego possível para o B
        BulkSendHelper source ("ns3::TcpSocketFactory", InetSocketAddress (interfaces.GetAddress(2), port));
	source.SetAttribute ("MaxBytes", UintegerValue(0));
	
        // BulkSendApplication - Defino que quem envia o tráfego para o B, é o  A
	ApplicationContainer sourceApps = source.Install(nodes.Get (0));
	sourceApps.Start (Seconds (1.0));
	sourceApps.Stop (Seconds (total));
        
        // PacketSink - Receive and consume traffic generated to an IP address and port
	PacketSinkHelper sink ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny(), port));

        // Define aplicação para o sink [B]
	ApplicationContainer sinkApps = sink.Install (nodes.Get(2));
	sinkApps.Start (Seconds (0.0));
	sinkApps.Stop (Seconds (total));

	B1 = StaticCast<PacketSink> (sinkApps.Get (0));

	if(DEBUG)Simulator::Schedule (Seconds (2), &calcVazao);

	Simulator::Stop(Seconds (total + 1));
  	Simulator::Run();
  	Simulator::Destroy ();
	
	myfile <<((B1->GetTotalRx () * 8) / (1e6  * (total-1)))<< std::endl;
	myfile.close();
	return 0;
}
