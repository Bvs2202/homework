#include "../include/smart_home.h"

int get_interface_mac(const char *interface, unsigned char *mac)
{
	struct ifreq ifr = {0};
	int sock;

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		return -1;
	}

	strncpy(ifr.ifr_name, interface, IFNAMSIZ);
	if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
		close(sock);
		return -1;
	}

	memcpy(mac, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
	close(sock);

	return 0;
}


unsigned short checksum(struct IP *ip_header)
{
	unsigned short *ptr = (unsigned short *)ip_header;
	unsigned int csum = 0;
	
	for (int i = 0; i < 10; i++) {
		csum += ptr[i];
	}

	unsigned short tmp = csum >> 16;
	csum = (csum & 0xFFFF) + tmp;

	return ~(unsigned short)csum;
}

void swap_mac_address(struct packet *pkt)
{
	unsigned char tmp_mac[ETH_ALEN];
	memcpy(tmp_mac, pkt->eth.h_dest, ETH_ALEN);
	memcpy(pkt->eth.h_dest, pkt->eth.h_source, ETH_ALEN);
	memcpy(pkt->eth.h_source, tmp_mac, ETH_ALEN);
}

void swap_ip_address(struct packet *pkt)
{
	uint32_t tmp_ip = pkt->ip.daddr;
	pkt->ip.daddr = pkt->ip.saddr;
	pkt->ip.saddr = tmp_ip;
}

void swap_udp_ports(struct packet *pkt)
{
	uint16_t tmp_port = pkt->proto.udp.dest;
	pkt->proto.udp.dest = pkt->proto.udp.source;
	pkt->proto.udp.source = tmp_port;
}
