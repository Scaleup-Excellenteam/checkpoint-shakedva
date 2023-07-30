
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shaked");
MODULE_DESCRIPTION("Example for kernel module for ip src and dest");
MODULE_VERSION("0.01");


static struct nf_hook_ops *nfho = NULL;

static int hfunc(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	struct iphdr *iph;
	unsigned int src_ip;
	unsigned int dest_ip;
	
	if (!skb)
		return NF_ACCEPT;

	iph = ip_hdr(skb);
	src_ip = (unsigned int)iph->saddr;
    	dest_ip= (unsigned int)iph->daddr;
	printk(KERN_INFO "src: %pI4 dest: %pI4\n", &src_ip, &dest_ip);
	return NF_ACCEPT;
}

static int __init LKM_init(void)
{
	nfho = (struct nf_hook_ops*)kcalloc(1, sizeof(struct nf_hook_ops), GFP_KERNEL);
	
	/* Initialize netfilter hook */
	nfho->hook 	= (nf_hookfn*)hfunc;		/* hook function */
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
