#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

// Get file name from args
char* extract_filename(char *full_path)
{
    char *filename;
    for (filename = full_path + strlen(full_path); filename >= full_path && *filename != '/'; filename--)
        ;
    return filename + 1;
}

// Search target
void recursive_search(char *directory_path, char *target_filename) {
    char path_buffer[512], *path_end;
    int dir_fd;
    struct dirent dir_entry;
    struct stat path_stat;

    // Check target
    if (strcmp(extract_filename(directory_path), target_filename) == 0) {
        printf("%s\n", directory_path);
    }

    // Open folder
    if ((dir_fd = open(directory_path, O_RDONLY)) < 0) {
        fprintf(2, "recursive_search: cannot open [%s], fd=%d\n", directory_path, dir_fd);
        return;
    }

    // Get path
    if (fstat(dir_fd, &path_stat) < 0) {
        fprintf(2, "recursive_search: cannot stat %s\n", directory_path);
        close(dir_fd);
        return;
    }

    // Check folder
    if (path_stat.type != T_DIR) {
        close(dir_fd);
        return;
    }

    // Check path
    if (strlen(directory_path) + 1 + DIRSIZ + 1 > sizeof(path_buffer)) {
        printf("recursive_search: path too long\n");
        close(dir_fd);
        return;
    }
    strcpy(path_buffer, directory_path);
    path_end = path_buffer + strlen(path_buffer);
    *path_end++ = '/';
    
    // Read each folder
    while (read(dir_fd, &dir_entry, sizeof(dir_entry)) == sizeof(dir_entry)) {
        if (dir_entry.inum == 0)
            continue;
        
        memmove(path_end, dir_entry.name, DIRSIZ);
        path_end[DIRSIZ] = 0;
        
        // Ignore folder "." and ".."
        if (strcmp(dir_entry.name, ".") == 0 || strcmp(dir_entry.name, "..") == 0)
            continue;
        
        // Recursive search
        recursive_search(path_buffer, target_filename);
    }
    close(dir_fd);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(2, "usage: recursive_search <path> <filename>\n");
        exit(1);
    }

    recursive_search(argv[1], argv[2]);
    exit(0);
}