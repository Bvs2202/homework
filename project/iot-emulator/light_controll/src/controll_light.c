#include "../../common/smart_home.h"
#include "../include/lamp.h"
#include "../include/str_utils.h"
#include "../../common/crypto/encrypt.h"

#define BINARY_SIZE 34

int main(int argc, char *argv[]) {
  if (argc != 4) return -1;
  uint32_t mask_lights = 0;

  int sockfd;
  struct sockaddr_ll cli_addr;

  //= Setting ======================================================
  struct in_addr cli_ip_addr;
  if (inet_pton(AF_INET, argv[1], &cli_ip_addr) == -1) return -1;

  uint32_t cli_ip = cli_ip_addr.s_addr;

  unsigned int cli_port = atoi(argv[2]);
  char *interface = argv[3];

  unsigned char src_mac[ETH_ALEN];
  if (get_interface_mac(interface, src_mac) != 0) return -1;
  //====================================================== Setting =

  if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) return -1;

  memset(&cli_addr, 0, sizeof(cli_addr));
  cli_addr.sll_family = AF_PACKET;
  cli_addr.sll_ifindex = if_nametoindex(interface);
  cli_addr.sll_halen = ETH_ALEN;

  if (bind(sockfd, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) == -1)
    return -1;

  while (1) {
    struct Packet pkt;
    memset(&pkt, 0, sizeof(pkt));

    struct sockaddr_ll svaddr;
    socklen_t addr_len_sv = sizeof(svaddr);

    if (recvfrom(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&svaddr,
                 &addr_len_sv) < 0)
      return -1;

    if (ntohs(pkt.proto.tcp.dest) != cli_port || pkt.ip.daddr != cli_ip ||
        pkt.ip.protocol != IPPROTO_TCP)
      continue;

    swap_mac_address(&pkt);
    swap_ip_address(&pkt);
    swap_tcp_ports(&pkt);

    pkt.ip.check = 0;
    pkt.ip.check = checksum(&pkt.ip);

    if (decrypt_payload(pkt.payload) == -1) {
      continue;
    }

    char cmd[SIZE_PAYLOAD];
    int id_light;
    short stat = 1;

    if (sscanj(pkt.payload, "{\"cmd\": \"%[^\"]\", \"id\": %d}", cmd,
               &id_light) != 2) {
      stat = 0;
    }

    char answer_json[BINARY_SIZE];

    if (strcmp(cmd, "on_light") == 0) {
      turn_on_light(id_light, &mask_lights);
    } else if (strcmp(cmd, "off_light") == 0) {
      turn_off_light(id_light, &mask_lights);
    } else if (strcmp(cmd, "all_on") == 0) {
      turn_on_all(&mask_lights);
    } else if (strcmp(cmd, "all_off") == 0) {
      turn_off_all(&mask_lights);
    } else {
      stat = 0;
    }

    binary(mask_lights, answer_json, BINARY_SIZE);

    memset(pkt.payload, 0, strlen((const char*)pkt.payload));
    snprintj(pkt.payload, SIZE_PAYLOAD * 4,
             "{\"status\": %s, \"new_stat\": 0b%s}", stat ? "ok" : "error",
             answer_json);

    if (encrypt_payload(pkt.payload) == -1) {
      continue;
    }

    if (sendto(sockfd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&svaddr,
               addr_len_sv) != 0)
      continue;
  }

  if (close(sockfd) != 0) return -1;
  return 0;
}
