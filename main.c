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
const char *INPUT_FILE_PATH = "resources/students.txt";
const char *DB_FILE_PATH = "resources/studentsDB.txt";
const char *MENU_MSG = "Please choose an option:\n1. Insert a new student\n2. Remove a student\n3. Edit a student\n"
                       "4. Search a student by first and last name\n5. The 10 excellent students in a course by a level\n6. Students leaving\n"
                       "7. Calculating an average for a course by a level\n8. Export the database into a file\n"
                       "9. Print all students\n"; //TODO remove option 9
const int NUM_OF_DATA = 15;
const int DECIMAL = 10;

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

FILE *openOutputFile(const char *fileName) {
    FILE *outputFile;
    outputFile = fopen(fileName, "w");
    if(outputFile == NULL)
    {
        perror("Can not open file to save the database\n");
        exit(1);
    }
    return outputFile;
}
FILE *openInputFile(const char *fileName) {
    FILE *inputFile;
    inputFile = fopen(fileName, "r");
    if (inputFile == NULL) {
        perror("Can not open input file\n");
        exit(1);
    }
    return inputFile;
}

void loadDatabaseToFile(FILE *outputFile)
{
    for (int level = 0; level < MAX_LEVELS; level++) {
        for (int class = 0; class < MAX_CLASSES; class++) {
            struct Student *student = school.DB[level][class];
            while (student != NULL) {
                fprintf(outputFile,"%s %s %s %d %d ",
                        student->firstName, student->lastName, student->tel, level, class);
                for(int gradeIndex = 0; gradeIndex < MAX_COURSES;  gradeIndex++)
                    fprintf(outputFile, "%d ", student->grades[gradeIndex]);
                fprintf(outputFile, "\n");

                student = student->next;
            }
        }
    }
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
        return 0; // Unable to read all the fields
    }

    long level, class;
    level = strtol(levelStr, NULL, DECIMAL);
    class = strtol(classStr, NULL, DECIMAL);

    printf("%s / %s / %s / %ld / %ld / %d %d %d %d %d %d %d %d %d %d\n", firstName, lastName, tel, level, class,
           grades[0], grades[1], grades[2], grades[3], grades[4],
           grades[5], grades[6], grades[7], grades[8], grades[9]);

    struct Student *student = (struct Student *) malloc(sizeof(struct Student));
    if (student == NULL) {
        printf("malloc failed\n");
        return -1;
    }
//    strcpy(student->firstName, firstName);
//    strcpy(student->lastName, lastName);
    strncpy(student->firstName, firstName, sizeof(student->firstName));
    strncpy(student->lastName, lastName, sizeof(student->lastName));
    strncpy(student->tel, tel, sizeof(student->tel));
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
    FILE *inputFile = openInputFile(INPUT_FILE_PATH);
    readFile(inputFile);
    fclose(inputFile);
}

void handleClosing()
{
    FILE *outputFile = openOutputFile(DB_FILE_PATH);
    loadDatabaseToFile(outputFile);
    fclose(outputFile);
    freeSchool();
}
void menu()
{
    char option[MAX_NAME_LEN];
    printf("%s", MENU_MSG);
    scanf("%s", option);
    switch (strtol(option, NULL, DECIMAL))
    {
        case 1:
            break;
    }
}
int main() {
    init();
    menu();
    handleClosing();
    return 0;
}
