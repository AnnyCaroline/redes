/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Author: Anny Caroline C. Chagas <annycarolinegnr@gmail.com>
 */

#include "TimeHeader.h"
#include "ns3/network-module.h"

namespace ns3{

  NS_OBJECT_ENSURE_REGISTERED (TimeHeader);

  TypeId
  TimeHeader::GetTypeId (void)
  {
    static TypeId tid = TypeId ("ns3::TimeHeader")
      .SetParent<Header> ()
      .SetGroupName ("Internet")      
      .AddConstructor<TimeHeader> ()
    ;
    return tid;
  }
  TypeId
  TimeHeader::GetInstanceTypeId (void) const
  {
    return GetTypeId ();
  }

  uint32_t 
  TimeHeader::GetSerializedSize (void) const
  {
    return 6;
  }
  void 
  TimeHeader::Serialize (Buffer::Iterator start) const
  {
    // The 2 byte-constant
    start.WriteU8 (0xfe);
    start.WriteU8 (0xef);
    // The time sent.
    start.WriteHtonU32 (m_timeSent);
  }
  uint32_t 
  TimeHeader::Deserialize (Buffer::Iterator start)
  {
    uint8_t tmp;
    tmp = start.ReadU8 ();
    NS_ASSERT (tmp == 0xfe);
    tmp = start.ReadU8 ();
    NS_ASSERT (tmp == 0xef);
    m_timeSent = start.ReadNtohU32 ();
    return 6; // the number of bytes consumed.
  }

  void 
  TimeHeader::Print (std::ostream &os) const
  {
    os << "timeSent=" << m_timeSent;
  }

  void
  TimeHeader::SetTimeSent (double timeSent){
    m_timeSent = timeSent;
  }

  double
  TimeHeader::GetTimeSent (void) const{
    return m_timeSent;
  }

}