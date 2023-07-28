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
const char *GET_STUDENT_NAME_MSG = "Please enter the first and last name of the student\n";

const int NUM_OF_DATA = 15;
const int DECIMAL = 10;


struct Student {
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    char tel[MAX_TEL];
    int grades[MAX_COURSES];
    struct Student *next;
};
struct StudentCourseNode {
    struct Student *student;
    struct StudentCourseNode *next;
};
struct School {
    char *name;
    struct Student *DB[MAX_LEVELS][MAX_CLASSES];
    struct StudentCourseNode *courses[MAX_LEVELS][MAX_COURSES];
};
static struct School school;

FILE *openOutputFile(const char *fileName) {
    FILE *outputFile;
    outputFile = fopen(fileName, "w");
    if (outputFile == NULL) {
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

void printSchoolCourses() { // todo delete
    printf("SchoolCourses\n");
    for (int level = 0; level < MAX_LEVELS; level++) {
        printf("level: %d\n", level);
        for (int courseNum = 0; courseNum < MAX_COURSES; courseNum++) {
            struct StudentCourseNode *studentCourseNode = school.courses[level][courseNum];
            printf("course: %d\n", courseNum);
            while (studentCourseNode != NULL) {
                printf("[%s %s %d] --> ", studentCourseNode->student->firstName, studentCourseNode->student->lastName,
                       studentCourseNode->student->grades[courseNum]);
                studentCourseNode = studentCourseNode->next;
            }
            printf("--|\n");
        }
    }
}

void addStudentToCourses(struct Student *student, int level) {
    for (int courseNum = 0; courseNum < MAX_COURSES; courseNum++) {
        struct StudentCourseNode* studentCourseNode = (struct StudentCourseNode *) malloc(sizeof(struct StudentCourseNode));
        studentCourseNode->student = student;
        studentCourseNode->next = NULL;
        struct StudentCourseNode *curr = school.courses[level][courseNum];
        struct StudentCourseNode *prev = NULL;
        if (curr == NULL) { //empty list
            school.courses[level][courseNum] = studentCourseNode;
            continue; // to the next course
        }
        while (curr != NULL && student->grades[courseNum] <= curr->student->grades[courseNum]) {
            prev = curr;
            curr = curr->next;
        }
        if (curr == NULL) // reached the end
        {
            prev->next = studentCourseNode;
        } else // head or middle
        {
            if (prev == NULL) // head
            {
                studentCourseNode->next = curr;
                school.courses[level][courseNum] = studentCourseNode;
            } else //middle
            {
                prev->next = studentCourseNode;
                studentCourseNode->next = curr;
            }
        }

    }
}

struct Student *createStudent(char *firstName, char *lastName, char *tel, const int *grades, struct Student *next) {
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
    struct Student *student = NULL;
    if (sscanf(line, "%s %s %s %s %s %d %d %d %d %d %d %d %d %d %d",
               firstName, lastName, tel, levelStr, classStr, &grades[0], &grades[1], &grades[2], &grades[3],
               &grades[4], &grades[5], &grades[6], &grades[7], &grades[8], &grades[9]) != NUM_OF_DATA) {
        return 0; // Unable to read all the fields
    }
    level = strtol(levelStr, NULL, DECIMAL) - 1;
    class = strtol(classStr, NULL, DECIMAL) - 1;

    student = createStudent(firstName, lastName, tel, grades, school.DB[level][class]);
    if (student == NULL) {
        printf("malloc failed\n");
        return -1;
    }
    school.DB[level][class] = student;
    addStudentToCourses(student, level);
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

void loadDatabaseToFile(FILE *outputFile) {
    for (int level = 0; level < MAX_LEVELS; level++) {
        for (int class = 0; class < MAX_CLASSES; class++) {
            struct Student *student = school.DB[level][class];
            while (student != NULL) {
                fprintf(outputFile, "%s %s %s %d %d ",
                        student->firstName, student->lastName, student->tel, level + 1, class + 1);
                for (int gradeIndex = 0; gradeIndex < MAX_COURSES; gradeIndex++)
                    fprintf(outputFile, "%d ", student->grades[gradeIndex]);
                fprintf(outputFile, "\n");

                student = student->next;
            }
        }
    }
}

void exportDatabase() {
    FILE *outputFile = openOutputFile(DB_FILE_PATH);
    loadDatabaseToFile(outputFile);
    fclose(outputFile);
}

void handleClosing() {
    exportDatabase();
    freeSchool();
}

void init() {
    FILE *inputFile = openInputFile(INPUT_FILE_PATH);
    readFile(inputFile);
    fclose(inputFile);
}

void insertNewStudent() {
    char line[MAX_INPUT_LINE];
    printf("%s", INSERT_STUDENT_MSG);
    if (fgets(line, sizeof(line), stdin) != NULL)
        parseLine(line);
}

void deleteStudent() {
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    printf("%s", GET_STUDENT_NAME_MSG);
    scanf("%s %s", firstName, lastName); //TODO validation
    for (int level = 0; level < MAX_LEVELS; level++) {
        for (int class = 0; class < MAX_CLASSES; class++) {
            struct Student *current = school.DB[level][class];
            struct Student *prev = NULL;
            while (current != NULL) {
                if (strcmp(current->firstName, firstName) == 0 &&
                    strcmp(current->lastName, lastName) == 0) { // student found
                    if (prev != NULL) {
                        prev->next = current->next;
                    } else { // the student is the first node
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

void printStudent(struct Student *student, int level, int class) {
    if (student == NULL)
        return;
    printf("%s %s %s %d %d ", student->firstName, student->lastName, student->tel, level + 1, class + 1);
    for (int gradeIndex = 0; gradeIndex < MAX_COURSES; gradeIndex++)
        printf("%d ", student->grades[gradeIndex]);
    printf("\n");

}

void printAllStudents() {
    for (int level = 0; level < MAX_LEVELS; level++) {
        for (int class = 0; class < MAX_CLASSES; class++) {
            struct Student *student = school.DB[level][class];
            while (student != NULL) {
                printStudent(student, level, class);
                student = student->next;
            }
        }
    }
}

struct Student *getStudentByName(char *firstName, char *lastName) {
    for (int level = 0; level < MAX_LEVELS; level++) {
        for (int class = 0; class < MAX_CLASSES; class++) {
            struct Student *student = school.DB[level][class];
            while (student != NULL) {
                if (strcmp(student->firstName, firstName) == 0 &&
                    strcmp(student->lastName, lastName) == 0) { // student found
                    return student;
                }
                student = student->next;
            }
        }
    }
    return NULL;
}

void searchStudent() {
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    printf("%s", GET_STUDENT_NAME_MSG);
    scanf("%s %s", firstName, lastName); //TODO validation
    getc(stdin); // read the newline after student's name
    struct Student *student = getStudentByName(firstName, lastName);
    printStudent(student, 0, 0); // todo level&class
}

void editStudentGrade() { //TODO validation
    char courseStr[MAX_NAME_LEN];
    char gradeStr[MAX_NAME_LEN];
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    printf("%s", GET_STUDENT_NAME_MSG);
    scanf("%s %s", firstName, lastName);
    printf("Please enter the course number and grade: ");
    scanf("%s %s", courseStr, gradeStr);
    getc(stdin);
    long course = strtol(courseStr, NULL, DECIMAL) - 1;
    long grade = strtol(gradeStr, NULL, DECIMAL);

    struct Student *student = getStudentByName(firstName, lastName);
    student->grades[course] = grade;
}


enum menu_inputs {
    Insert = '0',
    Delete = '1',
    Edit = '2',
    Search = '3',
    Showall = '4',
    Sort = '5',
    Import = '6',
    Export = '7',
    Help = '8',
    Exit = '9'
};

void menu() {
    char input;
    size_t numOfStudents = 100;
    school.name = "Rabin";
    do {
//        clrscr();
//        system("clear");
        printf("\n|School Manager<::>Home|\n");
        printf("--------------------------------------------------------------------------------\n");
        printf("Welcome to ( %s ) School!\nYou have inserted ( %zu ) students.\n\n", school.name,
               numOfStudents);// school->name, school->num_of_students);
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
        printf("\n\tPlease Enter Your Choice (0-9): ");
        input = getc(stdin);
        fflush(stdin);
        getc(stdin);
        switch (input) {
            case Insert:
                insertNewStudent();
                break;
            case Delete:
                deleteStudent();
                getc(stdin); // read the new line after student's name
                break;
            case Edit:
                editStudentGrade();
                break;
            case Search:
                searchStudent();
                break;
            case Showall:
                printAllStudents();
                printSchoolCourses();
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
                printf("\nThere is no item with symbol \"%c\".Please enter a number between 1-10!\nPress any key to continue...",
                       input);
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
