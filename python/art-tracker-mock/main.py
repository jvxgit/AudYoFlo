import sys
import socket
import time
from pathlib import Path
from ipaddress import ip_address
from dataclasses import dataclass

from scapy.all import rdpcap, IP, UDP

@dataclass
class Packet:
    payload: bytes
    dest_port: int
    delay: float


def _get_sequence_id(payload: bytes) -> int:
    start = 3
    end = payload.find(b"\r\n", 3, 50)
    return int(payload[start:end])


def _read_packets(pcap_file: Path, pcap_ip: ip_address, pcap_ports: list[int]) -> list[Packet]:
    res: list[Packet] = []
    with pcap_file.open("rb") as f_in:
        packets = rdpcap(f_in)
        last_time: float | None = None
        for pkt in (pkt for pkt in packets if IP in pkt and UDP in pkt):
            ip_layer = pkt[IP]
            udp_layer = pkt[UDP]
            payload = bytes(udp_layer.payload)
            if udp_layer.dport in pcap_ports and payload[0:3] == b"fr " and ip_address(ip_layer.dst) == pcap_ip:
                delay: float
                if last_time is not None:
                    delay = float(pkt.time) - last_time
                else:
                    delay = 0.
                res.append(Packet(payload, udp_layer.dport, delay))
                last_time = float(pkt.time)
    return res


def main(pcap_file: Path, pcap_ip: ip_address, pcap_ports: list[int], dest_ip: ip_address):
    packets = _read_packets(pcap_file, pcap_ip, pcap_ports)
    num_packets = len(packets)
    udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        sequence_id = 0
        while True:
            sequence_id += 1
            packet = packets[sequence_id % num_packets]
            time.sleep(packet.delay)
            # TODO spoof sequence id
            udp_socket.sendto(packet.payload, (str(dest_ip), packet.dest_port))
    finally:
        udp_socket.close()


if __name__ == "__main__":
    if len(sys.argv) != 5:
        sys.stderr.write(f"Usage: {sys.argv[0]} pcap-file pcap-dest-ip pcap-dest-ports dest-ip\npcap-dest-ports are comma separated\n")
        sys.exit(1)
    pcap_file = Path(sys.argv[1])
    pcap_dest_ip = ip_address(sys.argv[2])
    pcap_dest_ports = [int(x) for x in sys.argv[3].split(",")]
    dest_ip = ip_address(sys.argv[4])
    main(pcap_file, pcap_dest_ip, pcap_dest_ports, dest_ip)
