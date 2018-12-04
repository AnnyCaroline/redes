/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
* Copyright (c) 2015, IMDEA Networks Institute
*
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
*
* Author: Anny Caroline C. Chagas <annycarolinegnr@gmail.com>
*
*
* Network topology:
*
*   ------5 Nós-------
*   ------------------
*
*   Árvore 1
*   0(192.92.236.1 - sink)
*             |
*      1(192.92.236.2)
*             |
*      2(192.92.236.3)
*             |  
*      3(192.92.236.4)
*             |  
*      4(192.92.236.5)
*
*
*
*   Árvore 2
*                    0(192.92.236.1 - sink)
*                     /                 \
*             1(192.92.236.2)      2(192.92.236.3)
*              /          \
*   3(192.92.236.4)  4(192.92.236.5)
*
*
*
*  Árvore 3
*                           0(192.92.236.1 - sink)
*                             |  |            |  |
*           __________________|  |            |  |_____________
*          /                     |            |                \
*   1(192.92.236.2)     2(192.92.236.3)   3(192.92.236.4)  4(192.92.236.5)
*
*
* Nessa simulação, todos os filhos (todos os nós menos o 0), enviam dados para o nó raiz (0).
* Mediamos a vazão recebida em um intervalo de X.
* O usuário pode especificar o número de nós, a configuração da árvore e o data rate.
*/
 
#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/string.h"
#include "ns3/log.h"
#include "ns3/applications-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/mobility-helper.h"
#include "ns3/on-off-helper.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/mobility-model.h"
#include "ns3/packet-sink.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/tcp-westwood.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
 
NS_LOG_COMPONENT_DEFINE ("wifi-tree");
 
using namespace ns3;
 
Ptr<PacketSink> sink;     // ponteiro para o nó sink, nó raiz
uint64_t lastTotalRx = 0; // O valor para o último total de bytes recebidos
std::ofstream myfile;

void CalculateThroughput(){
    //Simulator::Now obtém o tempo virtula do simulador
    Time now = Simulator::Now ();

    // Converte os pacotes recebidos para MBits
    double cur = (sink->GetTotalRx() - lastTotalRx) * 8.0 / 1e6;
    
    // Imprime a vazão atual
    std::cout << now.GetSeconds() << "s: \t" << cur << " Mbit/s" << std::endl;
    myfile << now.GetSeconds() << "s: \t" << cur << " Mbit/s" << std::endl;
    
    lastTotalRx = sink->GetTotalRx();
    
    //Executará a rotina novamente depois de 1s
    Simulator::Schedule (MilliSeconds(1000), &CalculateThroughput);
}
 
