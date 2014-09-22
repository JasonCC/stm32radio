/** @file:	agent.c
 * 
 * This file is part of STM32-Bootstrap framework
 *
 * @author	Cai XiangFeng <acecxf@126.com>
 * @date	Sep 22, 2014
 */

#include <string.h>
#include <stdlib.h>
#include <rtthread.h>
#include <dfs_posix.h>
#include <lwip/sockets.h>
#include <lwip/netif.h>
#include <time.h>

#define MOD_FMT					"[agent] "
#define AGENT_PORT				8981
#define AGENT_BUFFER_SIZE		256
#define AGENT_MAGIC				0x5958
#define AGENT_MAX_CONNECTION	2

/**
 * Agent protocol header
 */
typedef struct agent_hdr_s {
	rt_uint16_t tag;		//! magic
	rt_uint16_t len;		//! length of data
	rt_uint16_t fn;			//! function code
} agent_hdr_t;

/**
 * Function Set IP, Gateway, Netmask
 */
typedef struct msg_set_ip_s {
	rt_uint32_t ipaddr;		//! ip address
	rt_uint32_t gwaddr;		//! gateway address
	rt_uint32_t netmask;	//! netmask
} msg_set_ip_t;

typedef enum {
	AGENT_FN_UNKNOWN = 0,
	AGENT_FN_SET_IP,
	AGENT_FN_MAX
} agent_fn_code_t;

static int agent_proc(int fd, char *buf, size_t nbytes) 
{
	agent_hdr_t *hdr = (agent_hdr_t*)buf;
	msg_set_ip_t *msg;
	struct in_addr addr;

	if (nbytes < sizeof(*hdr) || hdr->tag != AGENT_MAGIC)
		return -1;
	nbytes -= sizeof(*hdr);
	msg = (msg_set_ip_t*)(hdr + 1);

	switch (hdr->fn) {
	case AGENT_FN_SET_IP:
		if (nbytes < sizeof(*msg) || hdr->len != sizeof(*msg)) 
			return -1;

		addr.s_addr = msg->ipaddr;
		rt_kprintf(MOD_FMT" set ip: %s\n", inet_ntoa(addr));
		addr.s_addr = msg->gwaddr;
		rt_kprintf(MOD_FMT" set gateway: %s\n", inet_ntoa(addr));
		addr.s_addr = msg->netmask;
		rt_kprintf(MOD_FMT" set netmask: %s\n", inet_ntoa(addr));

		if (msg->ipaddr) 
			netif_set_ipaddr(netif_default, (struct ip_addr*)&msg->ipaddr);
		if (msg->gwaddr)
			netif_set_gw(netif_default, (struct ip_addr*)&msg->gwaddr);
		if (msg->netmask)
			netif_set_netmask(netif_default, (struct ip_addr*)&msg->netmask);
		
		*(int*)msg = 0;
		send(fd, buf, sizeof(agent_hdr_t) + 4, 0);
		break;

	default:
		return -1;
	}

	return 0;
}

static void agent_thread_entry(void *param) 
{
	int sockfd, maxfdp1, comfd, nbytes;
	struct sockaddr_in local, remote;
	fd_set readfds;
	rt_uint32_t addrlen = sizeof(struct sockaddr);
	char *buffer = (char*)rt_malloc(AGENT_BUFFER_SIZE);

	local.sin_port = htons(AGENT_PORT);
	local.sin_family = PF_INET;
	local.sin_addr.s_addr = INADDR_ANY;

	FD_ZERO(&readfds);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		rt_kprintf(MOD_FMT"create socket failed\n");
		return;
	}

	bind(sockfd, (struct sockaddr*)&local, addrlen);
	listen(sockfd, AGENT_MAX_CONNECTION);

	for (;;) {
		comfd = accept(sockfd, (struct sockaddr*)&remote, &addrlen);
		if (comfd == -1) {
			rt_kprintf(MOD_FMT"error on accept, continuing...\n");
			continue;
		} else {
			rt_kprintf(MOD_FMT"get connection form %s\n", 
					   inet_ntoa(remote.sin_addr));
			for (;;) {
				nbytes = recv(comfd, buffer, AGENT_BUFFER_SIZE, 0);
				if (nbytes <= 0) {
					rt_kprintf(MOD_FMT"client %s disconnected(%d)\n", 
							   inet_ntoa(remote.sin_addr), nbytes);
					closesocket(comfd);
				} else {
					if (agent_proc(comfd, buffer, nbytes) == -1) {
						rt_kprintf(MOD_FMT"client %s disconnected(%d)\n",
								   inet_ntoa(remote.sin_addr), nbytes);
						closesocket(comfd);
					}
				}
			} // for recv
		}
	} // for accept
}

