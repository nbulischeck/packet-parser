import logging
logging.getLogger("kamene").setLevel(logging.ERROR)


from kamene.all import *

interface = "lo"

ether = Ether(
    src="DE:AD:BE:EF:FF:FF",
    dst="DE:AD:C0:DE:FF:FF"
)

ip = IPv6(
    src="2001:0db8:85a3:0000:0000:8a2e:0370:7334",
    dst="fe80::1ff:fe23:4567:890a"
)

packet=ether/ip

packet_list = []
packet_list += 10 * [packet]

sendp(packet_list, iface=interface, verbose=False)
