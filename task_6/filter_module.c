
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/inet.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shaked");
MODULE_DESCRIPTION("Example for kernel module that filter IPs");
MODULE_VERSION("0.01");

#define IP_TO_DROP "104.70.125.117"

static struct nf_hook_ops *nfho = NULL;

static int my_firewall(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	struct iphdr *iph;
	if (!skb)
		return NF_ACCEPT;

	iph = ip_hdr(skb);

	if(iph->saddr == in_aton(IP_TO_DROP) || iph->daddr == in_aton(IP_TO_DROP))
	{
		printk(KERN_INFO "Dropped ip src: %pI4 dest: %pI4\n", &iph->saddr, &iph->daddr);
		return NF_DROP;
	}
	return NF_ACCEPT;
}

static int __init LKM_init(void)
{
	nfho = (struct nf_hook_ops*)kcalloc(1, sizeof(struct nf_hook_ops), GFP_KERNEL);
	
	/* Initialize netfilter hook */
	nfho->hook 	= (nf_hookfn*)my_firewall;	/* hook function */
	nfho->hooknum 	= NF_INET_PRE_ROUTING;		/* received packets */
	nfho->pf 	= PF_INET;			/* IPv4 */
	nfho->priority  = NF_IP_PRI_FIRST;		/* max hook priority */
	
	nf_register_net_hook(&init_net, nfho);
	return 0;
}

static void __exit LKM_exit(void)
{
	nf_unregister_net_hook(&init_net, nfho);
	kfree(nfho);
}

module_init(LKM_init);
module_exit(LKM_exit);
