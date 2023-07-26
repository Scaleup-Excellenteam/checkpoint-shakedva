#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_NAME_LEN 20
#define MAX_TEL 10
#define MAX_CLASSES 10
#define MAX_COURSES 10
#define MAX_CLASS_LEN 10
#define MAX_LEVELS 12
#define MAX_INPUT_LINE 100

//TODO change file path
const char *FILE_PATH = "/home/shakedva/Desktop/bootcamp/checkpoint/school/students.txt";
const int NUM_OF_DATA = 15;

struct Student {
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    char tel[MAX_TEL];
    int grades[MAX_COURSES];
    struct Student *next;
};
struct School {
    struct Student *DB[MAX_LEVELS][MAX_CLASSES];
};

static struct School school;

FILE *openFile(const char *fileName) {
    FILE *input_file;
    input_file = fopen(fileName, "r");
    if (input_file == 0) {
        perror("Can not open input file\n");
        exit(-1);
    }
    return input_file;
}

int parseLine(char *line) {
    char firstName[MAX_NAME_LEN], lastName[MAX_NAME_LEN];
    char tel[MAX_TEL];
    char levelStr[MAX_CLASS_LEN];
    char classStr[MAX_CLASS_LEN];
    int grades[MAX_COURSES];

    int numRead = sscanf(line, "%s %s %s %s %s %d %d %d %d %d %d %d %d %d %d",
                         firstName, lastName, tel, levelStr, classStr, &grades[0], &grades[1], &grades[2], &grades[3],
                         &grades[4],
                         &grades[5], &grades[6], &grades[7], &grades[8], &grades[9]);
    if (numRead != NUM_OF_DATA) {
        return 0; // Unable to read the first four fields
    }

    char *ptr;
    long level, class;
    level = strtol(levelStr, &ptr, 10);
    class = strtol(classStr, &ptr, 10);

    printf("%s / %s / %s / %ld / %ld / %d %d %d %d %d %d %d %d %d %d\n", firstName, lastName, tel, level, class,
           grades[0], grades[1], grades[2], grades[3], grades[4],
           grades[5], grades[6], grades[7], grades[8], grades[9]);

    struct Student *student = (struct Student *) malloc(sizeof(struct Student));
    if (student == NULL) {
        printf("malloc failed\n");
        return -1;
    }
    strcpy(student->firstName, firstName);
    strcpy(student->lastName, lastName);
    strcpy(student->tel, tel);
    for (int i = 0; i < MAX_COURSES; i++)
        student->grades[i] = grades[i];

    student->next = school.DB[level][class];
    school.DB[level][class] = student;
    return 1;
}

void readFile(FILE *file) {
    char line[MAX_INPUT_LINE];
    while (fgets(line, sizeof(line), file)) {
        parseLine(line);
    }
}

void freeClassStudents(struct Student *student) {
    while (student != NULL) {
        struct Student *temp = student;
        student = student->next;
        free(temp);
    }
}

void freeSchool() {
    for (int level = 0; level < MAX_LEVELS; level++) {
        for (int class = 0; class < MAX_CLASSES; class++) {
            struct Student *student = school.DB[level][class];
            if (student != NULL) {
                freeClassStudents(student);
                school.DB[level][class] = NULL;
            }
        }
    }
}

void init() {
    FILE *inputFile = openFile(FILE_PATH);
    readFile(inputFile);
    fclose(inputFile);
}

int main() {
    init();
    freeSchool();
    return 0;
}
