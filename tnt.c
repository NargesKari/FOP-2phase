#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include <dirent.h>
//#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
//#include <sys/types.h>
#include <pwd.h>
//#define MAX_FILENAME_LENGTH 1000
//#define MAX_COMMIT_MESSAGE_LENGTH 2000
//#define MAX_LINE_LENGTH 1000
//#define MAX_MESSAGE_LENGTH 1000
//#define debug(x) printf("%s", x);
char cwd[1024];
char tnt_path[1024];
//void print_command(int argc, char * const argv[]);
int run_init(int argc, char * const argv[]);
int create_configs();
//int run_add(int argc, char * const argv[]);
//int add_to_staging(char *filepath);
//int run_reset(int argc, char * const argv[]);
//int remove_from_staging(char *filepath);
//int run_commit(int argc, char * const argv[]);
//int inc_last_commit_ID();
//bool check_file_directory_exists(char *filepath);
//int commit_staged_file(int commit_ID, char *filepath);
//int track_file(char *filepath);
//bool is_tracked(char *filepath);
//int create_commit_file(int commit_ID, char *message);
//int find_file_last_commit(char* filepath);
//int run_checkout(int argc, char *const argv[]);
//int find_file_last_change_before_commit(char *filepath, int commit_ID);
//int checkout_file(char *filepath, int commit_ID);
int find_tnt(){
    char tmp_cwd[1024];
    struct dirent *entry;
    do {
        // find .tnt
        DIR *dir = opendir(".");
        if (dir == NULL) {
            perror("Error opening current directory");
            return 1;
        }
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".tnt") == 0){
                getcwd(tnt_path, sizeof(tnt_path));
                strcat(tnt_path,"/.tnt");
            }
        }
        closedir(dir);
        if (getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL) return 1;     // update current working directory
        if (strcmp(tmp_cwd, "/") != 0) {        // change cwd to parent
            if (chdir("..") != 0) return 1;
        }
    } while (strcmp(tmp_cwd, "/") != 0);
    if (chdir(cwd) != 0) return 1;  // return to the initial cwd
    return 0;
}
int run_init(int argc, char * const argv[]) {
    if(find_tnt()) return 1;
    if (tnt_path[0] == 0) {
        if (mkdir(".tnt", 0755) != 0) return 1;
        if (chdir(".tnt") != 0) return 1;
        getcwd(tnt_path, sizeof(tnt_path));
        return create_configs();
    } else perror("tnt repository has already initialized");
    return 0;
}
int create_configs(){
    FILE *file;
    struct passwd *pw = getpwuid(getuid());
    if (chdir(pw->pw_dir) != 0) return 1;
    if(access(".tnt_global_configs", F_OK) == -1){
        printf("ye");
        file = fopen(".tnt_global_config", "w");
        fprintf(file,"user name:0\ndate and time:0\nuser email:0\ndate and time:0\nalias:\n");
        fclose(file);
    }
    printf("finish access\n");
    if (chdir(tnt_path) != 0) return 1;
    file = fopen("tracks", "w");
    fclose(file);
    file = fopen("config", "w");
    fprintf(file,"user name:0\ndate and time:0\nuser email:0\ndate and time:0\n");
    fprintf(file,"last commit ID:0\ncurrent commit ID:0\nbranch:master");
    fclose(file);
    file = fopen("branches", "w");
    fprintf(file,"master-0-");
    fclose(file);
    if (mkdir("COMMIT", 0755) != 0) return 1;
    if (mkdir("STAGE", 0755) != 0) return 1;
    if (chdir("STAGE") != 0) return 1;
    file = fopen("add history", "w");
    fclose(file);
    if (chdir("..") != 0) return 1;
    if (chdir("COMMIT") != 0) return 1;
    file = fopen("commits", "w");
    fclose(file);
    return  0;
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stdout, "please enter a valid command");
        return 1;
    }
    if (getcwd(cwd, sizeof(cwd)) == NULL) return 1;
    else if (strcmp(argv[1], "init") == 0){
        return run_init(argc, argv);
    } /*else if (strcmp(argv[1], "add") == 0){
        return run_add(argc, argv);
    } else if (strcmp(argv[1], "reset") == 0) {
        return run_reset(argc, argv);
    } else if (strcmp(argv[1], "commit") == 0) {
        return run_commit(argc, argv);
    } else if (strcmp(argv[1], "checkout") == 0) {
        return run_checkout(argc, argv);
    }*/

    return 0;
}