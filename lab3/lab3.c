#include <linux/module.h>
#include <linux/version.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/moduleparam.h>
#include <linux/in.h>
#include <net/arp.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/proc_fs.h>


#define STANDART_SIZE 1000

static char* link = "wlp1s0";
module_param(link, charp, 0);

static char* ifname = "vni%d";
static unsigned char data[1500];

static struct net_device_stats stats;

static struct net_device *child = NULL;
struct priv {
    struct net_device *parent;
};


static struct proc_dir_entry *entry;
int count_got = 0;
static char saddr[STANDART_SIZE][32];
static char daddr[STANDART_SIZE][32];
static int lens_got[STANDART_SIZE];
static int protocols[STANDART_SIZE];

static char check_frame(struct sk_buff *skb, unsigned char data_shift) {
    unsigned char *user_data_ptr = NULL;
    struct iphdr *ip = (struct iphdr *)skb_network_header(skb);


    int data_len = 0;
    data_len = ntohs(ip->tot_len);
    if (skb->dev && data_len <= 70){
        user_data_ptr = (unsigned char *)(skb->data + sizeof(struct iphdr)) + data_shift;
        memcpy(data, user_data_ptr, data_len);
        data[data_len] = '\n';
        data[data_len + 1] = '\0';
        printk("Captured saddr: %d.%d.%d.%d\n",
               ntohl(ip->saddr) >> 24, (ntohl(ip->saddr) >> 16) & 0x00FF,
               (ntohl(ip->saddr) >> 8) & 0x0000FF, (ntohl(ip->saddr)) & 0x000000FF);
        printk("daddr: %d.%d.%d.%d\n",
               ntohl(ip->daddr) >> 24, (ntohl(ip->daddr) >> 16) & 0x00FF,
               (ntohl(ip->daddr) >> 8) & 0x0000FF, (ntohl(ip->daddr)) & 0x000000FF);

        printk(KERN_INFO "Package full length: %d\n", data_len);
        printk("Data: %s", data);
        printk("Protocol_type: %d", ip->protocol);
        sprintf(saddr[count_got],"%d.%d.%d.%d", ntohl(ip->saddr) >> 24, (ntohl(ip->saddr) >> 16) & 0x00FF,
                (ntohl(ip->saddr) >> 8) & 0x0000FF, (ntohl(ip->saddr)) & 0x000000FF);
        sprintf(daddr[count_got], "%d.%d.%d.%d", ntohl(ip->daddr) >> 24, (ntohl(ip->daddr) >> 16) & 0x00FF,
                (ntohl(ip->daddr) >> 8) & 0x0000FF, (ntohl(ip->daddr)) & 0x000000FF);
        lens_got[count_got] = data_len;
        protocols[count_got] = ip->protocol;

        count_got = (count_got + 1) % STANDART_SIZE;

        printk("-----------------------------\n");
        return 1;
    } else {
        return 0;
    }

    return 0;
}

static rx_handler_result_t handle_frame(struct sk_buff **pskb) {

    if (check_frame(*pskb, 0)) {
        stats.rx_packets++;
        stats.rx_bytes += (*pskb)->len;
    }
    (*pskb)->dev = child;
    return RX_HANDLER_ANOTHER;
}

static int open(struct net_device *dev) {
    netif_start_queue(dev);
    printk(KERN_INFO "%s: device opened", dev->name);
    return 0;
}

static int stop(struct net_device *dev) {
    netif_stop_queue(dev);
    printk(KERN_INFO "%s: device closed", dev->name);
    return 0;
}

static netdev_tx_t start_xmit(struct sk_buff *skb, struct net_device *dev) {
    struct priv *priv = netdev_priv(dev);

    if (check_frame(skb, 14)) {
        stats.tx_packets++;
        stats.tx_bytes += skb->len;
    }

    if (priv->parent) {
        skb->dev = priv->parent;
        skb->priority = 1;
        dev_queue_xmit(skb);
        return 0;
    }
    return NETDEV_TX_OK;
}

