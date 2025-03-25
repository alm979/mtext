#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_FILES 32
#define MAX_NAME  64
#define MAX_TEXT  1024
#define FILECOUNT "filecount.bin"
#define FILELIST  "filelist.bin"

#ifdef _WIN32
#   define SAVEDIR "files\\"
#   define SCRCLEAR "cls"
#else
#   define SAVEDIR "files/"
#   define SCRCLEAR "clear"
#endif

/*--------- Internal function prototypes -----------*/
int  get_choice(void);
void get_fname(char *buffer);
void get_fcontent(char *buffer);
void file_check(FILE *file, const char *section);
bool out_of_bound(int choice);
int  read_filecount(void);
void write_filecount(int new_filecount);
void read_filelist(void);
void write_filelist(char *new_filename);
void read_filecontent(char *name, char *content_des);
void write_file(char *name, char *content);
void remove_file(char *name, int choice);
void remove_all_file(void);

/*------------ Global variables --------------------*/
int  filecount = 0;
char filelist[MAX_FILES][MAX_NAME];
FILE *flist, *fcount;

/*----------- Main program functions--------------*/
void create_file(void)
{
    if (filecount == MAX_FILES) {
        printf("Limit is reached.\n");
        return;
    }

    system(SCRCLEAR);

    char fname[MAX_NAME];
    char fcontent[MAX_TEXT];

    printf("Title: ");
    get_fname(fname);

    printf("Enter '##' to save:\n");
    get_fcontent(fcontent);

    write_filecount(++filecount);
    write_filelist(fname);
    write_file(fname, fcontent);
    read_filelist();
    printf("-- text saved --\n");
    getchar();
}

void print_list(void)
{
    if (filecount == 0) {
        printf("No saved file.\n");
        return;
    }

    system(SCRCLEAR);

    printf("%d text file%s:\n", filecount, (filecount > 1)? "s":"");
    for (int i = 0; i < filecount;i++) {
        printf("%2d %s\n", i+1, filelist[i]);
    }
}

void open_file(void)
{
    if (filecount == 0) {
        printf("No saved file.\n");
        return;
    }

    system(SCRCLEAR);

    char namebuf[MAX_NAME];
    char contentbuf[MAX_TEXT];

    print_list();
    printf("\nEnter the file number to open. 0 to exit.\n");
    int choice = get_choice();
    if (choice == 0 || out_of_bound(choice) == true) return;
    choice -= 1;

    snprintf(namebuf, MAX_NAME, "%s", filelist[choice]);
    read_filecontent(namebuf, contentbuf);

    system(SCRCLEAR);

    printf("Title: %s\n\n", namebuf);
    printf("%s\n", contentbuf);
}

void delete_file(void)
{
    if (filecount == 0) {
        printf("No saved file.\n");
        return;
    }

    system(SCRCLEAR);

    char namebuf[MAX_NAME];

    print_list();
    printf("\nEnter file number to delete. 0 to exit.\n");
    int choice = get_choice();
    if (choice == 0 || out_of_bound(choice) == true) return;
    choice -= 1;

    snprintf(namebuf, MAX_NAME, "%s", filelist[choice]);
    remove_file(namebuf, choice);

    write_filecount(--filecount);
    read_filelist();
    printf("-- text deleted --\n");
}

void delete_all_file(void)
{
    if (filecount == 0) {
        printf("No saved file.\n");
        return;
    }

    flist = fopen(FILELIST, "wb");
    file_check(flist, "delete_all_file");
    remove_all_file();
    write_filecount(0);
    filecount = 0;
    read_filelist();
    printf("-- emptied the list --\n");
}

/*---------------------- Main -----------------------*/
int main(void)
{
    filecount = read_filecount();
    read_filelist();

    system(SCRCLEAR);

    int quit = 0;
    while (!quit) {
        printf("***** mtext *****\n");
        printf("1 New file\n");
        printf("2 Open file\n");
        printf("3 Saved files\n");
        printf("4 Delete a file\n");
        printf("5 Delete all files\n");
        printf("0 Exit program\n");

        switch (get_choice()) {
            case 1: create_file();
                    break;
            case 2: open_file();
                    break;
            case 3: print_list();
                    break;
            case 4: delete_file();
                    break;
            case 5: delete_all_file();
                    break;
            case 0: quit = 1;
                    break;
            default:
                    printf("Not a choice.\n");
                    break;
        }
        getchar();
        system(SCRCLEAR);
    }

    return 0;
}

