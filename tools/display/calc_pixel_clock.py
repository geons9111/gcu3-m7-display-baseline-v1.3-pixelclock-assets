#!/usr/bin/env python3
import argparse
p=argparse.ArgumentParser()
for x in ("hactive","hfp","hsync","hbp","vactive","vfp","vsync","vbp","refresh"):
 p.add_argument("--"+x,type=int,required=True)
p.add_argument("--bpp",type=int,default=16); p.add_argument("--lanes",type=int,default=4)
a=p.parse_args()
ht=a.hactive+a.hfp+a.hsync+a.hbp; vt=a.vactive+a.vfp+a.vsync+a.vbp
pclk=ht*vt*a.refresh; lane=pclk*a.bpp/a.lanes
print(f"H_TOTAL={ht}\nV_TOTAL={vt}\nPIXEL_CLOCK_HZ={pclk}\nPIXEL_CLOCK_MHZ={pclk/1e6:.6f}")
print(f"DSI_ACTIVE_PAYLOAD_PER_LANE_BPS={lane:.0f}")
