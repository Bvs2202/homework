#include "server_parse.h"
#include "smart_home.h"
#include "encrypt.h"
#include "../../common/str_utils.h"

void send_ack(int fd, struct Packet *orig_pkt, const char *iface) 
{
    struct Packet ack_pkt;
    memset(&ack_pkt, 0, sizeof(ack_pkt));
    
    // 1. Ethernet заголовок (MAC берём из исходного пакета)
    memcpy(ack_pkt.eth.h_dest, orig_pkt->eth.h_source, ETH_ALEN); // MAC датчика
    memcpy(ack_pkt.eth.h_source, orig_pkt->eth.h_dest, ETH_ALEN);           // MAC сервера
    ack_pkt.eth.h_proto = htons(ETH_P_IP);
    
    // 2. IP заголовок (адреса подменяем местами)
    ack_pkt.ip.version = 4;
    ack_pkt.ip.ihl = 5;
    ack_pkt.ip.tos = 0;
    ack_pkt.ip.protocol = IPPROTO_UDP;
    ack_pkt.ip.ttl = 255;
    ack_pkt.ip.saddr = orig_pkt->ip.daddr;  // IP сервера
    ack_pkt.ip.daddr = orig_pkt->ip.saddr;  // IP датчика
    
    // 3. UDP заголовок (порты подменяем местами)
    ack_pkt.proto.udp.source = orig_pkt->proto.udp.dest;
    ack_pkt.proto.udp.dest = orig_pkt->proto.udp.source; // Порт датчика
    
    // 4. Формируем payload
    memcpy(ack_pkt.payload, "\"ack\":true", strlen("\"ack\":true"));

    // 5. Рассчитываем длины
    size_t payload_len = strlen(ack_pkt.payload);
    ack_pkt.ip.tot_len = htons(sizeof(struct Ip) + sizeof(struct Udp) + payload_len);
    ack_pkt.proto.udp.len = htons(sizeof(struct Udp) + payload_len);
    
    // 6. Контрольные суммы
    ack_pkt.ip.check = 0;
    ack_pkt.ip.check = checksum(&ack_pkt.ip);
    
    // 7. Отправка
    struct sockaddr_ll sa;
    memset(&sa, 0, sizeof(sa));
    sa.sll_family = AF_PACKET;
    sa.sll_ifindex = if_nametoindex(iface);
    sa.sll_halen = ETH_ALEN;
    memcpy(sa.sll_addr, orig_pkt->eth.h_source, ETH_ALEN); // MAC датчика

    sendto(fd, &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr*)&sa, sizeof(sa));
}

