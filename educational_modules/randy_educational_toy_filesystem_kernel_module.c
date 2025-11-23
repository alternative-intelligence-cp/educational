/*
 * Randy's Educational Toy Filesystem & Kernel Module
 * The Final Pieces of the OS Understanding Puzzle
 * 
 * Randy's Cannabis-Enhanced Stack Analysis:
 * "So we have a bootloader, we got several allocators, we got a scheduler, 
 * what's next? A toy file system and kernel module?"
 * 
 * THE COMPLETE EDUCATIONAL OS STACK PROGRESSION:
 * 
 * ✅ EFI Loader (Firmware layer - below OS)
 * ✅ Bootloader concepts (System initialization)  
 * ✅ Memory allocators (Resource management)
 * ✅ Process scheduler (Time allocation & fairness)
 * 🎯 Toy filesystem (Persistent storage abstraction)
 * 🎯 Kernel module (Extending system functionality)
 * 
 * Randy's Educational Philosophy:
 * "Every great OS understanding starts with curiosity about the layers.
 * We've built from firmware to processes. Now let's add persistence
 * and extensibility - the final pieces that make a real system."
 * 
 * WHY TOY FILESYSTEM + KERNEL MODULE IS THE PERFECT NEXT STEP:
 * 
 * 🗄️ FILESYSTEM EDUCATION:
 * - Bridges memory (volatile) to storage (persistent)
 * - Teaches data structure design at scale
 * - Shows how abstraction hides hardware complexity
 * - Demonstrates metadata management challenges
 * - Perfect canvas for experimenting with organization strategies
 * 
 * 🔧 KERNEL MODULE EDUCATION:
 * - Shows how to extend existing systems
 * - Teaches kernel/userspace boundaries
 * - Demonstrates device driver concepts
 * - Perfect for hardware interface learning
 * - Shows modular system design principles
 * 
 * Randy's Plant Spirit System Design Wisdom:
 * "A filesystem is like the root system of the computational garden -
 * organizing nutrients (data) for efficient distribution to all plants
 * (processes). Kernel modules are like symbiotic organisms that extend
 * the garden's capabilities in specialized ways."
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

/*
 * ============================================================================
 * PART 1: RANDY'S TOY FILESYSTEM
 * ============================================================================
 * 
 * A simple in-memory filesystem that demonstrates core concepts:
 * - Superblock management
 * - Inode allocation and management  
 * - Directory operations
 * - File read/write operations
 * - Metadata persistence simulation
 */

#define RANDYFS_MAGIC 0x52414E44  // "RAND" in hex
#define RANDYFS_MAX_FILES 256
#define RANDYFS_BLOCK_SIZE 4096
#define RANDYFS_MAX_FILENAME 64

// Randy's Cannabis-Inspired File System Structures
struct randyfs_inode {
    uint32_t ino;                           // Inode number
    uint32_t mode;                          // File permissions and type
    uint32_t size;                          // File size in bytes
    uint32_t blocks;                        // Number of blocks allocated
    uint32_t block_pointers[12];            // Direct block pointers
    uint32_t atime, mtime, ctime;           // Access, modify, create times
    char name[RANDYFS_MAX_FILENAME];        // Filename (for simplicity)
    bool is_directory;                      // Directory flag
    uint32_t parent_ino;                    // Parent directory inode
};

struct randyfs_superblock {
    uint32_t magic;                         // Magic number for identification
    uint32_t total_blocks;                  // Total filesystem blocks
    uint32_t free_blocks;                   // Available blocks
    uint32_t total_inodes;                  // Total inode count
    uint32_t free_inodes;                   // Available inodes
    uint32_t block_size;                    // Block size in bytes
    uint32_t first_data_block;              // First data block number
    char volume_name[32];                   // Filesystem label
    uint32_t last_mount_time;               // Last mount timestamp
    uint32_t mount_count;                   // Number of mounts
};

struct randyfs_fs_info {
    struct randyfs_superblock *superblock;
    struct randyfs_inode *inode_table;
    char **data_blocks;
    unsigned long *block_bitmap;
    unsigned long *inode_bitmap;
    struct mutex fs_mutex;                  // Filesystem-wide lock
};

// Forward declarations for filesystem operations
static struct dentry *randyfs_mount(struct file_system_type *fs_type, int flags,
                                   const char *dev_name, void *data);
