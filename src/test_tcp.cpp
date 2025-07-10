#include "tcp_tahoe.cpp"
#include <cstdio>

int main() {
    TCPTahoe* tcp = new TCPTahoe();
    tcp->send_packet();
    int cwnd = tcp->get_current_cwnd();
    printf("CWND: %d\n", cwnd);
    delete tcp;
    return 0;
} 