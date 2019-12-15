import logging
logging.getLogger("kamene").setLevel(logging.ERROR)


from kamene.all import *

interface = "lo"

ether = Ether(
    src="DE:AD:BE:EF:FF:FF",
    dst="DE:AD:C0:DE:FF:FF"
)

ip = IP(
    src="10.2.3.4",
    dst="10.5.6.7"
)

packet=ether/ip

packet_list = []
packet_list += 10 * [packet]

sendp(packet_list, iface=interface, verbose=False)
