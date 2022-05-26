#ifndef __KERNEL__
    #define __KERNEL__
#endif
#ifndef MODULE
    #define MODULE
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/skbuff.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/netdevice.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <linux/netfilter_ipv4.h>
MODULE_LICENSE("Dual BSD/GPL");

#define AP_num 2
#define threshold 5000
static struct nf_hook_ops nfho;
int count = 0;
/*int flag = 0;

unsigned int AP_1[4] = {100000, 300000, 500000, 700000};
unsigned int AP_2[4] = {200000, 400000, 600000, 800000};*/
struct __kernel_sock_timeval pre, now;

struct AP_data{
	int AP;
	int count;
	unsigned int delay[4];
}APs[AP_num];

void init_AP(void){
	APs[0].AP = 1;
	APs[0].delay[0] = 100000;
	APs[0].delay[1] = 300000;
	APs[0].delay[2] = 500000;
	APs[0].delay[3] = 800000;
	APs[0].count = 0;
	APs[1].AP = 2;
	APs[1].delay[0] = 100000;
	APs[1].delay[1] = 300000;
	APs[1].delay[2] = 500000;
	APs[1].delay[3] = 700000;
	APs[1].count = 0;
}
unsigned int hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
        if(skb){
		
                struct sk_buff *sb = skb;
                struct tcphdr *tcph = NULL;
                struct udphdr *udph = NULL;
                struct iphdr *iph = NULL;
                unsigned char *payload;    // The pointer for the tcp payload.
		unsigned char *it;
		int k = 0;
		unsigned char *tail;
                u32 saddr, daddr;           /* Source and destination addresses */
		u16 sport, dport;           /* Source and destination ports */
                iph = ip_hdr(sb);
                if(iph){
			saddr = ntohl(iph->saddr);
   			daddr = ntohl(iph->daddr);

                        if(saddr == 0xc0a8ec88){ //192.168.256.136
                                switch(iph->protocol){
					tail = skb_tail_pointer(sb);
                                        case IPPROTO_UDP:
                                                /*get the udp information*/
                                                udph = udp_hdr(sb);
                                                //printk(KERN_DEBUG "UDP: %pI4h -> %pI4h\n", &saddr,&daddr);       
                                                payload = (unsigned char *)udph + (unsigned char)sizeof(struct udphdr);
                                                
                                                /*for (it = payload, k; it != tail; ++it, ++k) {
							char c = *(char *)it;

							printk("%c", c);
						    }
						printk("\n\n");*/
						char i;
						//printk("tail = %p, payload = %p\n", tail, payload);
                                                /*for(i=0;i<20;i++){
                                                        if(payload[i]) printk("%c", payload[i]);
							else break;
                                                }*/
                                                break;
                                        case IPPROTO_TCP:
                                                /*get the tcp header*/
						tcph = tcp_hdr(sb);
						sport = ntohs(tcph->source);
						dport = ntohs(tcph->dest);
                                                //printk(KERN_DEBUG "TCP: %pI4h -> %pI4h\n", &saddr,&daddr);

						payload = (unsigned char *)((unsigned char *)tcph + (tcph->doff * 4));
						
                                                for(i=0;i<20;i++){
                                                        if(payload[i]) printk("%c", payload[i]);
							else break;
                                                }
						printk("\n\n");

                                                break;
                                        default:
                                                printk("unkown protocol!\n");
                                                break;
                                }
				skb_get_new_timestamp(skb, &now);
				unsigned int time_diff = (now.tv_sec - pre.tv_sec)*1000000 + now.tv_usec - pre.tv_usec;
				int i, flag = 0;
				printk("%u", time_diff);
				for(i = 0;i < AP_num; i++){
					if(time_diff - APs[i].delay[APs[i].count] < threshold){
						APs[i].count++;
						
					}
					if(count < APs[i].count) flag = 1;
				}
				if(flag) count++;
				else{
					init_AP();
					count = 0;
				}
				if(count == 4){
					for(i = 0;i < AP_num; i++) if(APs[i].count == count) break;
					printk("This is AP_%d's flow.\n", i+1);
					init_AP();
					count = 0;
				}
				/*old code:if(time_diff - AP_1[count] < 5000 && (!flag || flag == 1)){
					flag = 1;
					count++;
				}
				else if(time_diff - AP_2[count] < 5000 && (!flag || flag == 2)){
					flag = 2;
					count++;
				}
				else{
					flag = 0;
					count = 0;
				}
				if(count == 4 && flag){
					printk("This is AP_%d's flow.\n", flag);
					flag = 0, count = 0;
				}*/
				skb_get_new_timestamp(skb, &pre);
                        }
                }
                else
                {
                        return NF_ACCEPT;
                }
        }
        else
        {
                return NF_ACCEPT;
        }

        return NF_ACCEPT;         
}


static int hello_init(void)
{
	init_AP();
        nfho.hook = hook_func;        
        nfho.hooknum  = NF_INET_PRE_ROUTING;
        nfho.pf = PF_INET;
        nfho.priority = NF_IP_PRI_FIRST; 

        nf_register_net_hook(&init_net, &nfho);

        printk("init module----------------ok\n");

        return 0;
}

static void hello_exit(void)
{
        nf_unregister_net_hook(&init_net, &nfho);
        printk("exit module----------------ok\n");
}

module_init(hello_init);  
module_exit(hello_exit); 
