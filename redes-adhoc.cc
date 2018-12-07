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
*   6 tipos de árvore, descritas no artigo
*   Os tipos 1 e 2 tem 5 nós filhos, 3 e 4, 20 nós filhos, 5 e 6, 30 nós filhos.
*
* Nessa simulação, todos os filhos (todos os nós menos o 0), enviam dados para o nó raiz (0).
* São medidas a taxa de entrega de pacotes e o atraso até o nó raiz.
* O usuário pode especificar a quantidade de pacotes enviados pelos nós filhos, o tempo total da simulação, e a configuração da árvore. 
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
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"

#include "ns3/core-module.h" // para o RngSeedManager 
 
NS_LOG_COMPONENT_DEFINE("wifi-arvore-adhoc");
 
using namespace ns3;

const int port = 9; 
int nFilhos = 4;
uint64_t lastTotalRx = 0; // O valor para o último total de bytes recebidos
std::ofstream myfile;
 
int main (int argc, char *argv[]){
    // LogComponentEnable ("RequestResponseClientApplication", LOG_LEVEL_INFO);
    // LogComponentEnable ("RequestResponseServerApplication",  LOG_LEVEL_INFO);

    std::string phyRate = "OfdmRate6Mbps";     /* Physical layer bitrate. */
    double simulationTime = 10;                 /* Simulation time in seconds. */
    bool pcapTracing = false;                   /* PCAP Tracing is enabled or not. */
    int round = 1;
    int arvore = 1;
    int pacotes = 1;

    // Argumentos de linha de comando
    CommandLine cmd;
    cmd.AddValue ("arvore", "Arvore utilizada para o teste", arvore);
    cmd.AddValue ("pacotes", "Número de pacotes a serem enviados pelos nós filhos", pacotes);
    cmd.AddValue ("round", "Round", round);
    cmd.AddValue ("phyRate", "Physical layer bitrate", phyRate);
    cmd.AddValue ("simulationTime", "Simulation time in seconds", simulationTime);
    cmd.AddValue ("pcap", "Enable/disable PCAP Tracing", pcapTracing);
    cmd.Parse (argc, argv);

    RngSeedManager::SetSeed(2018);  
	RngSeedManager::SetRun(round); 

    // Define um arquivo de saída
    std::string filename="Anny_"+std::to_string(arvore)+"_"+std::to_string(pacotes)+"_"+std::to_string(simulationTime)+"_"+std::to_string(round);
    std::cout << filename << std::endl;
    myfile.open(filename);

    WifiMacHelper wifiMac;
    WifiHelper wifiHelper;
    wifiHelper.SetStandard(WIFI_PHY_STANDARD_80211n_5GHZ);
 
    /* Set up Legacy Channel */
    YansWifiChannelHelper wifiChannel;
    wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5e9));

    // Setup Physical Layer
    YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
    wifiPhy.SetChannel (wifiChannel.Create ());
    wifiPhy.SetErrorRateModel("ns3::YansErrorRateModel");
    wifiHelper.SetRemoteStationManager("ns3::ConstantRateWifiManager");

    // Define nFilhos com base no tipo de árvore escolhido
    if (arvore==1 || arvore==2){
        nFilhos = 5;
    }else if(arvore==3 || arvore==4){
        nFilhos = 17;
    }else if(arvore==5 || arvore==6){
        nFilhos = 30;
    }

	// Cria os nós
    NodeContainer raiz;
    raiz.Create(1);

    NodeContainer filhos;
    filhos.Create(nFilhos);

	NodeContainer nodes(raiz, filhos); // Nodes é a concatenação do container "raiz" com o "filhos"

    NetDeviceContainer devices;

    wifiMac.SetType("ns3::AdhocWifiMac");
    devices = (wifiHelper.Install (wifiPhy, wifiMac, nodes));

    // altera a banda
    // Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/ChannelWidth", UintegerValue (40)); //ou 40
 
    /* Mobility model */
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();

    if (arvore == 1){
        positionAlloc->Add (Vector (1.0, 4.0, 0.0));
        positionAlloc->Add (Vector (1.0, 3.0, 0.0));
        positionAlloc->Add (Vector (1.0, 2.0, 0.0));
        positionAlloc->Add (Vector (1.0, 1.0, 0.0));
        positionAlloc->Add (Vector (0.0, 0.0, 0.0));
        positionAlloc->Add (Vector (2.0, 0.0, 0.0));
    }else if (arvore == 2){
        positionAlloc->Add (Vector (2.0, 1.0, 0.0));
        positionAlloc->Add (Vector (0.0, 0.0, 0.0));
        positionAlloc->Add (Vector (1.0, 0.0, 0.0));
        positionAlloc->Add (Vector (2.0, 0.0, 0.0));
        positionAlloc->Add (Vector (3.0, 0.0, 0.0));  
        positionAlloc->Add (Vector (4.0, 0.0, 0.0));  
    }else if (arvore == 3){
        positionAlloc->Add (Vector (6.0,  4.0, 0.0));
        positionAlloc->Add (Vector (6.0,  3.0, 0.0));
        positionAlloc->Add (Vector (6.0,  2.0, 0.0));
        positionAlloc->Add (Vector (2.0,  1.0, 0.0));
        positionAlloc->Add (Vector (6.0,  1.0, 0.0));
        positionAlloc->Add (Vector (10.0, 1.0, 0.0));
        positionAlloc->Add (Vector (0.0,  0.0, 0.0));
        positionAlloc->Add (Vector (1.0,  0.0, 0.0));
        positionAlloc->Add (Vector (2.0,  0.0, 0.0));
        positionAlloc->Add (Vector (3.0,  0.0, 0.0));
        positionAlloc->Add (Vector (4.0,  0.0, 0.0));
        positionAlloc->Add (Vector (5.0,  0.0, 0.0));
        positionAlloc->Add (Vector (6.0,  0.0, 0.0));
        positionAlloc->Add (Vector (7.0,  0.0, 0.0));
        positionAlloc->Add (Vector (8.0,  0.0, 0.0));
        positionAlloc->Add (Vector (9.0,  0.0, 0.0));
        positionAlloc->Add (Vector (10.0, 0.0, 0.0));
        positionAlloc->Add (Vector (11.0, 0.0, 0.0));
    }else if (arvore == 4){
        positionAlloc->Add (Vector (7.0,  2.0, 0.0));
        positionAlloc->Add (Vector (1.0,  1.0, 0.0));
        positionAlloc->Add (Vector (4.0,  1.0, 0.0));
        positionAlloc->Add (Vector (7.0,  1.0, 0.0));
        positionAlloc->Add (Vector (10.0, 1.0, 0.0));
        positionAlloc->Add (Vector (13.0, 1.0, 0.0));
        positionAlloc->Add (Vector (0.0,  0.0, 0.0));
        positionAlloc->Add (Vector (2.0,  0.0, 0.0));
        positionAlloc->Add (Vector (3.0,  0.0, 0.0));
        positionAlloc->Add (Vector (4.0,  0.0, 0.0));
        positionAlloc->Add (Vector (5.0,  0.0, 0.0));
        positionAlloc->Add (Vector (6.0,  0.0, 0.0));
        positionAlloc->Add (Vector (8.0,  0.0, 0.0));
        positionAlloc->Add (Vector (9.0,  0.0, 0.0));
        positionAlloc->Add (Vector (10.0, 0.0, 0.0));
        positionAlloc->Add (Vector (11.0, 0.0, 0.0));
        positionAlloc->Add (Vector (12.0, 0.0, 0.0));
        positionAlloc->Add (Vector (14.0, 0.0, 0.0));
    }else if(arvore == 5){
        positionAlloc->Add (Vector (12.0, 4.0, 0.0));
        positionAlloc->Add (Vector (12.0, 3.0, 0.0));
        positionAlloc->Add (Vector (12.0, 2.0, 0.0));
        positionAlloc->Add (Vector (2.0,  1.0, 0.0));
        positionAlloc->Add (Vector (7.0,  1.0, 0.0));
        positionAlloc->Add (Vector (12.0, 1.0, 0.0));
        positionAlloc->Add (Vector (17.0, 1.0, 0.0));
        positionAlloc->Add (Vector (21.0, 1.0, 0.0));
        positionAlloc->Add (Vector (0.0,  0.0, 0.0));
        positionAlloc->Add (Vector (1.0,  0.0, 0.0));
        positionAlloc->Add (Vector (2.0,  0.0, 0.0));
        positionAlloc->Add (Vector (3.0,  0.0, 0.0));
        positionAlloc->Add (Vector (4.0,  0.0, 0.0));
        positionAlloc->Add (Vector (5.0,  0.0, 0.0));
        positionAlloc->Add (Vector (6.0,  0.0, 0.0));
        positionAlloc->Add (Vector (7.0,  0.0, 0.0));
        positionAlloc->Add (Vector (8.0,  0.0, 0.0));
        positionAlloc->Add (Vector (9.0,  0.0, 0.0));
        positionAlloc->Add (Vector (10.0, 0.0, 0.0));
        positionAlloc->Add (Vector (11.0, 0.0, 0.0));
        positionAlloc->Add (Vector (12.0, 0.0, 0.0));
        positionAlloc->Add (Vector (13.0, 0.0, 0.0));
        positionAlloc->Add (Vector (14.0, 0.0, 0.0)); 
        positionAlloc->Add (Vector (15.0, 0.0, 0.0));
        positionAlloc->Add (Vector (16.0, 0.0, 0.0));
        positionAlloc->Add (Vector (17.0, 0.0, 0.0));
        positionAlloc->Add (Vector (18.0, 0.0, 0.0));
        positionAlloc->Add (Vector (19.0, 0.0, 0.0));
        positionAlloc->Add (Vector (20.0, 0.0, 0.0));
        positionAlloc->Add (Vector (21.0, 0.0, 0.0));
        positionAlloc->Add (Vector (22.0, 0.0, 0.0));        
    }else if(arvore==6){
        positionAlloc->Add (Vector (12.0, 2.0, 0.0));
        positionAlloc->Add (Vector (2.0,  1.0, 0.0));
        positionAlloc->Add (Vector (7.0,  1.0, 0.0));
        positionAlloc->Add (Vector (12.0, 1.0, 0.0));
        positionAlloc->Add (Vector (17.0, 1.0, 0.0));
        positionAlloc->Add (Vector (22.0, 1.0, 0.0));
        positionAlloc->Add (Vector (0.0,  0.0, 0.0));
        positionAlloc->Add (Vector (1.0,  0.0, 0.0));
        positionAlloc->Add (Vector (2.0,  0.0, 0.0));
        positionAlloc->Add (Vector (3.0,  0.0, 0.0));
        positionAlloc->Add (Vector (4.0,  0.0, 0.0));
        positionAlloc->Add (Vector (5.0,  0.0, 0.0));
        positionAlloc->Add (Vector (6.0,  0.0, 0.0));
        positionAlloc->Add (Vector (7.0,  0.0, 0.0));
        positionAlloc->Add (Vector (8.0,  0.0, 0.0));
        positionAlloc->Add (Vector (9.0,  0.0, 0.0));
        positionAlloc->Add (Vector (10.0, 0.0, 0.0));
        positionAlloc->Add (Vector (11.0, 0.0, 0.0));
        positionAlloc->Add (Vector (12.0, 0.0, 0.0));
        positionAlloc->Add (Vector (13.0, 0.0, 0.0));
        positionAlloc->Add (Vector (14.0, 0.0, 0.0)); 
        positionAlloc->Add (Vector (15.0, 0.0, 0.0));
        positionAlloc->Add (Vector (16.0, 0.0, 0.0));
        positionAlloc->Add (Vector (17.0, 0.0, 0.0));
        positionAlloc->Add (Vector (18.0, 0.0, 0.0));
        positionAlloc->Add (Vector (19.0, 0.0, 0.0));
        positionAlloc->Add (Vector (20.0, 0.0, 0.0));
        positionAlloc->Add (Vector (21.0, 0.0, 0.0));
        positionAlloc->Add (Vector (22.0, 0.0, 0.0));
        positionAlloc->Add (Vector (23.0, 0.0, 0.0));
        positionAlloc->Add (Vector (24.0, 0.0, 0.0));                 
    }

    mobility.SetPositionAllocator(positionAlloc);
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
    Ptr<Ipv4StaticRouting> staticRouting[nFilhos+1];
    for(int i=0; i<=nFilhos; i++){
        staticRouting[i] = ipv4RoutingHelper.GetStaticRouting (nodes.Get(i)->GetObject<Ipv4>());    
    }    

    if (arvore == 1){
        // 1 -> 0
        staticRouting[1]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1); 
        // 2 -> 1
        staticRouting[2]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.2"), 1);
        // 3 -> 2
        staticRouting[3]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.3"), 1);
        // 4 -> 3
        staticRouting[4]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);
        // 5 -> 3
        staticRouting[5]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);        
        //destination, nextHop, interface        
    }else if (arvore == 2){
        // 1 -> 0
        staticRouting[1]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 2 -> 0
        staticRouting[2]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 3 -> 0
        staticRouting[3]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 4 -> 0   
        staticRouting[4]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 5 -> 0   
        staticRouting[5]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);        
    }else if (arvore==3){
        // 1  -> 0
        staticRouting[1]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 2  -> 1
        staticRouting[2]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.2"), 1);
        // 3  -> 2
        staticRouting[3]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.3"), 1);
        // 4  -> 2
        staticRouting[4]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.3"), 1);
        // 5  -> 2
        staticRouting[5]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.3"), 1);
        // 6  -> 3
        staticRouting[6]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);
        // 7  -> 3
        staticRouting[7]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);
        // 8  -> 3
        staticRouting[8]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);
        // 9  -> 3
        staticRouting[9]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);
        // 10 -> 4
        staticRouting[10]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.5"), 1);
        // 11 -> 4
        staticRouting[11]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.5"), 1);
        // 12 -> 4
        staticRouting[12]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.5"), 1);
        // 13 -> 4
        staticRouting[13]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.5"), 1);
        // 14 -> 5
        staticRouting[14]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.6"), 1);
        // 15 -> 5
        staticRouting[15]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.6"), 1);
        // 16 -> 5
        staticRouting[16]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.6"), 1);
        // 17 -> 5
        staticRouting[17]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.6"), 1);
    }else if(arvore==4){
        // 1 -> 0
        staticRouting[1]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 2 -> 0
        staticRouting[2]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 3 -> 0
        staticRouting[3]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 4 -> 0
        staticRouting[4]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 5 -> 0
        staticRouting[5]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 6 -> 1
        staticRouting[6]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.2"), 1);
        // 7 -> 1
        staticRouting[7]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.2"), 1);
        // 8  -> 2
        staticRouting[8]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.3"), 1);
        // 9  -> 2
        staticRouting[9]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.3"), 1);
        // 10 -> 2
        staticRouting[10]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.3"), 1);
        // 11 -> 3
        staticRouting[11]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);
        // 12 -> 3
        staticRouting[12]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);
        // 13 -> 4
        staticRouting[13]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.5"), 1);
        // 14 -> 4
        staticRouting[14]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.5"), 1);
        // 15 -> 4
        staticRouting[15]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.5"), 1);
        // 16 -> 5
        staticRouting[16]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.6"), 1);
        // 17 -> 5
        staticRouting[17]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.6"), 1);
    }if (arvore==5){
        // 1  -> 0
        staticRouting[1]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 2  -> 1
        staticRouting[2]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.2"), 1);
        // 3  -> 2
        staticRouting[3]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.3"), 1);
        // 4  -> 2
        staticRouting[4]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.3"), 1);
        // 5  -> 2
        staticRouting[5]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.3"), 1);
        // 6  -> 2
        staticRouting[6]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.3"), 1);
        // 7  -> 2
        staticRouting[7]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.3"), 1);
        // 8  -> 3
        staticRouting[8]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);
        // 9 -> 3
        staticRouting[9]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);
        // 10 -> 3
        staticRouting[10]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);
        // 11 -> 3
        staticRouting[11]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);
        // 12 -> 3
        staticRouting[12]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);
        // 13 -> 4
        staticRouting[13]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.5"), 1);
        // 14 -> 4
        staticRouting[14]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.5"), 1);
        // 15 -> 4
        staticRouting[15]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.5"), 1);
        // 16 -> 4
        staticRouting[16]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.5"), 1);
        // 17 -> 4
        staticRouting[17]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.5"), 1);
        // 18 -> 5
        staticRouting[18]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.6"), 1);
        // 19 -> 5
        staticRouting[19]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.6"), 1);
        // 20 -> 5
        staticRouting[20]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.6"), 1);
        // 21 -> 5
        staticRouting[21]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.6"), 1);
        // 22 -> 5
        staticRouting[22]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.6"), 1);
        // 23 -> 6
        staticRouting[23]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.7"), 1);
        // 24 -> 6
        staticRouting[24]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.7"), 1);
        // 25 -> 6
        staticRouting[25]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.7"), 1);
        // 26 -> 6
        staticRouting[26]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.7"), 1);
        // 27 -> 6
        staticRouting[27]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.7"), 1);
        // 28 -> 7
        staticRouting[28]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.8"), 1);
        // 29 -> 7
        staticRouting[29]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.8"), 1);
        // 30 -> 7
        staticRouting[30]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.8"), 1);
    }else if (arvore==6){
        // 1  -> 0
        staticRouting[1]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 2  -> 0
        staticRouting[2]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 3  -> 0
        staticRouting[3]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 4  -> 0
        staticRouting[4]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 5  -> 0
        staticRouting[5]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.1"), 1);
        // 6  -> 1
        staticRouting[6]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.2"), 1);
        // 7  -> 1
        staticRouting[7]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.2"), 1);
        // 8  -> 1
        staticRouting[8]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.2"), 1);
        // 9 -> 1
        staticRouting[9]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.2"), 1);
        // 10 -> 1
        staticRouting[10]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.2"), 1);
        // 11 -> 2
        staticRouting[11]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.3"), 1);
        // 12 -> 2
        staticRouting[12]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.3"), 1);
        // 13 -> 2
        staticRouting[13]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.3"), 1);
        // 14 -> 2
        staticRouting[14]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.3"), 1);
        // 15 -> 2
        staticRouting[15]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.3"), 1);
        // 16 -> 3
        staticRouting[16]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);
        // 17 -> 3
        staticRouting[17]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);
        // 18 -> 3
        staticRouting[18]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);
        // 19 -> 3
        staticRouting[19]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);
        // 20 -> 3
        staticRouting[20]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.4"), 1);
        // 21 -> 4
        staticRouting[21]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.5"), 1);
        // 22 -> 4
        staticRouting[22]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.5"), 1);
        // 23 -> 4
        staticRouting[23]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.5"), 1);
        // 24 -> 4
        staticRouting[24]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.5"), 1);
        // 25 -> 4
        staticRouting[25]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.5"), 1);
        // 26 -> 5
        staticRouting[26]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.6"), 1);
        // 27 -> 5
        staticRouting[27]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.6"), 1);
        // 28 -> 5
        staticRouting[28]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.6"), 1);
        // 29 -> 5
        staticRouting[29]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.6"), 1);
        // 30 -> 5
        staticRouting[30]->AddHostRouteTo(Ipv4Address ("192.168.1.1"), Ipv4Address ("192.168.1.6"), 1);
    }

    // RequestResponseServer no nó raiz - 0
    RequestResponseServerHelper echoServer(port);
    ApplicationContainer raizApp = echoServer.Install(raiz);
    raizApp.Start(Seconds(0.0));
    raizApp.Stop(Seconds(simulationTime));    

    // RequestResponseClient nos nós filhos (enviando para o raiz=0)
    RequestResponseClientHelper echoClient (interfaces.GetAddress(0), port);
    echoClient.SetAttribute("MaxPackets", UintegerValue(pacotes));
    echoClient.SetAttribute("Interval", TimeValue (Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));
    ApplicationContainer filhosApp = echoClient.Install(filhos);
    filhosApp.Start(Seconds(1.0));
    filhosApp.Stop(Seconds(simulationTime));

    // Iniciar simulação
    Simulator::Stop(Seconds (simulationTime+1));
    Simulator::Run();

    Simulator::Destroy ();

    unsigned long int totalSent = 0;
    unsigned long int sent = 0;

    for(int i=0; i<nFilhos; i++){
        sent          = filhosApp.Get(i)->GetObject<RequestResponseClient>()->GetPacketsSent();
        totalSent     += sent;
        // std::cout << " Filho " << i+1 <<" >>  enviados: " << sent << std::endl;
    }

    std::cout << "\nPkts enviados: " << totalSent << std::endl;

    unsigned long int raizReceived = raizApp.Get(0)->GetObject<RequestResponseServer>()->GetPacketsReceived();
    double raizAtraso = raizApp.Get(0)->GetObject<RequestResponseServer>()->GetAtraso();
    // std::cout << "\nSomatório de todos os atrasos dos pacotes que chegam até o raiz: " << raizAtraso << std::endl;
    std::cout << "\nPacotes recebidos pelo nó raiz: " << raizReceived << std::endl;
    // std::cout << "\nAtraso médio dos pacotes que chegam ao nó raiz: " << (raizAtraso/raizReceived) << std::endl;

    double pdr = ((double)raizReceived) / totalSent;
    double atrasoMedio = (raizAtraso/raizReceived);
    myfile << pdr << std::endl;
    myfile << atrasoMedio << std::endl;
    std::cout << pdr << std::endl;
    std::cout << atrasoMedio << std::endl;

    myfile.close();
    return 0;
 }