/*------------- Inputing functions -----------------*/
int get_choice(void)
{
    int input;
    while (1) {
        printf(": ");
        if (scanf("%d", &input) == 1 && input >= 0) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            break;
        } else {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Error input.\n");
        }
    }

    return input;
}

void get_fname(char *buffer)
{
    int i, c;
    for (i = 0; (c = getchar()) != '\n'; i++) {
        if (strchr(" \\/\'\"~$", c) != NULL) {
            c = '_';
        }
        buffer[i] = c;
    }

    if (i != 0) {
        buffer[i] = '\0';
    } else {
        snprintf(buffer, MAX_NAME, "File%d", filecount);
    }
}

void get_fcontent(char *buffer)
{
    // Entering '##' will terminate the inputing
    int i = 0;
    char c;
    while (1){
        c = getchar();
        if (i >= 1 && buffer[i-1] == '#' && c == '#') break;
        // Will save one '#' to the buffer as the last one is the i which will be replaced with '\0'
        buffer[i++] = c;
        if (i >= MAX_TEXT) break;
    }

    if (i != 0) {
        buffer[i--] = '\0'; // To not save the last '#' charcter from the terminator
        snprintf(buffer + i, MAX_TEXT - i, "\n\n%s %s", __DATE__, __TIME__);
    } else {
        snprintf(buffer, MAX_TEXT, "\n%s %s", __DATE__, __TIME__);
    }
}

/*------------ Internal functions -------------------*/
void file_check(FILE *file, const char *section)
{
    if (file == NULL) {
        fprintf(stderr, "Func: %s\nFail: opening a file.\n", section);
        exit(1);
    }
}

bool out_of_bound(int choice)
{
    if (choice > filecount) {
        printf("Our of bounds.\n");
        return true;
    }

    return false;
}
int read_filecount(void)
{
    int c = 0;
    fcount = fopen(FILECOUNT, "rb");
    file_check(fcount, "read_filecount");
    if (fscanf(fcount, "%d", &c) != 1) {
        fprintf(stderr, "Fail reading file count.\n");
        exit(1);
    }

    fclose(fcount);
    return c;
}

void write_filecount(int new_filecount)
{
    fcount = fopen(FILECOUNT, "wb");
    file_check(fcount, "write_filecount");
    fprintf(fcount, "%d", new_filecount);
    fclose(fcount);
}

void read_filelist(void)
{
    if (filecount == 0) return;
    char buf[MAX_NAME];
    flist = fopen(FILELIST, "rb");
    file_check(flist, "read_filelist");
    for (int i = 0; fgets(buf, MAX_NAME, flist) != NULL; i++) {
        buf[strcspn(buf, "\n")] = 0;
        strcpy(filelist[i], buf);
    }

    fclose(flist);
}

void write_filelist(char *new_filename)
{
    flist = fopen(FILELIST, "ab");
    file_check(flist, "write_filelist");
    fprintf(flist, "%s\n", new_filename);
    fclose(flist);
}

void read_filecontent(char *name, char *content_des)
{
    char savepath[MAX_NAME];
    snprintf(savepath, MAX_NAME, "%s%s.bin", SAVEDIR, name);
    FILE *rf = fopen(savepath, "rb");
    file_check(rf, "read_filecontent");
    fread(content_des, 1, MAX_TEXT, rf);
    fclose(rf);
}

void write_file(char *name, char *content)
{
    char savepath[MAX_NAME];
    snprintf(savepath, MAX_NAME, "%s%s.bin", SAVEDIR, name);
    FILE *wf = fopen(savepath, "wb");
    file_check(wf, "write_file");
    fwrite(content, MAX_TEXT, 1, wf);
    fclose(wf);
}

void remove_file(char *filename, int choice)
{
    char file[MAX_NAME];
    snprintf(file, MAX_NAME, "%s%s.bin", SAVEDIR, filename);
    remove(file);
    flist = fopen(FILELIST, "wb");
    file_check(flist, "remove_file");

    for (int i = 0; i < filecount; i++) {
        if (choice == i) continue;
        fprintf(flist, "%s\n", filelist[i]);
    }

    fclose(flist);
}

void remove_all_file(void)
{
    char file[MAX_NAME];

    for (int i = 0; i < filecount; i++) {
        snprintf(file, MAX_NAME, "%s%s.bin", SAVEDIR, filelist[i]);
        remove(file);
    }
}
