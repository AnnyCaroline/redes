## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    obj = bld.create_ns3_program('_redes_old', ['internet', 'mobility', 'wifi', 'applications', 'point-to-point'])    
    obj.source = '_redes_old.cc'

    obj = bld.create_ns3_program('redes', ['internet', 'mobility', 'wifi', 'applications', 'point-to-point'])    
    obj.source = 'redes.cc'

    obj = bld.create_ns3_program('redes-adhoc', ['internet', 'mobility', 'wifi', 'applications', 'point-to-point'])    
    obj.source = 'redes-adhoc.cc'
