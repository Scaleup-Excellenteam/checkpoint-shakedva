#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_NAME_LEN 20
#define MAX_TEL 12
#define MAX_CLASSES 10
#define MAX_COURSES 10
#define MAX_CLASS_LEN 10
#define MAX_LEVELS 12
#define MAX_INPUT_LINE 100

//TODO change file path
const char *INPUT_FILE_PATH = "resources/students.txt";
const char *DB_FILE_PATH = "resources/studentsDB.txt";
const char *INSERT_STUDENT_MSG = "Please enter the the following information separated by spaces\nFirst and last name, "
                                 "telephone number, level, class and their 10 course's grades\n";
const char *DELETE_STUDENT_MSG = "Please enter the first and last name of the student\n";

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

struct Student* createStudent(char* firstName, char* lastName, char*  tel, const int* grades, struct Student *next)
{
    struct Student *student = (struct Student *) malloc(sizeof(struct Student));
    if (student == NULL) {
        return NULL;
    }
    strncpy(student->firstName, firstName, sizeof(student->firstName));
    strncpy(student->lastName, lastName, sizeof(student->lastName));
    strncpy(student->tel, tel, sizeof(student->tel));
    for (int i = 0; i < MAX_COURSES; i++)
        student->grades[i] = grades[i];
    student->next = next;
    return student;
}

int parseLine(char *line) { //TODO add input validation
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    char tel[MAX_TEL];
    char levelStr[MAX_CLASS_LEN];
    char classStr[MAX_CLASS_LEN];
    int grades[MAX_COURSES];
    long level;
    long class;
    struct Student* student = NULL;
    if (sscanf(line, "%s %s %s %s %s %d %d %d %d %d %d %d %d %d %d",
               firstName, lastName, tel, levelStr, classStr, &grades[0], &grades[1], &grades[2], &grades[3],
               &grades[4], &grades[5], &grades[6], &grades[7], &grades[8], &grades[9]) != NUM_OF_DATA) {
        return 0; // Unable to read all the fields
    }
    level = strtol(levelStr, NULL, DECIMAL);
    class = strtol(classStr, NULL, DECIMAL);

    student = createStudent(firstName, lastName, tel, grades,school.DB[level][class]);
    if(student == NULL){
        printf("malloc failed\n");
        return -1;
    }
    school.DB[level][class] = student;
    return 1;
}

void readFile(FILE *file) {
    char line[MAX_INPUT_LINE];
    while (fgets(line, sizeof(line), file)) {
        parseLine(line);
    }
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
void exportDatabase()
{
    FILE *outputFile = openOutputFile(DB_FILE_PATH);
    loadDatabaseToFile(outputFile);
    fclose(outputFile);
}

void handleClosing()
{
    exportDatabase();
    freeSchool();
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void insertNewStudent()
{
    char line[MAX_INPUT_LINE];
    printf("%s", INSERT_STUDENT_MSG);
    if (fgets(line, sizeof(line), stdin) != NULL)
        parseLine(line);
}

void deleteStudent()
{
    char firstName[MAX_NAME_LEN], lastName[MAX_NAME_LEN];
    printf("%s", DELETE_STUDENT_MSG);
    scanf("%s %s", firstName, lastName);
    printf("\nreceived: <%s> <%s>", firstName, lastName);
    for (int level = 0; level < MAX_LEVELS; level++) {
        for (int class = 0; class < MAX_CLASSES; class++) {
            struct Student *current = school.DB[level][class];
            struct Student *prev = NULL;
            while (current != NULL) {
                if (strcmp(current->firstName, firstName) == 0 &&
                    strcmp(current->lastName, lastName) == 0) { // student found
                    printf("\n<%s> <%s> is found", firstName, lastName);
                    if (prev != NULL) {
                        printf("\nmiddle of the linked list.\nAfter <%s> <%s>", prev->firstName, prev->lastName);
                        prev->next = current->next;
                    } else { // the student is the first node
                        printf("\n<%s> <%s> is head of the linked list", current->firstName, current->lastName);
                        school.DB[level][class] = current->next;
                    }
                    free(current);
                    return;
                }
                prev = current;
                current = current->next;
            }
        }
    }
}

void printAllStudents() {
    for (int level = 0; level < MAX_LEVELS; level++) {
        for (int class = 0; class < MAX_CLASSES; class++) {
            struct Student *student = school.DB[level][class];
            while (student != NULL) {
                printf("%s / %s / %s / %d / %d ", student->firstName,  student->lastName,  student->tel, level, class);
                for(int gradeIndex = 0; gradeIndex < MAX_COURSES;  gradeIndex++)
                    printf("%d ", student->grades[gradeIndex]);
                printf("\n");
                student = student->next;
            }
        }
    }
}

enum menu_inputs { Insert = '0', Delete = '1', Edit = '2', Search = '3', Showall = '4', Sort = '5', Import= '6', Export = '7', Help = '8', Exit = '9' };

void menu() {
    char input;
    size_t numOfStudents = 100;
    do {
//        clrscr();
        printf("\n|School Manager<::>Home|\n");
        printf("--------------------------------------------------------------------------------\n");
        printf("Welcome to ( %s ) School!\nYou have inserted ( %zu ) students.\n\n", "Rabin", numOfStudents);// school->name, school->num_of_students);
        printf("\t[0] |--> Insert\n");
        printf("\t[1] |--> Delete\n");
        printf("\t[2] |--> Edit\n");
        printf("\t[3] |--> Search\n");
        printf("\t[4] |--> Show All\n");
        printf("\t[5] |--> Sort\n");
        printf("\t[6] |--> Import\n");
        printf("\t[7] |--> Export\n");
        printf("\t[8] |--> Help\n");
        printf("\t[9] |--> Exit\n");
        printf("\n\tPlease Enter Your Choice (1-10): ");
        input = getc(stdin);
        fflush(stdin);
        getc(stdin);
        switch (input) {
            case Insert:
                insertNewStudent();
                break;
            case Delete:
                deleteStudent();
                break;
            case Edit:
                printf("Edit Not supported yet.\n");
                break;
            case Search:
                printf("Not supported yet.\n");
                break;
            case Showall:
                printAllStudents();
                break;
            case Sort:
                printf("Sort Not supported yet.\n");
                break;
            case Export:
                exportDatabase();
                break;
            case Help:
//                help();
                printf("Not supported yet.\n");
                break;
            case Exit:
                handleClosing();
                break;
            default:
                printf("\nThere is no item with symbol \"%c\".Please enter a number between 1-10!\nPress any key to continue...", input);
                getc(stdin);
                getc(stdin);
                break;
        }
    } while (input != Exit);
}


int main() {
    init();
    menu();
    handleClosing();
    return 0;
}