int main() {
  srand(time(NULL));

  int seq = 0;
  struct sockaddr_ll sa;
  struct Controller controllers[MAX_CONTROLLERS];
  struct timeval tv;
  struct Packet pkt;
  unsigned char mac[ETH_ALEN];
  char iface[SIZE_NAME_IFACE];
  int count_device = controller_config(CONF_FILE_NAME, iface, controllers);

  if (count_device <= 0) {
    write(FD_ERROR, "controller_config\n", SIZE_ERROR_MSG);
    return EXIT_ERROR;
  }

  if (get_interface_mac(iface, mac) < 0) {
    write(FD_ERROR, "get_interface_mac\n", SIZE_ERROR_MSG);
    return EXIT_ERROR;
  }

  memcpy(pkt.eth.h_source, mac, ETH_ALEN);

  int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

  if (fd < 0) {
    write(FD_ERROR, "socket\n", SIZE_ERROR_MSG);
    return EXIT_ERROR;
  }

  memset(&sa, 0, sizeof(sa));

  sa.sll_family = AF_PACKET;
  sa.sll_protocol = htons(ETH_P_ALL);
  sa.sll_ifindex = if_nametoindex(iface);
  sa.sll_halen = ETH_ALEN;

  fd_set rfds;

  while (1) {
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    tv.tv_sec = 1;
    tv.tv_usec = 0;

    if (select(fd + 1, &rfds, NULL, NULL, &tv) <= 0)
      continue;

    memset(&pkt, 0, sizeof(pkt));

    if (recvfrom(fd, &pkt, sizeof(pkt), FLAG_PACKET, NULL, NULL) < 0) {
      write(FD_ERROR, "recvfrom\n", SIZE_ERROR_MSG);
      continue;
    }

    if (pkt.ip.protocol == IPPROTO_UDP &&
        pkt.proto.udp.dest == htons(SRC_PORT)) {
      char device_id[SIZE_NAME_IFACE];
      float temperature;

      if (decrypt_payload(pkt.payload) == -1) {
        log_file_msg("decrypt error!", LOG_FILE_NAME);
        continue;
      }

      log_file_msg(pkt.payload, LOG_FILE_NAME);

      if (parse_sensor(pkt.payload, device_id, &temperature, &seq) < 0) {
        write(FD_ERROR, "bad JSON\n", SIZE_ERROR_MSG);
        continue;
      }

      if (seq % 5 == 0) {
        send_ack(fd, &pkt, iface);
      }


      struct Controller *contr =
          get_controller(device_id, controllers, &count_device);

      if (!contr) {
        write(FD_ERROR, "controller not found\n", SIZE_ERROR_MSG);
        continue;
      }

      memset(pkt.payload, 0, sizeof(pkt.payload));

      int command, id;

      if (temperature > MAX_TEMPERATURE) {
        command = OFF_ALL;
        *contr->state = 0;
        id = RAND_ID;
      } else {
        id = rand() % RAND_ID;
        command = rand() % RAND_CMD == ON_LIGHT ? ON_LIGHT : OFF_LIGHT;

        if (command == ON_LIGHT && (*contr->state & (1 << id)))
          command = OFF_LIGHT;

        if (command == OFF_LIGHT && !(*contr->state & (1 << id)))
          command = ON_LIGHT;

        if (command == ON_LIGHT)
          *contr->state |= (1 << id);
        else
          *contr->state &= ~(1 << id);
      }

      if (create_command_controller(pkt.payload, id + 1, command) < 0) {
        write(FD_ERROR, "JSON failed\n", SIZE_ERROR_MSG);
        continue;
      }

      log_file_msg(pkt.payload, LOG_FILE_NAME);
      swap_ip_address(&pkt);
      swap_mac_address(&pkt);

      pkt.ip.protocol = IPPROTO_TCP;
      pkt.proto.tcp.source = htons(SRC_PORT);
      pkt.proto.tcp.dest = htons(DEST_PORT);
      pkt.proto.tcp.seq = htonl(1);
      pkt.proto.tcp.ack_seq = 0;
      pkt.proto.tcp.data_off = 5 << 4;
      pkt.proto.tcp.flags = 0x18;
      pkt.proto.tcp.window = htons(SIZE_WINDOW);
      pkt.ip.daddr = contr->ip;
      pkt.ip.tot_len =
          htons(sizeof(struct Ip) + sizeof(struct Tcp) + strlen(pkt.payload));
      pkt.ip.check = 0;
      pkt.ip.check = checksum(&pkt.ip);
      pkt.eth.h_proto = htons(ETH_PROTOCOL);

      memcpy(pkt.eth.h_dest, contr->mac_dest, ETH_ALEN);
      memcpy(sa.sll_addr, contr->mac_dest, ETH_ALEN);

      if (encrypt_payload(pkt.payload) == -1) {
        log_file_msg("Encrypt error!", LOG_FILE_NAME);
        continue;
      }
      if (sendto(fd, &pkt, sizeof(pkt), FLAG_PACKET, (struct sockaddr *)&sa,
                 sizeof(sa)) < 0) {
        write(FD_ERROR, "sendto\n", SIZE_ERROR_MSG);
        continue;
      }
    } else if (pkt.ip.protocol == IPPROTO_TCP &&
               pkt.proto.tcp.dest == htons(SRC_PORT)) {
      write(FD_OUT, "TCP recv OK\n", SIZE_ERROR_MSG);
      if (decrypt_payload(pkt.payload) == -1) {
        log_file_msg("decrypt error!", LOG_FILE_NAME);
        continue;
      }
      log_file_msg(pkt.payload, LOG_FILE_NAME);
    }
  }

  close(fd);

  return 0;
}