int main (int argc, char *argv[]){
    uint32_t payloadSize = 1472;                /* Transport layer payload size in bytes. */
    std::string tcpVariant = "TcpWestwoodPlus"; /* TCP variant type. */
    std::string phyRate = "OfdmRate54Mbps";     /* Physical layer bitrate. */
    double simulationTime = 10;                 /* Simulation time in seconds. */
    bool pcapTracing = false;                   /* PCAP Tracing is enabled or not. */
    int round = 1;
    int arvore = 1;

    /* Command line argument parser setup. */
    CommandLine cmd;
    cmd.AddValue ("payloadSize", "Payload size in bytes", payloadSize);
    cmd.AddValue ("arvore", "Arvore utilizada para o teste", arvore);
    cmd.AddValue ("round", "Round", round);
    cmd.AddValue ("phyRate", "Physical layer bitrate", phyRate);
    cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
    cmd.AddValue ("pcap", "Enable/disable PCAP Tracing", pcapTracing);
    cmd.Parse (argc, argv);

    // Define um arquivo de saída
    std::string filename="Resultado_"+std::to_string(arvore)+"_"+phyRate+"_"+std::to_string(simulationTime)+"_"+std::to_string(round);
    std::cout << filename << std::endl;
    myfile.open(filename);

    tcpVariant = std::string ("ns3::") + tcpVariant;
    // Select TCP variant
    if (tcpVariant.compare ("ns3::TcpWestwoodPlus") == 0){
       // TcpWestwoodPlus is not an actual TypeId name; we need TcpWestwood here
       Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TcpWestwood::GetTypeId ()));
       // the default protocol type in ns3::TcpWestwood is WESTWOOD
       Config::SetDefault ("ns3::TcpWestwood::ProtocolType", EnumValue (TcpWestwood::WESTWOODPLUS));
    }
    else{
        TypeId tcpTid;
        NS_ABORT_MSG_UNLESS (TypeId::LookupByNameFailSafe (tcpVariant, &tcpTid), "TypeId " << tcpVariant << " not found");
        Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (tcpVariant)));
    }
 
    /* Configure TCP Options */
    Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (payloadSize));

    WifiMacHelper wifiMac;
    WifiHelper wifiHelper;
    wifiHelper.SetStandard(WIFI_PHY_STANDARD_80211n_5GHZ);
 
    /* Set up Legacy Channel */
    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5e9));

    /* Setup Physical Layer */
    YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
    wifiPhy.SetChannel (wifiChannel.Create ());
    wifiPhy.SetErrorRateModel("ns3::YansErrorRateModel");
    wifiHelper.SetRemoteStationManager("ns3::ConstantRateWifiManager");

	//Cria os nós: nodes contém [AP, STA]
	NodeContainer nodes;
	nodes.Create(5);

    NetDeviceContainer devices;

    wifiMac.SetType("ns3::AdhocWifiMac");
    devices = (wifiHelper.Install (wifiPhy, wifiMac, nodes));
    /*devices.Add (wifiHelper.Install (wifiPhy, wifiMac, nodes.Get(1)));
    devices.Add (wifiHelper.Install (wifiPhy, wifiMac, nodes.Get(2)));
    devices.Add (wifiHelper.Install (wifiPhy, wifiMac, nodes.Get(3)));
    devices.Add (wifiHelper.Install (wifiPhy, wifiMac, nodes.Get(4)));
    */

    //altera a banda
    Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/ChannelWidth", UintegerValue (40)); //ou 40
 
    /* Mobility model */
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();

    if (arvore == 1){
        positionAlloc->Add (Vector (3.0, 4.0, 0.0));
        positionAlloc->Add (Vector (1.0, 3.0, 0.0));
        positionAlloc->Add (Vector (5.0, 3.0, 0.0));
        positionAlloc->Add (Vector (0.0, 2.0, 0.0));
        positionAlloc->Add (Vector (2.0, 2.0, 0.0));      
    }else if (arvore == 2){
        positionAlloc->Add (Vector (0.0, 4.0, 0.0));
        positionAlloc->Add (Vector (0.0, 3.0, 0.0));
        positionAlloc->Add (Vector (0.0, 2.0, 0.0));
        positionAlloc->Add (Vector (0.0, 1.0, 0.0));
        positionAlloc->Add (Vector (0.0, 0.0, 0.0));  
    }else if (arvore == 3){
        positionAlloc->Add (Vector (0.0, 4.0, 0.0));
        positionAlloc->Add (Vector (0.0, 3.0, 0.0));
        positionAlloc->Add (Vector (0.0, 2.0, 0.0));
        positionAlloc->Add (Vector (0.0, 1.0, 0.0));
        positionAlloc->Add (Vector (0.0, 0.0, 0.0));  
    }

    mobility.SetPositionAllocator (positionAlloc);
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);
 
    // Internet stack
    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase ("192.168.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    // Popular a tabela de roteamento
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> staticRouting0 = ipv4RoutingHelper.GetStaticRouting (nodes.Get(0)->GetObject<Ipv4>());
    Ptr<Ipv4StaticRouting> staticRouting1 = ipv4RoutingHelper.GetStaticRouting (nodes.Get(1)->GetObject<Ipv4>());
    Ptr<Ipv4StaticRouting> staticRouting2 = ipv4RoutingHelper.GetStaticRouting (nodes.Get(2)->GetObject<Ipv4>());
    Ptr<Ipv4StaticRouting> staticRouting3 = ipv4RoutingHelper.GetStaticRouting (nodes.Get(3)->GetObject<Ipv4>());
    Ptr<Ipv4StaticRouting> staticRouting4 = ipv4RoutingHelper.GetStaticRouting (nodes.Get(4)->GetObject<Ipv4>());
    

    if (arvore == 1){
        // 1 -> 0
        staticRouting1->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1); 
        // 2 -> 0
        staticRouting2->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.2"), 1);
        // 3 -> 0
        staticRouting3->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.3"), 1);
        // 4 -> 0
        staticRouting4->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);
        //destination, nextHop, interface        
    }else if (arvore == 2){
        // 1 -> 0
        staticRouting1->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 2 -> 0
        staticRouting2->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 3 -> 0
        staticRouting3->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.2"), 1);
        // 4 -> 0   
        staticRouting4->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.2"), 1);
    }else if (arvore==3){
        // 1 -> 0
        staticRouting1->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 2 -> 0
        staticRouting2->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 3 -> 0
        staticRouting3->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 4 -> 0   
        staticRouting4->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);        
    }

    /* Sink no nó raiz - 0 */
    PacketSinkHelper sinkHelper ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny(), 9));
    ApplicationContainer sinkApp = sinkHelper.Install (nodes.Get(0)); //raiz
    sinkApp.Start(Seconds(0.0));
    sinkApp.Stop(Seconds(simulationTime));
    sink = StaticCast<PacketSink>(sinkApp.Get (0));

    // Transmitter no STA
    /*
    BulkSendHelper source ("ns3::TcpSocketFactory",InetSocketAddress (interfaces.GetAddress(0), 9));	
    ApplicationContainer sourceApps = source.Install(nodes.Get(1));//instalo no STA
    sourceApps.Start (Seconds(1.0));
    sourceApps.Stop (Seconds(simulationTime));
    */

    // Transmissores nos nós 1~4 (enviam para o raiz-0)
        OnOffHelper server ("ns3::TcpSocketFactory", (InetSocketAddress (interfaces.GetAddress(0), 9)));
        server.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
        server.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
        server.SetConstantRate (DataRate ("200Mb/s"), payloadSize);
        NodeContainer filhos;
        filhos.Add(nodes.Get(1));
        filhos.Add(nodes.Get(2));
        filhos.Add(nodes.Get(3));
        filhos.Add(nodes.Get(4));
        ApplicationContainer sourceApps = server.Install(filhos);
    /*for(int i=1; i<5; i++){    
        
    }*/

    sourceApps.Start (Seconds(1.0));
    sourceApps.Stop (Seconds(simulationTime));
 
    /* Start Simulation */ 
    Simulator::Schedule(Seconds(2), &CalculateThroughput);
    Simulator::Stop(Seconds (simulationTime+1));
    Simulator::Run();

    double averageThroughput = ((sink->GetTotalRx () * 8) / (1e6 * (simulationTime-1)));

    Simulator::Destroy ();

    std::cout << "\nAverage throughput: " << averageThroughput << " Mbit/s" << std::endl;
    myfile << "\nAverage throughput: " << averageThroughput << " Mbit/s" << std::endl;

    myfile.close();
    return 0;
 }

