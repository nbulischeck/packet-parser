import logging
logging.getLogger("kamene").setLevel(logging.ERROR)


from kamene.all import *

interface = "lo"

ether = Ether(
    src="DE:AD:BE:EF:FF:FF",
    dst="DE:AD:C0:DE:FF:FF"
)

packet=ether

packet_list = []
packet_list += 10 * [packet]

sendp(packet_list, iface=interface, verbose=False)