static struct net_device_stats *get_stats(struct net_device *dev) {
    return &stats;
}

static struct net_device_ops net_device_ops = {
        .ndo_open = open,
        .ndo_stop = stop,
        .ndo_get_stats = get_stats,
        .ndo_start_xmit = start_xmit
};

static void setup(struct net_device *dev) {
    int i;
    ether_setup(dev);
    memset(netdev_priv(dev), 0, sizeof(struct priv));
    dev->netdev_ops = &net_device_ops;

    //fill in the MAC address
    for (i = 0; i < ETH_ALEN; i++)
        dev->dev_addr[i] = (char)i;
}

static ssize_t my_proc_read(struct file *file, char __user * buf, size_t count, loff_t* ppos)
{
    int k = 0;
    char data[4096];
    int i ;

    if (count_got > 0) {
    for (i = 0; i < count_got; i++) {
        k += sprintf(&data[k], "SADDR: %s\nDADDR: %s\nPROTOCOL: %d\nSIZE: %d\n ----------------------\n", saddr[i], daddr[i], protocols[i], lens_got[i]);
    }
    } else {
        k += sprintf(data, "No packets\n");
    }
    data[k++] = '\0';
    if (copy_to_user(buf, data, k) != 0)
    {
        pr_info("error copy_to user\n");
        return -EFAULT;
    }
    if (*ppos > 0 || count < k) {
        return 0;
    }
    *ppos += k;
    return k;
}

static struct proc_ops proc_fops = {
        .proc_read = my_proc_read
};

int __init vni_init(void) {
    int err = 0;
    struct priv *priv;
    child = alloc_netdev(sizeof(struct priv), ifname, NET_NAME_UNKNOWN, setup);
    if (child == NULL) {
        printk(KERN_ERR "%s: allocate error", THIS_MODULE->name);
        return -ENOMEM;
    }
    priv = netdev_priv(child);
    priv->parent = __dev_get_by_name(&init_net, link); //parent interface
    if (!priv->parent) {
        printk(KERN_ERR "%s: no such net: %s", THIS_MODULE->name, link);
        free_netdev(child);
        return -ENODEV;
    }
    if (priv->parent->type != ARPHRD_ETHER && priv->parent->type != ARPHRD_LOOPBACK) {
        printk(KERN_ERR "%s: illegal net type", THIS_MODULE->name);
        free_netdev(child);
        return -EINVAL;
    }

    //copy IP, MAC and other information
    memcpy(child->dev_addr, priv->parent->dev_addr, ETH_ALEN);
    memcpy(child->broadcast, priv->parent->broadcast, ETH_ALEN);
    if ((err = dev_alloc_name(child, child->name))) {
        printk(KERN_ERR "%s: allocate name, error %i", THIS_MODULE->name, err);
        free_netdev(child);
        return -EIO;
    }

    register_netdev(child);
    rtnl_lock();
    netdev_rx_handler_register(priv->parent, &handle_frame, NULL);
    rtnl_unlock();
    entry = proc_create("var5", 0444, NULL, &proc_fops);
    printk(KERN_INFO "Module %s loaded", THIS_MODULE->name);
    printk(KERN_INFO "%s: create link %s", THIS_MODULE->name, child->name);
    printk(KERN_INFO "%s: registered rx handler for %s", THIS_MODULE->name, priv->parent->name);
    return 0;
}

void __exit vni_exit(void) {
    struct priv *priv = netdev_priv(child);
    if (priv->parent) {
        rtnl_lock();
        netdev_rx_handler_unregister(priv->parent);
        rtnl_unlock();
        printk(KERN_INFO "%s: unregister rx handler for %s", THIS_MODULE->name, priv->parent->name);
    }
    unregister_netdev(child);
    free_netdev(child);
    proc_remove(entry);
    printk(KERN_INFO "Module %s unloaded", THIS_MODULE->name);
}

module_init(vni_init);
module_exit(vni_exit);

MODULE_AUTHOR("Author");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Description");
