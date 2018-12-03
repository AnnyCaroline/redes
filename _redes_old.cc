/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/config-store.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/wifi-mode.h"
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"
#include "ns3/on-off-helper.h"

#include <iostream>
#include <fstream>
#include <string>
#ifndef DEBUG
#define DEBUG true
#endif

#define MAX_STA 3

using namespace ns3;

NodeContainer nodes;
/*
        <--- A
    AP  <--- B
        <--- C

    Clientes enviam dados para AP (sink).
    Número de clientes varia de 1 a 3 (MAX_STA).
*/

std::ofstream myfile;

Ptr<PacketSink> PS; 
double vazao;
uint64_t ultimoRX=0;
void calcVazao(){
	vazao = (PS->GetTotalRx () - ultimoRX) *  8.0 / 1e6;     
	ultimoRX = PS->GetTotalRx ();
  	std::cout <<vazao<< std::endl;
  	myfile <<vazao<< std::endl;
	Simulator::Schedule (MilliSeconds (1000), &calcVazao);
}

int main(int argc, char *argv[]){
  // Variáveis e seus valores default
	std::string tcpVariant =  "TcpNewReno";
	std::string protocol   =  "n";
    uint32_t payloadSize = 1472; 
    std::string dataRate = "100Mbps";
	double total = 60.0;
 	int round = 1;

    // Add a program argument        
	CommandLine cmd;
        cmd.AddValue ("protocol", "802.11n or 802.11ac (n/ac)", protocol);
	cmd.AddValue ("time", "Duração do teste", total);
	cmd.AddValue ("dataRate", "dataRate", dataRate);
  	cmd.AddValue ("round", "Round", round);
        cmd.AddValue ("payloadSize", "Payload size in bytes", payloadSize);
  	cmd.Parse (argc, argv);

    // Semente e rodada
	RngSeedManager::SetSeed(2018); 
	RngSeedManager::SetRun(round);

    // Define um arquivo de saída
	std::string filename="Resultado_"+protocol+"_"+dataRate+"_"+std::to_string(total)+"_"+std::to_string(round);
	std::cout << filename << std::endl;
	myfile.open(filename);

    // Adiciona o prefixo ns3:: ao tcpVariant
	tcpVariant = std::string ("ns3::") + tcpVariant;

	YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
	channel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

	YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();

	phy.SetChannel (channel.Create ());
	
	WifiMacHelper mac;

    // Defino o protocolo (IEEE 802.11n ou 802.11ac) pelo argumento de linha de comando "protocol"
	WifiHelper wifi;
    if (protocol.compare("n")){
        wifi.SetStandard (WIFI_PHY_STANDARD_80211n_5GHZ);
    }else{
        wifi.SetStandard (WIFI_PHY_STANDARD_80211ac); //IEEE 802.11ac operates in 5 GHz                
    }
    wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue("OfdmRate6Mbps"), "ControlMode", StringValue("OfdmRate6Mbps")); //https://groups.google.com/forum/#!topic/ns-3-users/gXEXhjeNy_o    

	//Cria os nós: nodes contém [AP, A]
	NodeContainer nodes;
	nodes.Create(2);
	
	NetDeviceContainer devices;

	mac.SetType ("ns3::ApWifiMac");
	devices=(wifi.Install (phy, mac, nodes.Get(0)));

	mac.SetType ("ns3::StaWifiMac");
    devices.Add(wifi.Install (phy, mac, nodes.Get(1)));

	if(DEBUG)std::cout<<"Devices = "<<devices.GetN()<<"\n";

    // Na verdade não preciso dessa comparação, mas não quis mexer.
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
	positionAlloc->Add (Vector (0.0, 0.0, 0.0)); // AP
	positionAlloc->Add (Vector (0.1, 0.1, 0.0)); // A
	mobility.SetPositionAllocator (positionAlloc);
	mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	mobility.Install(nodes);

    //portas usadas tanto para o source quanto para 
	uint16_t port = 8080; 

    // Instala o transmissor nos nós A, B e C, dependendo do nSta
    // Envia pro AP (endereço 0)
    OnOffHelper server ("ns3::TcpSocketFactory", InetSocketAddress(interfaces.GetAddress(0), port));
    server.SetAttribute ("PacketSize", UintegerValue (payloadSize));
    server.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    server.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    server.SetAttribute ("DataRate", DataRateValue (DataRate (dataRate)));
    ApplicationContainer sourceApps = server.Install (nodes.Get(1));
    sourceApps.Start (Seconds(1.0));
    sourceApps.Stop (Seconds(total));
 
    // PacketSink - Receive and consume traffic generated to an IP address and port
	PacketSinkHelper sink ("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port) );

    // Define aplicação para o sink [AP]
	ApplicationContainer sinkApps = sink.Install (nodes.Get(0));
	sinkApps.Start (Seconds(0.0));
	sinkApps.Stop (Seconds(total));

	PS = StaticCast<PacketSink>(sinkApps.Get (0));

	if(DEBUG)Simulator::Schedule (Seconds (2), &calcVazao);


	Simulator::Stop(Seconds (total + 1));
  	Simulator::Run();
  	Simulator::Destroy ();
	
	myfile <<((PS->GetTotalRx () * 8) / (1e6  * (total-1)))<< std::endl;
        std::cout <<"Media: " << ((PS->GetTotalRx () * 8) / (1e6  * (total-1)))<< std::endl;
	myfile.close();
	return 0;
}
