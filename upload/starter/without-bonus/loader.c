#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;

int fd;//file descriptor 
int add =0;
// release memory and other cleanups
void loader_cleanup(){
     free(ehdr);
     free(phdr);
}
int check(off_t size){
    if(size==-1){
        return 0;
    }
    else{
        return 1;} 

}
int error_chk(void*loaded_segment){
    if(loaded_segment==MAP_FAILED){
    return 0; }
    else{
        return 1;
    }
}
void chek(int check_working){
     int flag=0;
    if (check_working){
        printf("error");
        flag=1;
    }
}
void allocating(Elf32_Ehdr *file_data ){
    if (!file_data) {
    perror("Error allocating memory for file content");
    loader_cleanup();
    return;
}

}

 // Load and run the ELF executable file
void load_and_run_elf(char** argv) {
    fd=open(argv[1], O_RDONLY);
    if(fd<0){
        perror("Error opening file");
        return;
    }
// Step 1: Load entire file content into memory
off_t size = lseek(fd, 0, SEEK_END);
int flag=check(size);
if (flag == 0) {
    perror("Error during lseek execution");
    loader_cleanup();
    return;
}
Elf32_Shdr *shdr_table;


// Move the file pointer back to the beginning
lseek(fd, 0, SEEK_SET); 


Elf32_Ehdr *file_data = malloc(size);
read(fd,ehdr,sizeof(Elf32_Ehdr));
allocating(file_data);

ssize_t bytes_read = read(fd, file_data, size);
if (bytes_read != size) {
    perror("Error reading  the file content");
    loader_cleanup();
    return;
}
  // Step 2: Iterating  through PHDR table to find PT_LOAD segment
    ehdr = file_data;
    phdr = (Elf32_Phdr *)(file_data + ehdr->e_phoff);
     Elf32_Addr entrypoint = 0;
    
    lseek(fd,ehdr->e_phoff,SEEK_SET);
    read(fd,phdr,sizeof(Elf32_Phdr));
    int pnum=ehdr->e_phnum;
    shdr_table = (Elf32_Shdr *)(file_data + ehdr->e_shoff);
   int i = 0;
   int pt=0;
while (i < pnum) {
    if (phdr->p_type == PT_LOAD) {
        // Step 4: Find entry point within the loaded segment
        if (ehdr->e_entry >= phdr->p_vaddr ){
            if(ehdr->e_entry < phdr->p_vaddr + phdr->p_memsz){
            break;
        }
        }
    }
    lseek(fd, ehdr->e_phoff + i * ehdr->e_phentsize, SEEK_SET);
    read(fd, phdr, ehdr->e_phentsize);
    chek(pt);

    i++;
}
    // Step 3: Allocate memory and copy segment content
    void *loaded_segment = mmap((void*)(phdr->p_vaddr), phdr->p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_FIXED, fd, phdr->p_offset);
    int l=error_chk(loaded_segment);
    if (loaded_segment == MAP_FAILED) {
        perror("Error while mapping the  memory");
        loader_cleanup();
       
        return;
    }
    int num=ehdr->e_phnum;
     while(num>0){int ptd=0;chek(ptd);num--; }


//type casting  
    int (*start_func)() = (int (*)())(ehdr->e_entry);
   
    start_func();
    
    // Step 5 and 6: Call _start function
    int result = start_func();
    //printing the ans
    printf("user _start return value  = %d\n", result);
}
int main(int argc, char** argv) {
    if (argc != 2) {
        printf( "Usage: %s <path_to_executable>\n", argv[0]);
        exit(1);
    }

 // Cleanup
    loader_cleanup();
    load_and_run_elf(argv);
//checking the valid elf or not
int flag=0;
chek(add);
    if(ehdr->e_ident[EI_MAG0]==ELFMAG0){
        if(ehdr->e_ident[EI_MAG1]==ELFMAG1){
            if(ehdr->e_ident[EI_MAG2]==ELFMAG2){
                if(ehdr->e_ident[EI_MAG3]==ELFMAG3){
            flag=1;
        } 
        }
        }
    }
    if(flag==1){
        printf("valid elf");
    }
    else{
        printf("invalid elf");
    }
    return 0;
}