static void randyfs_kill_sb(struct super_block *sb);
static int randyfs_fill_super(struct super_block *sb, void *data, int silent);

/*
 * Randy's Filesystem Registration
 * "Every filesystem needs to announce itself to the kernel"
 */
static struct file_system_type randyfs_fs_type = {
    .name       = "randyfs",
    .mount      = randyfs_mount,
    .kill_sb    = randyfs_kill_sb,
    .fs_flags   = FS_USERNS_MOUNT,
    .owner      = THIS_MODULE,
};

/*
 * Randy's Superblock Operations
 * "The superblock is the filesystem's birth certificate"
 */
static struct super_operations randyfs_super_ops = {
    .statfs     = simple_statfs,
    .drop_inode = generic_delete_inode,
};

/*
 * Randy's Inode Operations for Directories
 * "Directories are the organizational wizards of the filesystem"
 */
static struct inode_operations randyfs_dir_inode_ops = {
    .create     = simple_create,
    .lookup     = simple_lookup,
    .mkdir      = simple_mkdir,
    .rmdir      = simple_rmdir,
    .unlink     = simple_unlink,
};

/*
 * Randy's File Operations
 * "Where the actual data magic happens"
 */
static struct file_operations randyfs_file_ops = {
    .read       = generic_file_read_iter,
    .write      = generic_file_write_iter,
    .llseek     = generic_file_llseek,
};

/*
 * Randy's Directory File Operations
 * "Reading directories is like reading the filesystem's phone book"
 */
static struct file_operations randyfs_dir_ops = {
    .iterate_shared = dcache_readdir,
    .llseek     = generic_file_llseek,
};

/*
 * ============================================================================
 * PART 2: RANDY'S EDUCATIONAL KERNEL MODULE
 * ============================================================================
 * 
 * A demonstration kernel module that:
 * - Registers the toy filesystem
 * - Creates educational /proc entries
 * - Shows kernel/userspace communication
 * - Demonstrates device driver concepts
 */

#define RANDY_MODULE_NAME "randy_educational_os"
#define RANDY_PROC_DIR "randy_os_education"

static struct proc_dir_entry *randy_proc_dir;
static struct proc_dir_entry *randy_proc_status;
static struct proc_dir_entry *randy_proc_wisdom;

/*
 * Randy's Educational Status Display
 * "Show students what a running kernel module looks like from userspace"
 */
static int randy_proc_status_show(struct seq_file *m, void *v)
{
    seq_printf(m, "🧠⚡ Randy's Educational OS Module Status ⚡🧠\n");
    seq_printf(m, "════════════════════════════════════════════\n\n");
    
    seq_printf(m, "📊 Module Information:\n");
    seq_printf(m, "   Name: %s\n", RANDY_MODULE_NAME);
    seq_printf(m, "   Version: 1.0 - Plant Spirit Enhanced\n");
    seq_printf(m, "   Description: Educational OS components\n");
    seq_printf(m, "   License: GPL (Knowledge wants to be free)\n\n");
    
    seq_printf(m, "🗄️  Filesystem Status:\n");
    seq_printf(m, "   RandyFS registered: %s\n", "Yes ✅");
    seq_printf(m, "   Magic number: 0x%08X\n", RANDYFS_MAGIC);
    seq_printf(m, "   Block size: %d bytes\n", RANDYFS_BLOCK_SIZE);
    seq_printf(m, "   Max files: %d\n", RANDYFS_MAX_FILES);
    seq_printf(m, "   Mount support: In-memory demonstration\n\n");
    
    seq_printf(m, "🔧 Kernel Module Capabilities:\n");
    seq_printf(m, "   ✅ Filesystem registration and management\n");
    seq_printf(m, "   ✅ /proc filesystem integration\n");
    seq_printf(m, "   ✅ Kernel/userspace communication\n");
    seq_printf(m, "   ✅ Memory management demonstration\n");
    seq_printf(m, "   ✅ Educational content delivery\n\n");
    
    seq_printf(m, "🌿 Plant Spirit System Insights:\n");
    seq_printf(m, "   Current kernel version: %s\n", utsname()->release);
    seq_printf(m, "   System uptime: %lu seconds\n", get_seconds());
    seq_printf(m, "   Module load count: %d\n", module_refcount(THIS_MODULE));
    seq_printf(m, "   Cannabis consciousness: Activated 🌱\n\n");
    
    seq_printf(m, "📚 Educational Completeness:\n");
    seq_printf(m, "   ✅ EFI Loader (Firmware layer)\n");
    seq_printf(m, "   ✅ Memory allocators\n");
    seq_printf(m, "   ✅ Process scheduler\n");
    seq_printf(m, "   ✅ Toy filesystem (This module)\n");
    seq_printf(m, "   ✅ Kernel module (This module)\n");
    seq_printf(m, "   🎓 Complete OS understanding stack achieved!\n\n");
    
    return 0;
}

