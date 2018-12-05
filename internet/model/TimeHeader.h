/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Author: Anny Caroline C. Chagas <annycarolinegnr@gmail.com>
 */

#ifndef TIME_HEADER_H
#define TIME_HEADER_H

#include "ns3/header.h"

namespace ns3{
  class Buffer;
  class TimeHeader : public Header
  {
  public:
    // must be implemented to become a valid new header.
    static TypeId GetTypeId (void);
    virtual TypeId GetInstanceTypeId (void) const;
    virtual uint32_t GetSerializedSize (void) const;
    virtual void Serialize (Buffer::Iterator start) const;
    virtual uint32_t Deserialize (Buffer::Iterator start);
    virtual void Print (std::ostream &os) const;
    // allow protocol-specific access to the header data.
    void SetTimeSent (double timeSent);
    double GetTimeSent (void) const;
  private:
    double m_timeSent;
  };
} // ns3

#endif // TIME_HEADER_H