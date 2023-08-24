#include "loader.h"

Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
int fd;//file descriptor 

/*
 * release memory and other cleanups
 */
void loader_cleanup(){
    if(ehdr!=NULL){
        free(ehdr);
        ehdr=NULL;
    }
    if(phdr!=NULL){
        free(phdr);
        phdr=NULL;
    }
    
}
/*
 * Load and run the ELF executable file
 */
void load_and_run_elf(char** argv) {
    fd=open(argv[1], O_RDONLY);
    if(fd<0){
        perror("Error opening file");
        return;
    }
 
// Step 1: Load entire binary content into memory
off_t size = lseek(fd, 0, SEEK_END);
if (size == -1) {
    perror("Error seeking to end of file");
    loader_cleanup();
    return;
}
lseek(fd, 0, SEEK_SET);  // Move the file pointer back to the beginning
Elf32_Ehdr *file_data = malloc(size);
read(fd,ehdr,sizeof(Elf32_Ehdr));
if (!file_data) {
    perror("Error allocating memory for binary content");
    loader_cleanup();
    return;
}
ssize_t bytes_read = read(fd, file_data, size);
if (bytes_read != size) {
    perror("Error reading binary content");
    loader_cleanup();
    return;
}
  // Step 2: Iterate through PHDR table to find PT_LOAD segment
    ehdr = file_data;
    phdr = (Elf32_Phdr *)(file_data + ehdr->e_phoff);
     Elf32_Addr entrypoint = 0;
    
    lseek(fd,ehdr->e_phoff,SEEK_SET);
    read(fd,phdr,sizeof(Elf32_Phdr));
    int pnum=ehdr->e_phnum;
    for (int i = 0; i < pnum; i++){
       
        if (phdr->p_type==PT_LOAD){
            if(ehdr->e_entry>=phdr->p_vaddr&&ehdr->e_entry < phdr->p_vaddr + phdr->p_memsz){
            
            break;}
    }
    lseek(fd, ehdr->e_phoff + i * ehdr->e_phentsize, SEEK_SET);

    read(fd, phdr, ehdr -> e_phentsize);
}

    // Step 3: Allocate memory and copy segment content
    void *loaded_segment = mmap((void*)(phdr->p_vaddr), phdr->p_memsz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_FIXED, fd, phdr->p_offset);
    
    if (loaded_segment == MAP_FAILED) {
        perror("Error mapping memory");
        loader_cleanup();
        return;
    }
    // Step 4: Find entry point within the loaded segment
    int (*start_func)() = (int (*)())(ehdr->e_entry);
    start_func();
    
    // Step 5 and 6: Call _start function
    int ans = start_func();
    printf("User_start return value = %d\n", ans);
}
int main(int argc, char** argv) {
    if (argc != 2) {
        printf( "Usage: %s <path_to_executable>\n", argv[0]);
        return 1;
    }
 // Cleanup
    loader_cleanup();
    load_and_run_elf(argv);
    return 0;
}
