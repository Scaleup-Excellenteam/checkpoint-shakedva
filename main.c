#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 50
#define MAX_TEL 10
#define MAX_CLASSES 10
#define MAX_COURSES 10
#define MAX_LEVELS 12
#define MAX_INPUT_LINE 100

//TODO change file path
const char* FILE_PATH = "/home/shakedva/Desktop/bootcamp/checkpoint/school/students.txt";

//struct Course
//{
//    char name[MAX_LEN];
//    int grade;
//    struct Course* next;
//};

struct Student
{
    char name[MAX_LEN];
    char tel[MAX_TEL];
    int grades[MAX_COURSES];
    struct Student* next;
};
struct School
{
    struct Student * DB[MAX_LEVELS][MAX_CLASSES];
};

static struct School school;

FILE* openFile(const char* fileName)
{
    FILE *input_file;
    input_file = fopen(fileName, "r");
    if (input_file == 0)
    {
        perror("Can not open input file\n");
        exit(-1);
    }
    return input_file;
}

int parseLine(char* line, struct Student* student) {
    char firstName[MAX_LEN], lastName[MAX_LEN];
    char tel[11];
    char level[20];
    char class[10];
    int grades[10];

    int numRead = sscanf(line, "%s %s %s %s %s", firstName, lastName, tel, level, class);
    if (numRead != 5) {
        return 0; // Unable to read the first four fields
    }
    ///strcat(strcat(firstName, " "), lastName);
    printf("%s / %s / %s / %s / %s\n", firstName, lastName, tel, level, class);

    for (int i = 0; i < 10; ++i) {

        char delim[] = " ";
        char *ptr = strtok(line + strlen(firstName) + strlen(lastName) + strlen(tel) + strlen(level) + strlen(class), delim);

        while(ptr != NULL)
        {
            printf("'%s'\n", ptr);
            ptr = strtok(NULL, delim);
        }

//        if (sscanf(line + strlen(firstName) + strlen(lastName) + strlen(tel) + strlen(level) + strlen(class),
//                   "%d", &grades[i]) != 1) {
//            return 0; // Unable to read the grades
//        }
    }
//    for (int i = 0; i < 10; ++i)
//        printf("%d", grades[i]);
    printf("\n");
//    strcpy(student->name, strcat(firstName, lastName));
//    strcpy(student->tel, tel);
    return 1;
}

void readFile(FILE *file)
{
    char line[MAX_INPUT_LINE];
    struct Student student;
    while (fgets(line, sizeof(line), file)) {
        parseLine(line, &student);
    }
    puts("\n---------------------------");
}

void init()
{
    FILE* inputFile = openFile(FILE_PATH);
    readFile(inputFile);
    fclose(inputFile);
}

int main() {
    init();
    return 0;
}