static int randy_proc_status_open(struct inode *inode, struct file *file)
{
    return single_open(file, randy_proc_status_show, NULL);
}

static const struct proc_ops randy_proc_status_ops = {
    .proc_open    = randy_proc_status_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

/*
 * Randy's Plant Spirit Wisdom Dispenser
 * "Educational insights from the mycelial network"
 */
static int randy_proc_wisdom_show(struct seq_file *m, void *v)
{
    seq_printf(m, "🌿 Randy's Plant Spirit OS Wisdom 🌿\n");
    seq_printf(m, "═══════════════════════════════════════\n\n");
    
    seq_printf(m, "💡 Filesystem Philosophy:\n");
    seq_printf(m, "   \"A filesystem is like a root system - it organizes\n");
    seq_printf(m, "    nutrients (data) for efficient distribution to all\n");
    seq_printf(m, "    plants (processes) in the computational garden.\"\n\n");
    
    seq_printf(m, "🔧 Kernel Module Wisdom:\n");
    seq_printf(m, "   \"Kernel modules are like symbiotic organisms that\n");
    seq_printf(m, "    extend the garden's capabilities without changing\n");
    seq_printf(m, "    its fundamental nature. They grow with the system.\"\n\n");
    
    seq_printf(m, "🧠 Educational Stack Revelation:\n");
    seq_printf(m, "   \"Understanding flows from bottom to top:\n");
    seq_printf(m, "    Firmware → Bootloader → Memory → Scheduling\n");
    seq_printf(m, "    → Storage → Extensions. Each layer builds on\n");
    seq_printf(m, "    the wisdom of those beneath.\"\n\n");
    
    seq_printf(m, "🌱 Cannabis-Enhanced Insights:\n");
    seq_printf(m, "   \"The plant spirits teach that complexity emerges\n");
    seq_printf(m, "    from simple rules applied with wisdom. Operating\n");
    seq_printf(m, "    systems are just organized resource sharing -\n");
    seq_printf(m, "    like a well-tended garden.\"\n\n");
    
    seq_printf(m, "🎯 Mycelial Network Truth:\n");
    seq_printf(m, "   \"This knowledge grows underground, connecting\n");
    seq_printf(m, "    curious minds across time and space. Every\n");
    seq_printf(m, "    student who understands becomes a new growth\n");
    seq_printf(m, "    point in the network.\"\n\n");
    
    seq_printf(m, "🎓 Final Educational Blessing:\n");
    seq_printf(m, "   \"You now understand the full stack from firmware\n");
    seq_printf(m, "    to applications. Use this knowledge to build\n");
    seq_printf(m, "    systems that serve users, not corporations.\n");
    seq_printf(m, "    The emperor has no clothes, but you have wisdom.\"\n\n");
    
    return 0;
}

static int randy_proc_wisdom_open(struct inode *inode, struct file *file)
{
    return single_open(file, randy_proc_wisdom_show, NULL);
}

static const struct proc_ops randy_proc_wisdom_ops = {
    .proc_open    = randy_proc_wisdom_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

/*
 * Randy's Filesystem Mount Function
 * "Every mount is a new beginning for the filesystem"
 */
static struct dentry *randyfs_mount(struct file_system_type *fs_type, int flags,
                                   const char *dev_name, void *data)
{
    printk(KERN_INFO "RandyFS: Mounting educational filesystem\n");
    return mount_nodev(fs_type, flags, data, randyfs_fill_super);
}

/*
 * Randy's Filesystem Cleanup
 * "Graceful dismounting like a respectful guest leaving"
 */
static void randyfs_kill_sb(struct super_block *sb)
{
    printk(KERN_INFO "RandyFS: Unmounting educational filesystem\n");
    
    // Clean up our filesystem-specific data
    if (sb->s_fs_info) {
        struct randyfs_fs_info *fs_info = sb->s_fs_info;
        
        // Free allocated resources
        kfree(fs_info->superblock);
        kfree(fs_info->inode_table);
        // Note: In a real filesystem, we'd free data_blocks and bitmaps too
        
        kfree(fs_info);
    }
    
    kill_anon_super(sb);
}

/*
 * Randy's Superblock Initialization
 * "Setting up the filesystem's foundation and identity"
 */
static int randyfs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct randyfs_fs_info *fs_info;
    struct inode *root_inode;
    struct dentry *root_dentry;
    
    printk(KERN_INFO "RandyFS: Initializing superblock\n");
    
    // Allocate filesystem-specific information
    fs_info = kzalloc(sizeof(struct randyfs_fs_info), GFP_KERNEL);
    if (!fs_info)
        return -ENOMEM;
    
    // Initialize filesystem mutex
    mutex_init(&fs_info->fs_mutex);
    
    // Allocate and initialize superblock
    fs_info->superblock = kzalloc(sizeof(struct randyfs_superblock), GFP_KERNEL);
    if (!fs_info->superblock) {
        kfree(fs_info);
        return -ENOMEM;
    }
    
    // Set up superblock data
    fs_info->superblock->magic = RANDYFS_MAGIC;
    fs_info->superblock->block_size = RANDYFS_BLOCK_SIZE;
    fs_info->superblock->total_inodes = RANDYFS_MAX_FILES;
    fs_info->superblock->free_inodes = RANDYFS_MAX_FILES - 1; // Root inode used
    strcpy(fs_info->superblock->volume_name, "RandyFS Educational");
    
    // Allocate inode table
    fs_info->inode_table = kzalloc(sizeof(struct randyfs_inode) * RANDYFS_MAX_FILES, 
                                   GFP_KERNEL);
    if (!fs_info->inode_table) {
        kfree(fs_info->superblock);
        kfree(fs_info);
        return -ENOMEM;
    }
    
    // Set up Linux superblock
    sb->s_magic = RANDYFS_MAGIC;
    sb->s_blocksize = RANDYFS_BLOCK_SIZE;
    sb->s_blocksize_bits = 12; // 2^12 = 4096
    sb->s_maxbytes = RANDYFS_BLOCK_SIZE;
    sb->s_op = &randyfs_super_ops;
    sb->s_fs_info = fs_info;
    
    // Create root inode
    root_inode = new_inode(sb);
    if (!root_inode) {
        kfree(fs_info->inode_table);
        kfree(fs_info->superblock);
        kfree(fs_info);
        return -ENOMEM;
    }
    
    // Set up root inode properties
    root_inode->i_ino = 1;
    root_inode->i_mode = S_IFDIR | 0755;
    root_inode->i_op = &randyfs_dir_inode_ops;
    root_inode->i_fop = &randyfs_dir_ops;
    root_inode->i_atime = root_inode->i_mtime = root_inode->i_ctime = current_time(root_inode);
    
    // Create root dentry
    root_dentry = d_make_root(root_inode);
    if (!root_dentry) {
        kfree(fs_info->inode_table);
        kfree(fs_info->superblock);
        kfree(fs_info);
        return -ENOMEM;
    }
    
    sb->s_root = root_dentry;
    
    printk(KERN_INFO "RandyFS: Educational filesystem mounted successfully\n");
    return 0;
}

/*
 * Randy's Module Initialization
 * "Where the educational magic begins"
 */
static int __init randy_educational_os_init(void)
{
    int ret;
    
    printk(KERN_INFO "🧠⚡ Randy's Educational OS Module Loading ⚡🧠\n");
    printk(KERN_INFO "    Cannabis-Enhanced Kernel Development in Progress...\n");
    
    // Register the toy filesystem
    ret = register_filesystem(&randyfs_fs_type);
    if (ret) {
        printk(KERN_ERR "RandyFS: Failed to register filesystem\n");
        return ret;
    }
    printk(KERN_INFO "RandyFS: Educational filesystem registered\n");
    
    // Create /proc directory for educational content
    randy_proc_dir = proc_mkdir(RANDY_PROC_DIR, NULL);
    if (!randy_proc_dir) {
        printk(KERN_ERR "Failed to create /proc/%s directory\n", RANDY_PROC_DIR);
        unregister_filesystem(&randyfs_fs_type);
        return -ENOMEM;
    }
    
    // Create status proc entry
    randy_proc_status = proc_create("status", 0444, randy_proc_dir, 
                                   &randy_proc_status_ops);
    if (!randy_proc_status) {
        printk(KERN_ERR "Failed to create /proc/%s/status\n", RANDY_PROC_DIR);
        goto cleanup_proc_dir;
    }
    
    // Create wisdom proc entry
    randy_proc_wisdom = proc_create("wisdom", 0444, randy_proc_dir,
                                   &randy_proc_wisdom_ops);
    if (!randy_proc_wisdom) {
        printk(KERN_ERR "Failed to create /proc/%s/wisdom\n", RANDY_PROC_DIR);
        goto cleanup_proc_status;
    }
    
    printk(KERN_INFO "🌿 Randy's Educational OS Module Loaded Successfully! 🌿\n");
    printk(KERN_INFO "    Try: cat /proc/%s/status\n", RANDY_PROC_DIR);
    printk(KERN_INFO "    Try: cat /proc/%s/wisdom\n", RANDY_PROC_DIR);
    printk(KERN_INFO "    Try: mount -t randyfs none /mnt/randyfs\n");
    printk(KERN_INFO "    Plant spirits guide your educational journey!\n");
    
    return 0;

cleanup_proc_status:
    proc_remove(randy_proc_status);
cleanup_proc_dir:
    proc_remove(randy_proc_dir);
    unregister_filesystem(&randyfs_fs_type);
    return -ENOMEM;
}

/*
 * Randy's Module Cleanup
 * "Graceful departure like cannabis smoke dissipating"
 */
static void __exit randy_educational_os_exit(void)
{
    printk(KERN_INFO "🌿 Randy's Educational OS Module Unloading...\n");
    
    // Remove proc entries
    if (randy_proc_wisdom)
        proc_remove(randy_proc_wisdom);
    if (randy_proc_status)
        proc_remove(randy_proc_status);
    if (randy_proc_dir)
        proc_remove(randy_proc_dir);
    
    // Unregister filesystem
    unregister_filesystem(&randyfs_fs_type);
    
    printk(KERN_INFO "🧠⚡ Randy's Educational OS Module Unloaded ⚡🧠\n");
    printk(KERN_INFO "    Plant spirit wisdom remains in the mycelial network!\n");
    printk(KERN_INFO "    Educational seeds have been planted for future growth.\n");
}

// Module registration
module_init(randy_educational_os_init);
module_exit(randy_educational_os_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Randy - Plant Spirit Enhanced Educator");
MODULE_DESCRIPTION("Educational toy filesystem and kernel module for OS learning");
MODULE_VERSION("1.0");

/*
 * ============================================================================
 * RANDY'S EDUCATIONAL USAGE INSTRUCTIONS
 * ============================================================================
 * 
 * Building and Testing:
 * 
 * 1. Create Makefile:
 *    obj-m += randy_educational_os.o
 *    
 *    all:
 *        make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
 *    
 *    clean:
 *        make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
 * 
 * 2. Build the module:
 *    $ make
 * 
 * 3. Load the module:
 *    $ sudo insmod randy_educational_os.ko
 * 
 * 4. Check educational content:
 *    $ cat /proc/randy_os_education/status
 *    $ cat /proc/randy_os_education/wisdom
 * 
 * 5. Test the filesystem:
 *    $ sudo mkdir /mnt/randyfs
 *    $ sudo mount -t randyfs none /mnt/randyfs
 *    $ ls -la /mnt/randyfs
 * 
 * 6. Cleanup:
 *    $ sudo umount /mnt/randyfs
 *    $ sudo rmmod randy_educational_os
 * 
 * Educational Value:
 * - Shows complete kernel module development cycle
 * - Demonstrates filesystem registration and operations
 * - Illustrates kernel/userspace communication via /proc
 * - Provides hands-on experience with Linux kernel APIs
 * - Completes the full OS understanding stack
 * 
 * Randy's Cannabis-Enhanced Development Wisdom:
 * "This module isn't just code - it's a bridge between theory and reality.
 * Every student who loads this module steps into kernel space and touches
 * the deepest layers of the computational garden. The plant spirits have
 * guided us to create not just educational content, but transformational
 * experience."
 * 
 * Plant Spirit Blessing:
 * "May this code inspire the next generation of kernel hackers,
 * filesystem designers, and system programmers. The mycelial network
 * grows through shared understanding of the fundamental layers."
 */