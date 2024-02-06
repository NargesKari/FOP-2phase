#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
char cwd[1024];
char tnt_path[1024];
char Time[100];
char history_path[2048];
char tracks_path[2048];
char config_path[2048];
char commits_path[2048];
char shortcuts_path[2048];
char branches_path[2048];
int Last_ID, Current_ID;
char Branch[256];
//void print_command(int argc, char * const argv[]);
int compareTimes(const char *timeStr1, const char *timeStr2);
int run_init(int argc, char * const argv[]);
int create_configs();
int run_config(int argc, char * const argv[]);
void make_space(char str[],long targetLength);
int check_command(char *command);
int manage_add_mode(int argc, char * const argv[]);
void run_add(char * path);
int file_or_directory(char * path);
int add_to_staging(char * path, int mode);
int add_to_tracking(char * path);
void delete_from_file(char* str, char *file_path);
void exploreDirectory(const char *path, long depth);
void copyFile(const char *sourcePath, const char *destinationPath);
bool matchWildcard(const char *word, const char *pattern);
int manage_reset_mode(int argc, char * const argv[]);
int run_reset(char * str);
int reset_by_time(long line_numb);
int run_status(int argc, char * const argv[]);
void saveLastModifiedTime(char *filename,char* time);
int run_commit(int argc, char * const argv[]);
void current_name_email(char name[], char email[]);
void get_data(int mode);
int count_changed_files();
int save_commit();
int check_stage(int *ID, int *status, char *path);
int last_version(int code, char *path);
void clear_stage();
int set_shortcut(int argc, char *argv[]);
int replace_shortcut(int argc, char *argv[]);
int remove_shortcut(int argc, char *argv[]);
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
    sprintf(history_path,"%s/STAGE/history", tnt_path);
    sprintf(tracks_path,"%s/tracks", tnt_path);
    sprintf(config_path,"%s/config", tnt_path);
    sprintf(commits_path,"%s/COMMIT/commits", tnt_path);
    sprintf(shortcuts_path,"%s/COMMIT/shortcuts", tnt_path);
    sprintf(branches_path,"%s/branches", tnt_path);
    if (chdir(cwd) != 0) return 1;  // return to the initial cwd
    return 0;
}
int run_init(int argc, char * const argv[]) {
    if(argc > 2 ) {
        fprintf(stdout, "please enter a valid command\n");
        return 1;
    }
    if(find_tnt()) return 1;
    if (tnt_path[0] == 0) {
        if (mkdir(".tnt", 0755) != 0) return 1;
        if (chdir(".tnt") != 0) return 1;
        getcwd(tnt_path, sizeof(tnt_path));
        return create_configs();
    } else perror("tnt repository has already initialized\n");
    return 0;
}
int create_configs(){
    char Name[256],Date_Time[256],Email[256],last_ID[256],current_ID[256],branch[256];
    strcpy(Name,"user name:");
    make_space(Name,100);
    strcpy(Date_Time,"date and time:");
    make_space(Date_Time,100);
    strcpy(Email,"user email:");
    make_space(Email,100);
    strcpy(last_ID,"last commit ID:0");
    make_space(last_ID,100);
    strcpy(current_ID,"current commit ID:0");
    make_space(current_ID,100);
    strcpy(branch,"Branch:master");
    make_space(branch,100);
    char space[40];
    strcpy(space,"                                   ");
    FILE *file;
    struct passwd *pw = getpwuid(getuid());
    if (chdir(pw->pw_dir) != 0) return 1;   //go to user folder
    if(access(".tnt_global_configs", F_OK) == -1){   //make global configs' place
        file = fopen(".tnt_global_configs", "w");
        fprintf(file, "%s\n%s\n%s\n%s\nalias:\n", Name, Date_Time, Email, Date_Time);
        fclose(file);
    }
    if (chdir(tnt_path) != 0) return 1;
    file = fopen("tracks", "w");
    fclose(file);
    file = fopen("config", "w");
    fprintf(file, "%s\n%s\n%s\n%s\nalias:\n", Name, Date_Time, Email, Date_Time);
    fprintf(file,"%s\n%s\n%s\nalias:\n", last_ID, current_ID, branch);
    fclose(file);
    file = fopen("branches", "w");
    strcpy(branch,"master:0");
    make_space(branch,100);
    fprintf(file,"%s\n", branch);
    fclose(file);
    if (mkdir("COMMIT", 0755) != 0) return 1;
    if (mkdir("STAGE", 0755) != 0) return 1;
    if (chdir("STAGE") != 0) return 1;
    file = fopen("history", "w");
    fclose(file);
    if (chdir("..") != 0) return 1;
    if (chdir("COMMIT") != 0) return 1;
    file = fopen("commits", "w");
    fclose(file);
    file = fopen("shortcuts", "w");
    fclose(file);
    return  0;
}
int run_config(int argc, char * const argv[]){
    char Name[256],Date_Time[256],Email[256];
    strcpy(Name,"user name:");
    make_space(Name,100);
    strcpy(Date_Time,"date and time:");
    make_space(Date_Time,100);
    strcpy(Email,"user email:");
    make_space(Email,100);
    int m=0;
    FILE *file;
    char buffer[512];
    char data[256];
    if(argc < 3 ){
        fprintf(stdout, "please enter a valid command\n");
        return 1;
    } if(strcmp(argv[2], "-global") == 0) {
        m = 1;
        struct passwd *pw = getpwuid(getuid());
        if (chdir(pw->pw_dir) != 0) return 1;
        if (access(".tnt_global_configs", F_OK) == -1) {   //make global configs' place
            file = fopen(".tnt_global_configs", "w");
            fprintf(file, "%s\n%s\n%s\n%s\nalias:\n", Name, Date_Time, Email, Date_Time);
            fclose(file);
        }
    }else {
        if (find_tnt() == 1 || tnt_path[0] == 0) {
            fprintf(stdout, "There is not any repository\n");
            return 1;
        }
        if (chdir(tnt_path) != 0) return 1;
    }
    if(argc < 3+m ){
        fprintf(stdout, "please enter a valid command\n");
        return 1;
    }
    if(strcmp(argv[2+m], "user.name") == 0){
        if(m) file = fopen(".tnt_global_configs","r+");
        else if((file = fopen("config","r+")) == NULL) return 1;
        if(argc < 4+m ){
            fprintf(stdout, "please enter a valid command\n");
            return 1;
        }if(strlen(argv[3+m])>30 || argc > 4+m){
            fprintf(stdout, "Your username can has a maximum of 30 characters without space\n");
            return 1;
        }
        sprintf(data, "user name:%s ", argv[3+m]);
        fputs( data, file);
        if (fgets(buffer, sizeof(buffer), file) == NULL) return 1;
        sprintf(data, "date and time:%s ", Time);
        fputs( data, file);
        fclose(file);
    }else if(strcmp(argv[2+m], "user.email") == 0) {
        if(m) file = fopen(".tnt_global_configs","r+");
        else if((file = fopen("config","r+")) == NULL) return 1;
        if (argc < 4+m) {
            fprintf(stdout, "please enter a valid command\n");
            return 1;
        }if (strlen(argv[3+m]) > 30 || argc > 4+m) {
            fprintf(stdout, "Your email can has a maximum of 30 characters without space\n");
            return 1;
        }
        if (fgets(buffer, sizeof(buffer), file) == NULL) return 1;
        if (fgets(buffer, sizeof(buffer), file) == NULL) return 1;
        sprintf(data, "user email:%s ", argv[3+m]);
        fputs(data, file);
        if (fgets(buffer, sizeof(buffer), file) == NULL) return 1;
        sprintf(data, "date and time:%s ", Time);
        fputs(data, file);
        fclose(file);
    }else if(strncmp(argv[2+m], "alias.",6) == 0){
        char command[256];
//        if(!check_command(argv[3+m])){      or: if(system(argv[3+m])!=0)
//            printf("%s\n",argv[3+m]);
//            return 1;
//        }
        sprintf(command,"Command:%s",argv[3+m]);
        make_space(command,100);
        strcat(command,"\n");
        if(m) {
            if((file = fopen(".tnt_global_configs","r+")) == NULL) {
                printf("Error we lost tnt_global_configs\n"); return 1;
            }
        }
        else if((file = fopen("config","r+")) == NULL) return 1;
        char name[256];
        sprintf(name,"Name:%s",strstr(argv[2+m], ".")+1);
        make_space(name,100);
        strcat(name,"\n");
        char line[256];
        while(fgets(line, sizeof(line), file) != NULL){
            if(strcmp(line, name) == 0){
                fputs(command,file);
                fprintf(file,"Time:%s\n", Time);
                return 0;
            }
        }
        if(m) file = fopen(".tnt_global_configs","a");
        else file = fopen("config","a");
        fprintf(file,"%s%sTime:%s\n",name,command,Time);
        fclose(file);
        return 0;
    }else{
        fprintf(stdout, "please enter a valid command\n");
        return 1;
    }
    return  0;
}
int check_command(char *command){
    int result = system(command);
    if (WIFEXITED(result) && WEXITSTATUS(result) == 0) return 1;
    return 0;
}
void make_space(char str[],long targetLength){
    long currentLength = strlen(str);
    if (currentLength < targetLength) {
        long numSpacesToAdd = targetLength - currentLength;
        for (int i = 0; i < numSpacesToAdd; ++i) {
            strcat(str, " ");
        }
    }
}
int file_or_directory(char * path){
    struct stat file_info;
    if (lstat(path, &file_info) == -1) {
        return 0;
    }
    if (S_ISREG(file_info.st_mode)) {   //file
        return 1;
    } else if (S_ISDIR(file_info.st_mode)) {  //directory
        return 2;
    }else {
        printf("Input is neither a regular file nor a directory.\n");
        return 0;
    }
}
int manage_add_mode(int argc, char * const argv[]){
    if(argc < 3 ) {
        fprintf(stdout, "please enter a valid command\n");
        return 1;
    }if(strcmp(argv[2], "-redo") == 0){
        FILE *history;
        history= fopen(history_path,"r+");
        char line[512];
        while(fgets(line, sizeof(line), history) != NULL){
            if(strstr(line, "STAGE:0") != NULL){
                long offset = ftell(history);
                fseek(history, offset-8, SEEK_SET);
                fputs("STAGE:1\n", history);
            }
        }
        return 0;
    }else if( strstr(argv[2],"*")!= NULL){
        DIR *dir = opendir(".");
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (matchWildcard(entry->d_name, argv[2])){
                char path[2048];
                sprintf(path, "%s/%s", cwd, entry->d_name);
                run_add(path);
            }
        }
    }else if(strcmp(argv[2], "-f")==0) {
        if(argc < 4 ) printf( "please enter a valid command\n");
        for (int i = 3; i < argc; i++) {
            run_add(argv[i]);
        }return 0;
    }else if(strcmp(argv[2], "-n") == 0){
        if(argc < 4 ){
            printf("please enter a valid command\n");
            return 1;
        }
        char *check;
        long depth= strtol(argv[3], &check,10);
        if( *check != '\0' || depth < 1){
            printf( "please enter a valid command\n");
            return 1;
        }
        exploreDirectory(cwd, depth);
    }else if(argv[2][0] != '-') {
        if(strstr(argv[2],"/") == NULL){
            char path[2048];
            sprintf(path, "%s/%s", cwd, argv[2]);
            run_add(path);
        } else run_add(argv[2]);
    }else{
        fprintf(stdout, "please enter a valid command\n");
        return 1;
    }
    return 0;
}
void exploreDirectory(const char *path, long depth) {
    DIR *directory = opendir(path);
    if (depth == 0 || directory == NULL) return;
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
        if (strncmp(entry->d_name, ".",1) == 0 ) continue;
        char address[PATH_MAX];
        strcpy(address,"");
        sprintf(address,  "%s/%s", path, entry->d_name);
        run_add(address);
        if (entry->d_type == DT_DIR) exploreDirectory(address, depth - 1);
    }
    closedir(directory);
}
void run_add(char * path){
    char buffer[512];
    char path1[512];
    int check= file_or_directory(path);
    if(check == 0){
        int check1=0;
        FILE *file = fopen(tracks_path, "r+");
        while (fscanf(file, "%s", buffer) == 1) {
            if (strstr(buffer, path) != NULL && strstr(buffer, "STATUS:1") != NULL) {
                check1=1;
                add_to_staging(buffer,2);
            }
        }
        if(!check1) printf("Error while getting %s information\n",path);
    }
    if(check == 1){
        if(add_to_tracking(path)) return;
        add_to_staging(path, 1);
    }
    else if(check == 2){
        char tracks_copy[2048];
        sprintf(tracks_copy,"%s/tracks_copy",tnt_path);
        copyFile(tracks_path,tracks_copy);
        FILE *file = fopen(tracks_copy, "r+");
        DIR *dir = opendir(path);
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type != DT_DIR){
                strcpy(path1,"");
                sprintf(path1,"%s/%s",path,entry->d_name);
                if(add_to_tracking(path1)) return;
                add_to_staging(path1, 1);
                delete_from_file(path1,tracks_copy);
            }
        }while (fscanf(file, "%s", buffer) == 1) {
            if (strstr(buffer, path) != NULL || strstr(buffer, "STATUS:0") != NULL) {
                add_to_staging(buffer,2);
            }
        }
    }
}
void delete_from_file(char* str, char *file_path){
    char buffer[512];
    FILE *file = fopen(file_path, "r+");
    while (fscanf(file, "%s", buffer) == 1) {
        if (strstr(buffer, str) != NULL) {
            unsigned long offset = ftell(file);
            fseek(file, offset-strlen(buffer), SEEK_SET);
            for(int i=0; i< strlen(buffer); i++){
                fputs(" ", file);
            }
        }
    }
}
int add_to_tracking(char * path){
    FILE *file = fopen(tracks_path, "a+");
    if (file == NULL) {
        printf("Error in opening tracks file!\n");
        return 1;
    }
    int CODE=0;
    char line[512];
    while(fgets(line, sizeof(line), file) != NULL){
        if(strstr(line,path)!=NULL) return 0;
        if(strstr(line, "CODE:") != NULL) sscanf(strstr(line, "CODE:") ,"CODE:%d", &CODE);
    }
    char buffer[1000];
    while (fscanf(file, "%s", buffer) == 1) {
        if (strcmp(buffer, path) == 0) {
            fclose(file);
            return 0;
        }
    }
    fprintf(file, "%s STATUS:1 CODE:%d\n", path, ++CODE);
    fclose(file);
    return 0;
}
int add_to_staging(char *path, int mode){    //mode1: file added or modified... mode2: file deleted
    int ID=0;
    FILE *history;
    history= fopen(history_path,"r+");
    char line[512];
    char check_path[512];
    sprintf(check_path,"PATH:%s\n",path);
    while(fgets(line, sizeof(line), history) != NULL){
        if(strcmp(check_path, line)==0) fputs("STAGE:2\n", history);
        if(strstr(line, "ID:") != NULL) sscanf(line ,"ID:%d", &ID);
    }
    fclose(history);
    if(mode==1){
        char destinationPath[2048];
        sprintf(destinationPath,"%s/STAGE/%d", tnt_path, ID+1);
        copyFile(path, destinationPath);
    }
    history= fopen(history_path,"a");
    char new_data[1024];
    sprintf(new_data,"Time:%s\nPATH:%s\nSTAGE:1\nSTATUS:%d\nID:%d\n", Time, path, mode, ID+1);
    fputs(new_data, history);
    fclose(history);
    return 0;
}
void copyFile(const char *sourcePath, const char *destinationPath) {
    FILE *sourceFile = fopen(sourcePath, "rb");
    if (sourceFile == NULL) {
        printf("Error opening source file:%s\n",sourcePath);
        return;
    }
    FILE *destinationFile = fopen(destinationPath, "wb");
    if (destinationFile == NULL) {
        perror("Error opening destination file");
        fclose(sourceFile);
        return;
    }
    const size_t bufferSize = 10000;
    char buffer[bufferSize];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, bufferSize, sourceFile)) > 0) {
        fwrite(buffer, 1, bytesRead, destinationFile);
    }
    fclose(sourceFile);
    fclose(destinationFile);
}
bool matchWildcard(const char *word, const char *pattern) {
    if (*pattern == '\0') {
        return *word == '\0';
    }
    if (*pattern == '*') {
        return matchWildcard(word, pattern + 1) || (*word != '\0' && matchWildcard(word + 1, pattern));
    }
    return (*word != '\0' && (*word == *pattern || *pattern == '?')) && matchWildcard(word + 1, pattern + 1);
}
int manage_reset_mode(int argc, char * const argv[]){
    if(argc < 3 ){
        fprintf(stdout, "please enter a valid command\n");
        return 1;
    }else if( strstr(argv[2],"*")!= NULL){
        DIR *dir = opendir(".");
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (matchWildcard(entry->d_name, argv[2])){
                char path[2048];
                sprintf(path, "%s/%s", cwd, entry->d_name);
                run_reset(path);
            }
        }
    }else if(strcmp(argv[2], "-f")==0) {
        if(argc < 4 ) fprintf(stdout, "please enter a valid command\n");
        for (int i = 3; i < argc; i++) {
            run_reset(argv[i]);
        }return 0;
    }else if(argv[2][0] != '-') {
        run_reset(argv[2]);
        return 0;
    }else if(strcmp(argv[2], "-undo") == 0){
        long count_line=0;
        if(argc < 4 ){
            fprintf(stdout, "please enter a valid command\n");
            return 1;
        }
        char *check;
        long numb= strtol(argv[3], &check,10);
        if( *check != '\0' || numb< 1){
            fprintf(stdout, "please enter a valid command\n");
            return 1;
        }
        FILE *history;
        history= fopen(history_path,"r");
        char line[512];
        while(fgets(line, sizeof(line), history) != NULL){
            count_line++;
        }
        long line_numb = count_line-2;
        for (int i = 0; i < numb; ++i) {
            line_numb=reset_by_time(line_numb);
            line_numb-=5;
        }
    }else{
        fprintf(stdout, "please enter a valid command\n");
        return 1;
    }
    return 0;
}
int run_reset(char * str){
    int check=1;
    FILE *history;
    history= fopen(history_path,"r+");
    char line[512];
    while(fgets(line, sizeof(line), history) != NULL){
        if(strstr(line, str) != NULL){
            check=0;
            if(fgets(line, sizeof(line), history) != NULL){
                if(strstr(line,"STAGE:1") != NULL){
                    long offset = ftell(history);
                    fseek(history, offset-8, SEEK_SET);
                    fputs("STAGE:0\n", history);
                }
            }
        }
    }
    if(check) printf("%s is not in add's history\n",str);
    return 0;
}
int reset_by_time(long line_numb){
    int first=0;
    char time_line[512];
    char line[512];
    FILE *history= fopen(history_path,"r");
    while(fgets(time_line, sizeof(time_line), history) != NULL){
        line_numb--;
        if(line_numb == 0) break;
    }
    fclose(history);
    history= fopen(history_path,"r+");
    while(fgets(line, sizeof(line), history) != NULL){
        first++;
        if(strstr(line, time_line) != NULL){
            while(fgets(line, sizeof(line), history) != NULL){
                if(strstr(line, "STAGE:1") != NULL){
                    long offset = ftell(history);
                    fseek(history, offset-8, SEEK_SET);
                    fputs("STAGE:0\n", history);
                }
            }
            return first;
        }
    }
    return 0;
}
int Alias(int argc, char *argv[]){
    char Command_global[256],Time_global[256];
    char Command_local[256],Time_local[256];
    char line[512], name[256];
    int check1=0,check2=0;
    sprintf(name, "Name:%s ",argv[1]);
    FILE *global_config, *local_config;
    struct passwd *pw = getpwuid(getuid());
    if (chdir(pw->pw_dir) != 0) return 1;
    if ((global_config = fopen(".tnt_global_configs", "r")) == NULL)
        printf("We lost global configs!");
    else{
        while(fgets(line, sizeof(line), global_config) != NULL){
            if(strstr(line,name)!= NULL){
                check1=1;
                fgets(line, sizeof(line), global_config);
                sscanf(line, "Command:%[^\n]",Command_global);
                fgets(line, sizeof(line), global_config);
                sscanf(line, "Time:%s",Time_global);
            }
        }
        fclose(global_config);
    }
    if (chdir(tnt_path) != 0) return 1;
    if((local_config = fopen("config","r")) == NULL) return 1;
    while(fgets(line, sizeof(line), local_config) != NULL){
        if(strstr(line,name)!= NULL){
            check2=1;
            fgets(line, sizeof(line), local_config);
            sscanf(line, "Command:%[^\n]",Command_local);
            fgets(line, sizeof(line), local_config);
            sscanf(line, "Time:%s",Time_local);
        }
    }
    fclose(global_config);
    if( check1 == 0 && check2 != 0)
        system(Command_local);
    else if(check1 != 0 && check2 == 0)
        system(Command_global);
    else if(check1 * check2 != 0){
        if(compareTimes(Time_local, Time_global) > 0) system(Command_local);
        else system(Command_global);
    }else{
        printf("please enter a valid command\n");
        return 1;
    }
    return 0;
}
int compareTimes(const char *timeStr1, const char *timeStr2){
    struct tm tm1, tm2;
    time_t time1, time2;
    if (sscanf(timeStr1, "%d-%d-%d/%d:%d:%d",
               &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday,
               &tm1.tm_hour, &tm1.tm_min, &tm1.tm_sec) != 6) {
        return 0;
    }
    if (sscanf(timeStr2, "%d-%d-%d/%d:%d:%d",
               &tm2.tm_year, &tm2.tm_mon, &tm2.tm_mday,
               &tm2.tm_hour, &tm2.tm_min, &tm2.tm_sec) != 6) {
        return 0;
    }
    tm1.tm_year -= 2000;
    tm1.tm_mon -= 1;
    tm2.tm_year -= 2000;
    tm2.tm_mon -= 1;
    time1 = mktime(&tm1);
    time2 = mktime(&tm2);
    if (time1 < time2) return -1;
    else if (time1 > time2) return 1;
    return 0;
}
int run_status(int argc, char * const argv[]){
    get_data(0);
    char last_commit_path[4096],copy_path[40976],line[256],last_time[256];
    sprintf(last_commit_path,"%s/COMMIT/commit_%d/detail",tnt_path, Last_ID);
    sprintf(copy_path,"%s/copy",tnt_path);
    copyFile(last_commit_path,copy_path);
    FILE *commits = fopen(commits_path,"r");
    while(fgets(line, sizeof(line), commits) != NULL){
        if(strstr(line, "Date & Time:") != NULL){
            sscanf(line, "Date & Time:%s", last_time);
        }
    }
    fclose(commits);
    int a,b,check;
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_DIR){
            check=0;
            char path[2048];
            sprintf(path, "%s/%s", cwd, entry->d_name);
            commits=fopen(last_commit_path,"r");
            while(fgets(line, sizeof(line), commits) != NULL){
                if(strstr(line, path) != NULL){
                    check=1;
                    char time[128];
                    saveLastModifiedTime(path,time);
                    if(compareTimes(time,last_time)>0){
                        if(check_stage(&a,&b,path)) printf("%s: +M\n", entry->d_name);
                        else printf("%s: -M\n", entry->d_name);
                    }
                    delete_from_file(path, copy_path);
                }
            }if(!check){
                if(check_stage(&a,&b,path)) printf("%s: +A\n", entry->d_name);
                else printf("%s: -A\n", entry->d_name);
            }
            fclose(commits);

        }
    }
    FILE *copy= fopen(copy_path,"r");
    while (fgets(line, sizeof(line), copy) != NULL){
        if(strstr(line,cwd) != NULL){
            char path[1024],name[512];
            sscanf(line, "PATH:%s", path);
            sscanf(strstr(line,cwd)+ strlen(cwd)+1, "%s", name);
            if(check_stage(&a,&b,path)) printf("%s: +D\n", name);
            else printf("%s: -D\n", name);
        }
    }
    remove(copy_path);
    return 0;
}
int all_status(char *path, int depth){
    char command[512];
    sprintf(command,"tnt reset");
    DIR *dir = opendir(path);
    if (dir == NULL) {
        return 1;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char new_path[1024];
            snprintf(new_path, sizeof(new_path), "%s/%s", path, entry->d_name);
            for (int i = 0; i < depth; i++) {
                printf("  ");
            }
            chdir(path);
            system(command);
//            printf("|-- %s\n", entry->d_name);
            if (entry->d_type == DT_DIR) {
                all_status(new_path, depth + 1);
            }
        }
    }
    closedir(dir);
    return 0;
}
void saveLastModifiedTime(char *filename,char* time) {
    struct stat fileStat;
    if (stat(filename, &fileStat) == -1) {
        return;
    }
    struct tm *modifiedTime = localtime(&fileStat.st_mtime);
    char formattedTime[20];
    strftime(formattedTime, sizeof(formattedTime), "%Y-%m-%d/%H:%M:%S", modifiedTime);
    strcpy(time,formattedTime);
}

