#include <stdio.h>
#include <stdlib.h>

#define MAX_LEN 50
#define MAX_TEL 10
#define MAX_CLASSES 10
#define MAX_COURSES 10
#define MAX_LEVELS 7
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
    struct Student* DB[MAX_LEVELS][MAX_CLASSES];
};



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
void parseLine(const char *line) {
    struct Student *student = NULL;
    sscanf(line, "%s %s", student->name, student->tel);
    for (int i = 0; i < 10; i++) {
        sscanf(line,"%d", &student->grades[i]);
    }
}

void readFile(FILE *file)
{
    char line[MAX_INPUT_LINE];
    while (fgets(line, sizeof(line), file)) {
        parseLine(line);
    }
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
