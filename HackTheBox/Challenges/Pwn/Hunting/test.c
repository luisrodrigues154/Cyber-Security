#include <stdlib.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <string.h>
 #include <sys/mman.h>

int32_t set_security()
{
    int16_t var_14 = 0xe;
    if (prctl(0x26, 1, 0, 0, 0) < 0)
    {
        perror("prctl(PR_SET_NO_NEW_PRIVS)");
        exit(2);
    }
    int32_t eax_1 = prctl(0x16, 2, &var_14);
    if (eax_1>= 0)
        return eax_1;
    perror("prctl(PR_SET_SECCOMP)");
    exit(2);
}

int32_t get_address()
{
    int32_t file = open("/dev/urandom", 0);
    int32_t rand_bytes;
    read(file, &rand_bytes, 8);
    close(file);
    srand(rand_bytes);
    int32_t new_addr = 0;
    while ((new_addr < 0x60000000) || (0xf7000000 < new_addr))
        new_addr = rand() << 0x10;
    return new_addr;
}
int32_t main(){
    int32_t addr = get_address();
    
    //signal(0xe, exit)
    //alarm(3)
    int32_t new_mapping = mmap(addr, 0x1000, 3, 0x31, 0xffffffff, 0);
    printf("reached\n");
    if (new_mapping != 0xffffffff){
        puts("1");
        strcpy(new_mapping, "HTB{XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX}");
        puts("2");
        //memset("HTB{XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX}", 0, 0x25);
        puts("3");
        //set_security();
        void * user_input = malloc(0x3c);
        read(0, user_input, 0x3c);
        
        return 0;
    }
    exit(0xffffffff);
}