int run_commit(int argc, char * const argv[]){
    if(argc < 3 ){
        fprintf(stdout, "please enter a valid command\n");
        return 1;
    }
    char name[128], email[128], message[100];
    char line[2048];
    current_name_email(name, email);
    if(name[0]=='\0' || email[0]=='\0'){
        printf("please set your user name and email first.\n");
        return 1;
    }
    if(strcmp(argv[2], "-m") == 0){
        if(argc == 3){
            printf( "please enter message\n");
            return 1;
        }if(argc > 4){
            printf("please write message in \"\" if it has more then one word\n");
            return 1;
        }if(strlen(argv[3]) > 72){
            printf("Message can have a maximum of 72 chars\n");
            return 1;
        }
        strcpy(message, argv[3]);
    }else if(strcmp(argv[2], "-s") == 0){
        if(argc != 4){
            printf("please enter a valid command\n");
            return 1;
        }
        int check=0;
        FILE *shortcuts;
        char check_name[100];
        sprintf(check_name, "Name:%s ", argv[3]);
        if( (shortcuts= fopen(shortcuts_path, "r")) == NULL){
            printf( "ERROR we lost shortcuts file!\n");
            return 1;
        }
        while(fgets(line, sizeof(line), shortcuts) != NULL){
            if(strstr(line,check_name) != NULL){
                check=1;
                sscanf(strstr(line,"Message:"), "Message:%[^\n]", message);
            }
        }
        fclose(shortcuts);
        if(!check){
            printf("This shortcut name doesn't exist!\n");
            return 1;
        }
    }else{
        printf("please enter a valid command\n");
        return 1;
    }
    int count= count_changed_files();
    if(count==0) return 1;
    get_data(1); //ID,Branch,...
    make_space(message,72);
    FILE *branches;
    if((branches = fopen(branches_path,"r+"))==NULL){
        printf("ERROR we lost tracks file!\n");
        return 1;
    }
    while(fgets(line, sizeof(line), branches) != NULL){
        if(strstr(line,Branch) != NULL){
            int n;
            sscanf(strstr(line,":"), ":%d", &n);
            if(Current_ID != n){
                printf("You only can commit changes on HEAD <<%d->%d>>\n",Current_ID,n);
                return 1;
            }
            long offset = ftell(branches);
            fseek(branches, offset-strlen(strstr(line,":"))+1, SEEK_SET);
            fprintf(branches, "%d", Last_ID);
        }
    }
    fclose(branches);
    FILE *commits;
    if((commits = fopen(commits_path,"a"))==NULL){
        printf("ERROR we lost tracks file!\n");
        return 1;
    }
    fprintf(commits,"Commit_ID:%d\nMessage:%s\nDate & Time:%s\nUser.mame:%s\nUse.email:%s\nBranch:%s\nChanged_files:%d\n",
            Last_ID, message, Time, name, email, Branch, count);
    fclose(commits);
    save_commit();
    printf(" COMMIT ID: %d\n Message: %s\n DATE & TIME: %s\n", Last_ID, message,Time);
    return 0;
}
int save_commit(){
    FILE *tracks,*detail;
    char new_name[128],previous[4096], line[2048], path[1024];
    sprintf(new_name, "commit_%d", Last_ID);
    sprintf(previous,"%s/COMMIT/commit_%d",tnt_path,Current_ID);
    if (chdir(tnt_path) != 0) return 1;
    if (chdir("COMMIT") != 0) return 1;
    if (mkdir(new_name, 0755) != 0) return 1;
    if (chdir(new_name) != 0) return 1;
    detail=fopen("detail","w");
    if((tracks = fopen(tracks_path,"r+"))==NULL){
        printf("ERROR we lost tracks file!\n");
        return 1;
    }
    int ID,status,code;
    while(fgets(line, sizeof(line), tracks) != NULL){
        if(strstr(line,"STATUS:1") != NULL){
            sscanf(line,"%s",path);
            sscanf(strstr(line,"CODE:")+5,"%d",&code);
            if(check_stage(&ID, &status, path)){
                if(status==1){
                    char new_path[4096], pre_path[4096];
                    sprintf(new_path,"%s/COMMIT/%s/%d", tnt_path,new_name,code);
                    sprintf(pre_path,"%s/STAGE/%d", tnt_path,ID);
                    copyFile(pre_path, new_path);
                    fprintf(detail,"PATH:%s\nCODE:%d\nVERSION:%d\n", path, code, Last_ID);
                } else{
                    long offset = ftell(tracks);
                    fseek(tracks, offset-strlen(strstr(line,":"))+1, SEEK_SET);
                    fprintf(tracks, "2");
                }
            }else{
                int ver= last_version(code, previous);
                fprintf(detail,"PATH:%s\nCODE:%d\nVERSION:%d\n", path, code, ver);
            }
        }
    }
    fclose(detail);
    clear_stage();
    return 0;
}
int last_version(int code, char *path){
    if (chdir(path) != 0) return 0;
    int ver;
    char line[2048];
    char code_str[50];
    sprintf(code_str, "CODE:%d\n",code);
    FILE *detail=fopen("detail","r");
    while(fgets(line, sizeof(line), detail) != NULL) {
        if (strcmp(line, code_str) == 0) {
            fgets(line, sizeof(line), detail);
            sscanf(line,"VERSION:%d", &ver);
            fclose(detail);
            return ver;
        }
    }
    fclose(detail);
    return 0;
}
int check_stage(int *ID, int *status, char *path){
    char line[2048];
    FILE *stage = fopen(history_path,"r");
    while(fgets(line, sizeof(line), stage) != NULL){
        if(strstr(line,path) != NULL){
            fgets(line, sizeof(line), stage);
            if(strstr(line,"STAGE:1") != NULL){
                fgets(line, sizeof(line), stage);
                sscanf(line,"STATUS:%d",status);
                fgets(line, sizeof(line), stage);
                sscanf(line,"ID:%d",ID);
                return 1;
            }
        }
    }
    return 0;
}
void clear_stage(){
    char path[4096];
    sprintf(path,"%s/STAGE", tnt_path);
    DIR *dir = opendir(path);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, "history") != 0) {
            char file_path[10000];
            sprintf(file_path,  "%s/%s", path, entry->d_name);
            remove(file_path);
        }
    }
    closedir(dir);
    FILE *clear= fopen(history_path,"w");
    fclose(clear);

}
int count_changed_files(){
    FILE *stage;
    if( (stage= fopen(history_path, "r")) == NULL){
        printf( "ERROR we lost STAGE AREA !\n");
        return 0;
    }
    int count=0;
    char  line[2048];
    while(fgets(line, sizeof(line), stage) != NULL){
        if(strstr(line,"STAGE:1") != NULL) count++;
    }
    fclose(stage);
    return count;
}
void current_name_email(char *name, char *email){
    FILE *local, *global;
    int numb1=1,numb2=1;
    char line[1024];
    char global_name[256] , local_name[256], global_email[256] , local_email[256];
    char time1_global[128], time2_global[256], time1_local[128], time2_local[128];
    struct passwd *pw = getpwuid(getuid());
    if (chdir(pw->pw_dir) != 0) return;
    if((global=fopen(".tnt_global_configs", "r"))==NULL){
        printf("ERROR we lost global configs!\n");
        return;
    }
    while (fgets(line, sizeof(line), global) != NULL || numb1 < 5){
        switch (numb1) {
            case 1: sscanf(line,"user name:%[^\n]" ,global_name);
                break;
            case 2: sscanf(line, "date and time:%s", time1_global);
                break;
            case 3: sscanf(line,"user email:%[^\n]" ,global_email);
                break;
            case 4: sscanf(line, "date and time:%s", time2_global);
                break;
            default:
                break;
        }
        numb1++;
    }
    fclose(global);
    if((local=fopen(config_path, "r")) == NULL){
        printf("ERROR we lost local configs!\n");
        return;
    }while (fgets(line, sizeof(line), local) != NULL || numb2 < 5){
        if(numb2 == 1) sscanf(line,"user name:%[^\n]" ,local_name);
        else if(numb2 == 2) sscanf(line, "date and time:%s", time1_local);
        else if(numb2 ==3 ) sscanf(line,"user email:%[^\n]" , local_email);
        else sscanf(line, "date and time:%s", time2_local);
        numb2++;
    }
    fclose(local);
    if(local_name[0]==' ' && global_name[0]!=' ') strcpy(name,global_name);
    else if(local_name[0]!=' ' && global_name[0]==' ') strcpy(name,local_name);
    else if(local_name[0]==' ' && global_name[0]==' ') return;
    else{
        if(compareTimes(time1_global,time1_local)>0) strcpy(name,global_name);
        else strcpy(name,local_name);
    }
    if(local_email[0]==' ' && global_email[0]!=' ') strcpy(email,global_email);
    else if(local_email[0]!=' ' && global_email[0]==' ') strcpy(email,local_email);
    else if(local_email[0]==' ' && global_email[0]==' ') return;
    else{
        if(compareTimes(time1_global,time1_local)>0) strcpy(email,global_email);
        else strcpy(email,local_email);
    }
}
void get_data(int mode){
    FILE *config;
    int line_numb=1;
    char line[256];
    if((config=fopen(config_path, "r")) == NULL){
        printf("ERROR we lost local configs!\n");
        return;
    }while (fgets(line, sizeof(line), config) != NULL && line_numb<9){
        if(line_numb == 6) sscanf(line, "last commit ID:%d", &Last_ID);
        if(line_numb == 7) sscanf(line, "current commit ID:%d", &Current_ID);
        if(line_numb == 8) sscanf(line, "Branch:%s", Branch);
        line_numb++;
    }
    fclose(config);
    config=fopen(config_path, "r+");
    line_numb=1;
    if(mode){
        Last_ID++;
        while (fgets(line, sizeof(line), config) != NULL && line_numb<9){
            if(line_numb == 5) fprintf(config, "last commit ID:%d", Last_ID );
            if(line_numb == 6) fprintf(config, "current commit ID:%d", Last_ID);
            line_numb++;
        }
    }

}
int set_shortcut(int argc, char *argv[]){
    if(argc != 6 || strcmp(argv[2],"-m")!=0 || strcmp(argv[4],"-s")!=0 ){
        printf( "please enter a valid command\n");
        return 1;
    }
    char message[128];
    strcpy(message, argv[3]);
    make_space(message,72);
    FILE *shortcuts=fopen(shortcuts_path,"a");
    fprintf(shortcuts, "Name:%s Message:%s\n",argv[5],argv[3]);
    return 0;
}
int replace_shortcut(int argc, char *argv[]){
    if(argc != 6 || strcmp(argv[2],"-m")!=0 || strcmp(argv[4],"-s")!=0 ){
        printf( "please enter a valid command\n");
        return 1;
    }
    FILE *shortcuts=fopen(shortcuts_path,"r+");
    char line[1024],name[512], message[128];
    strcpy(message, argv[3]);
    make_space(message,72);
    sprintf(name,"Name:%s ",argv[5]);
    while (fgets(line, sizeof(line), shortcuts) != NULL ){
        if(strstr(line,name) != NULL){
            long offset = ftell(shortcuts);
            fseek(shortcuts, offset-strlen(line), SEEK_SET);
            fprintf(shortcuts, "Name:%s Message:%s\n",argv[5],message);
            return 0;
        }
    }
    return 0;
}
int remove_shortcut(int argc, char *argv[]){
    if(argc != 4 || strcmp(argv[2],"-s")!=0 ){
        printf( "please enter a valid command\n");
        return 1;
    }
    char line[1024], message[128];
    strcpy(message, argv[3]);
    make_space(message,72);
    sprintf(line, "Name:%s Message:%s\n",argv[5],message);
    delete_from_file(line, shortcuts_path);
    return 0;
}
void copyLast7Lines(char *output) {
    FILE *inputFile;
    FILE *outputFile = fopen(output, "w");
    int count ,count2;
    count = Last_ID*7;
    char line[3000];
    for(int i=0; i<Last_ID; i++){
        count-=7;
        inputFile = fopen(commits_path, "r");
        count2=1;
        if(count>0){
            while(fgets(line,3000,inputFile)!=NULL && count2<count){
                count2++;
            }
        }
        for(int j=0; j<7; j++){
            fgets(line,3000,inputFile);
            fputs(line,outputFile);
        }
        fputs("\n",outputFile);
        fclose(inputFile);
    }
    fclose(outputFile);
}
int run_log(int argc, char *argv[]){
    char copy_path[2048] , line[1024];
    get_data(0);
    sprintf(copy_path,"%s/copy",tnt_path);
    copyLast7Lines(copy_path);
    FILE *copy=fopen(copy_path,"r");
    if(argc==2){
        while(fgets(line, sizeof(line),copy)!=NULL){
            printf("%s",line);
        }
    }else if(strcmp(argv[2],"-n")==0){
        char *check;
        long n= strtol(argv[3], &check,10);
        if( *check != '\0' || n < 1){
            printf( "please enter a valid command\n");
            return 1;
        }n*=8;
        while(fgets(line, sizeof(line),copy)!=NULL && n>0){
            printf("%s",line);
            n--;
        }
    }else if(strcmp(argv[2],"-branch")==0){
        int check = 1;
        FILE *branches = fopen(branches_path, "r");
        while (fgets(line, sizeof(line), branches) != NULL) {
            if (strstr(line, argv[3]) != NULL) {
                check = 0;
                break;
            }
        }
        fclose(branches);
        if (check) {
            printf("invalid branch name!\n");
            return 1;
        }
        char branch_name[100];
        sprintf(branch_name, "Branch:%s", argv[3]);
        strcat(branch_name,"\n");
        int n = 0, commits_id[Last_ID], counter = -1;
        FILE *commits = fopen(commits_path, "r");
        while (fgets(line, sizeof(line), commits) != NULL) {
            if (strstr(line, "ID:") != NULL) counter++;
            if (strcmp(line, branch_name) == 0) {
                commits_id[n] = Last_ID - counter;
                n++;
            }
        }
        fclose(commits);
        for(int i=1; i<=Last_ID; i++){
            for (int j = 0; j <= 7; j++) {
               fgets(line, sizeof(line), copy);
               if(i==commits_id[n-1]){
                   printf("%s", line);
               }
            }if(i==commits_id[n-1]) n--;
        }
    }else if(strcmp(argv[2],"-author")==0){
        char user_name[100];
        sprintf(user_name, "User.mame:%s ", argv[3]);
        int n = 0, commits_id[Last_ID], counter = -1;
        FILE *commits = fopen(commits_path, "r");
        while (fgets(line, sizeof(line), commits) != NULL) {
            if (strstr(line, "ID:") != NULL) counter++;
            if (strstr(line, user_name) != NULL) {
                commits_id[n] = Last_ID - counter;
                n++;
            }
        }
        fclose(commits);
        for(int i=1; i<=Last_ID; i++){
            for (int j = 0; j <= 7; j++) {
                fgets(line, sizeof(line), copy);
                if(i==commits_id[n-1]){
                    printf("%s", line);
                }
            }if(i==commits_id[n-1]) n--;
        }
    }else if(strcmp(argv[2],"-since")==0){
        int commits_id, counter = 0;
        FILE *commits = fopen(commits_path, "r");
        while (fgets(line, sizeof(line), commits) != NULL) {
            if (strstr(line, "ID:") != NULL) counter++;
            if (strstr(line, argv[3]) != NULL) {
                commits_id = Last_ID - counter;
                printf("%d",commits_id);
                break;
            }
        }
        fclose(commits);
        for(int i=0; i<Last_ID; i++){
            for (int j = 0; j <= 7; j++) {
                fgets(line, sizeof(line), copy);
                if(i<=commits_id) printf("%s", line);
            }
        }
    }else if(strcmp(argv[2],"-before")==0){
        int commits_id, counter = 0;
        FILE *commits = fopen(commits_path, "r");
        while (fgets(line, sizeof(line), commits) != NULL) {
            if (strstr(line, "ID:") != NULL) counter++;
            if (strstr(line, argv[3]) != NULL) {
                commits_id = Last_ID - counter;
            }
        }
        fclose(commits);
        for(int i=0; i<Last_ID; i++){
            for (int j = 0; j <= 7; j++) {
                fgets(line, sizeof(line), copy);
                if(i>=commits_id) printf("%s", line);
            }
        }
    }else if(strcmp(argv[2],"-search")==0){
        int n = 0, commits_id[Last_ID], counter = -1;
        FILE *commits = fopen(commits_path, "r");
        while (fgets(line, sizeof(line), commits) != NULL) {
            if (strstr(line, "ID:") != NULL) counter++;
            if (strstr(line, argv[3]) != NULL && strstr(line, "Message:") != NULL) {
                commits_id[n] = Last_ID - counter;
                n++;
            }
        }
        fclose(commits);
        for(int i=1; i<=Last_ID; i++){
            for (int j = 0; j <= 7; j++) {
                fgets(line, sizeof(line), copy);
                if(i==commits_id[n-1]){
                    printf("%s", line);
                }
            }if(i==commits_id[n-1]) n--;
        }
    }else{
        printf( "please enter a valid command\n");
    }
    fclose(copy);
    return 0;
}
int run_branch(int argc, char *argv[]){
    if (argc==3){
        FILE *branch=fopen(branches_path,"a+");
        char line[256],name[128];
        sprintf(name,"%s:",argv[2]);
        while (fgets(line, sizeof(line), branch) != NULL){
            if(strstr(line,name)!=NULL){
                printf("This branch name already exists\n");
                return 1;
            }
        }
        get_data(0);
        sprintf(name, "%s:%d", argv[2], Last_ID);
        make_space(name,100);
        strcat(name,"\n");
        fputs(name, branch);
        fclose(branch);
    }else if(argc==2){
        FILE *branch=fopen(branches_path,"r");
        char line[256];
        while (fgets(line, sizeof(line), branch) != NULL){
            printf("%s",line);
        }
        fclose(branch);
    }else{
        printf( "please enter a valid command\n");
        return 1;
    }
    return 0;
}
void checkout_commit(long ID){
    char commit_path[4096], line[2048];
    char sor_path[10000], des_path[4096],code[2048],version[2048];
    sprintf(commit_path,"%s/COMMIT/commit_%ld/detail",tnt_path, ID);
    FILE *detail= fopen(commit_path,"r");
    while (fgets(line, sizeof(line), detail) != NULL){
        sscanf(line,"PATH:%s",des_path);
        fgets(line, sizeof(line), detail);
        sscanf(line,"CODE:%s",code);
        fgets(line, sizeof(line), detail);
        sscanf(line,"VERSION:%s",version);
        if(version[0]=='0'){
            remove(des_path);
        }else{
            sprintf(sor_path,"%s/COMMIT/commit_%s/%s", tnt_path, version, code);
            copyFile(sor_path,des_path);
        }

    }
    fclose(detail);
}
int run_checkout(int argc, char *argv[]){
    get_data(0);
    long ID;
    if(strstr(argv[2],"HEAD") != NULL){
        if(strcmp(argv[2],"HEAD")==0)  ID=Last_ID;
        else{
            int n;
            sscanf(argv[2],"HEAD-%d",&n);
            ID=Last_ID-n;
        }
    }else{
        char *branch;
        ID = strtol(argv[2], &branch,10);
        if( branch[0] != '\0'){
            char line[1024];
            FILE *branches = fopen(branches_path, "r");
            while (fgets(line, sizeof(line), branches) != NULL) {
                if (strstr(line, argv[3]) != NULL) {
                    sscanf(strstr(line, ":")+1, "%ld", &ID);
                    break;
                }
            }
        }else{
            if(count_changed_files()){
                printf("You have uncommitted changes\n");
                return 1;
            }
        }
    }
    checkout_commit(ID);
    return 0;
}
void diffFiles(const char *filename1, const char *filename2, int lineStart, int lineEnd) {
    FILE *file1 = fopen(filename1, "r");
    FILE *file2 = fopen(filename2, "r");
    char line1[4096], line2[4096];
    int lineCounter = 0 ,is_blank = 1;
    while (lineCounter < lineStart - 1) {
        while (fgets(line1, sizeof(line1), file1)!= NULL && is_blank == 1) {
            is_blank = 1;
            for (int i = 0; line1[i] != '\0'; i++) {
                if (line1[i] != ' ' && line1[i] != '\t' && line1[i] != '\n') {
                    is_blank = 0;
                    break;
                }
            }
        }
        while (fgets(line2, sizeof(line2), file2)!= NULL && is_blank == 1) {
            is_blank = 1;
            for (int i = 0; line2[i] != '\0'; i++) {
                if (line2[i] != ' ' && line2[i] != '\t' && line2[i] != '\n') {
                    is_blank = 0;
                    break;
                }
            }
        }
        lineCounter++;
    }
    while (lineCounter < lineEnd) {
        while (fgets(line1, sizeof(line1), file1)!= NULL && is_blank == 1) {
            is_blank = 1;
            for (int i = 0; line1[i] != '\0'; i++) {
                if (line1[i] != ' ' && line1[i] != '\t' && line1[i] != '\n') {
                    is_blank = 0;
                    break;
                }
            }
        }
        while (fgets(line2, sizeof(line2), file2)!= NULL && is_blank == 1) {
            is_blank = 1;
            for (int i = 0; line2[i] != '\0'; i++) {
                if (line2[i] != ' ' && line2[i] != '\t' && line2[i] != '\n') {
                    is_blank = 0;
                    break;
                }
            }
        }
        lineCounter++;
        char *ptr1 = line1;
        char *ptr2 = line2;
        if (strcmp(ptr1, ptr2) != 0) {
            printf(ANSI_COLOR_RED"%d: %s"ANSI_COLOR_RED, lineCounter, ptr1);
            printf(ANSI_COLOR_GREEN"%d: %s"ANSI_COLOR_GREEN, lineCounter, ptr2);
        }
    }
    fclose(file1);
    fclose(file2);
}
int run_diff(int argc, char *argv[]){
    char file1[1024],file2[1024];
    int lineStart , lineEnd;
    if(strcmp(argv[2],"-c")==0){
        sprintf(file1,"%s/COMMIT/commit_%s/", tnt_path, argv[3]);
        sprintf(file2,"%s/COMMIT/commit_%s/", tnt_path, argv[4]);
        diffFiles(file1, file2, 0, 4096);

        return 0;
    }
    strcpy(file1,argv[3]);
    strcpy(file2,argv[4]);
    if(strcmp(argv[5],"line1")==0){
        sscanf(argv[6], &lineStart);
        if(strcmp(argv[7],"line2")==0)
            sscanf(argv[8], &lineEnd);
    }else{
        lineStart=0;
        if(strcmp(argv[5],"line2")==0)
            sscanf(argv[6], &lineEnd);
        else
            lineEnd=1024;
    }
    diffFiles(file1, file2, lineStart, lineEnd);

    return 0;
}
int main(int argc, char *argv[]) {
    time_t raw_time;
    static struct tm *time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);
    strftime(Time, sizeof(Time), "%Y-%m-%d/%H:%M:%S", time_info);

    if (argc < 2) {
        printf( "please enter a valid command\n");
        return 1;
    }
    if (getcwd(cwd, sizeof(cwd)) == NULL) return 1;
    else if (strcmp(argv[1], "init") == 0) {
        return run_init(argc, argv);
    } else if (strcmp(argv[1], "config") == 0){
        return run_config(argc, argv);
    } else if(find_tnt()==1 || tnt_path[0]==0) {
        fprintf(stdout, "There is not any repository\n");
        return 1;
    }else if (strcmp(argv[1], "add") == 0){
        return manage_add_mode(argc, argv);
    }else if (strcmp(argv[1], "reset") == 0) {
        return manage_reset_mode(argc, argv);
    }else if (strcmp(argv[1], "status") == 0) {
        return run_status(argc, argv);
    }else if (strcmp(argv[1], "status-a") == 0) {
        return all_status(cwd,0);
    }else if (strcmp(argv[1], "commit") == 0) {
        return run_commit(argc, argv);
    }else if (strcmp(argv[1], "set") == 0) {
        return set_shortcut(argc, argv);
    }else if (strcmp(argv[1], "replace") == 0) {
        return replace_shortcut(argc, argv);
    }else if (strcmp(argv[1], "remove") == 0) {
        return remove_shortcut(argc, argv);
    }else if (strcmp(argv[1], "log") == 0) {
        return run_log(argc, argv);
    }else if (strcmp(argv[1], "branch") == 0) {
        return run_branch(argc, argv);
    }else if (strcmp(argv[1], "checkout") == 0) {
        return run_checkout(argc, argv);
    }else if (strcmp(argv[1], "diff") == 0) {
        return run_diff(argc, argv);
    }else{
        return Alias(argc, argv);
    }/* else if (strcmp(argv[1], "checkout") == 0) {
        return run_checkout(argc, argv);
    }*/
    return 0;
}
