#include <linux/module.h>
#include <linux/highmem.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>		// for copy_to_user

#define __NR_add	351		// define sys_call number

unsigned long k_vmbuff[7];		// for return data 

void **syscall_table = (void**)SYSCALL_TABLE;	// SYSCALL TABLE pointer

/* sys_open function save pointer */
asmlinkage int (*real_add)(int, int);	

/* function made by me */
asmlinkage void my_add(int pid, unsigned long vmbuff[])
{
	struct task_struct *cur_task = pid_task(find_vpid(pid), PIDTYPE_PID);
	// get current task_struct
	struct mm_struct *cur_mm = get_task_mm(cur_task);
	// get current task_struct member mm_struct (use count +1)

	//-------------------- get vm information --------------------//
	k_vmbuff[0] = cur_mm->start_code;	k_vmbuff[1] = cur_mm->end_code;
	k_vmbuff[2] = cur_mm->start_data;	k_vmbuff[3] = cur_mm->end_data;
	k_vmbuff[4] = cur_mm->start_brk;	k_vmbuff[5] = cur_mm->brk;
	k_vmbuff[6] = cur_mm->start_stack;
	//-------------------- get vm information --------------------//

	copy_to_user(vmbuff, k_vmbuff, 7*sizeof(unsigned long));
	// copy to user
	mmput(cur_mm);
	// use count -1
}

/* Make the page writable */
void make_rw(void *address)
{
	unsigned int level;
	pte_t *pte = lookup_address((unsigned long)address, &level);
	if(pte->pte &~ _PAGE_RW)
	pte->pte |= _PAGE_RW;
}

/* Make the page write protected */
void make_ro(void *address)
{
	unsigned int level;
	pte_t *pte = lookup_address((unsigned long)address, &level);
	pte->pte = pte->pte &~ _PAGE_RW;
}

/* init function */
int hooking_init(void)
{
	make_rw(syscall_table);
	// syscall table swap
	real_add = syscall_table[__NR_add];
	syscall_table[__NR_add] = my_add;
	return 0;
}

/* exit function */
void hooking_exit(void)
{
	// syscall table swap
	syscall_table[__NR_add] = real_add;
	make_ro(syscall_table);
}

module_init(hooking_init);	// module_init function
module_exit(hooking_exit);	// module_exit function
MODULE_LICENSE("GPL");
