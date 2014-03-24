#include <linux/unistd.h>
#include <stdio.h>
#include <stdlib.h>			// for malloc, free

int m_data = 10;			// data site

int main(int argc, const char *argv[])
{
	unsigned long vmbuff[7];	// save vm information 
	int pid;
	char m_stack;			// stack site
	char *m_heap = (char *)malloc(sizeof(char)*10);		// heap site
	pid = getpid();
	syscall(__NR_add, pid, vmbuff);	// add system function call
	//------------------- Print vm information -------------------//
	printf("2008720061, Jang, seung-yong\n");
	printf("Code  : %p(%lx~%lx)\n", &main, vmbuff[0], vmbuff[1]);
	printf("Data  : %p(%lx~%lx)\n", &m_data, vmbuff[2], vmbuff[3]);
	printf("Heap  : %p(%lx~%lx)\n", &(*m_heap), vmbuff[4], vmbuff[5]);
	printf("Stack : %p(%lx~)\n", &m_stack, vmbuff[6]);
	//------------------- Print vm information -------------------//

	free(m_heap);	// deallocation
	return 0;
}
