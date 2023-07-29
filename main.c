#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>



#define MAX_NAME_LEN 20
#define MAX_TEL 12
#define MAX_CLASSES 10
#define MAX_COURSES 10
#define MAX_CLASS_LEN 10
#define MAX_LEVELS 12
#define MAX_INPUT_LINE 200
#define TOP_N_STUDENTS 10
#define MINIMUM_NUM_OF_FAILS 5
#define COURSE_PASS_GRADE 55

const char *INPUT_FILE_PATH = "resources/studentsDB.bin";
const char *DB_FILE_PATH = "resources/studentsDB.bin";
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
    size_t numOfStudents;
    struct Student *DB[MAX_LEVELS][MAX_CLASSES];
    struct StudentCourseNode *courses[MAX_LEVELS][MAX_COURSES];
};
static struct School school;

/* A 256 bit key */
unsigned char myKey[] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                       0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
                       0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
                       0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31
};

/* A 128 bit IV */
unsigned char myIv[] = { 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                      0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35
};

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;
    if(!(ctx = EVP_CIPHER_CTX_new())) {
        handleErrors();
    }
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        handleErrors();
    }
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) {
        handleErrors();
    }
    ciphertext_len = len;
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
        handleErrors();
    }
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        handleErrors();
    }
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) {
        handleErrors();
    }
    plaintext_len = len;

    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
        handleErrors();
    }
    plaintext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    return plaintext_len;
}


FILE *openOutputFile(const char *fileName) {
    FILE *outputFile;
    outputFile = fopen(fileName, "wb");
    if (outputFile == NULL) {
        perror("Can not open file to save the database\n");
        exit(1);
    }
    return outputFile;
}

FILE *openInputFile(const char *fileName) {
    FILE *inputFile;
    inputFile = fopen(fileName, "rb");
    if (inputFile == NULL) {
        perror("Can not open input file\n");
        exit(1);
    }
    return inputFile;
}

void addStudentToCourse(struct Student *student, int level, int course) {
    struct StudentCourseNode *studentCourseNode = (struct StudentCourseNode *) malloc(sizeof(struct StudentCourseNode));
    studentCourseNode->student = student;
    studentCourseNode->next = NULL;
    struct StudentCourseNode *current = school.courses[level][course];
    struct StudentCourseNode *prev = NULL;
    //TODO refactor
    if (current == NULL) { //empty list
        school.courses[level][course] = studentCourseNode;
        return;
    }
    while (current != NULL && student->grades[course] <= current->student->grades[course]) {
        prev = current;
        current = current->next;
    }
    if (current == NULL) // reached the end
    {
        prev->next = studentCourseNode;
    } else // head or middle
    {
        if (prev == NULL) // head
        {
            studentCourseNode->next = current;
            school.courses[level][course] = studentCourseNode;
        } else //middle
        {
            prev->next = studentCourseNode;
            studentCourseNode->next = current;
        }
    }
}

void addStudentToAllCourses(struct Student *student, int level) {
    for (int courseNum = 0; courseNum < MAX_COURSES; courseNum++) {
        addStudentToCourse(student, level, courseNum);
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
    addStudentToAllCourses(student, level);
    school.numOfStudents++;
    return 1;
}

void readFile(FILE *file) {
    int encryptedLen;
    unsigned char encryptedLine[MAX_INPUT_LINE];
    unsigned char decryptedLine[MAX_INPUT_LINE];
    while(fread(&encryptedLen, sizeof(encryptedLen), 1, file) > 0){
        fread(encryptedLine, sizeof(encryptedLine[0]), encryptedLen, file);
        decrypt(encryptedLine, encryptedLen, myKey, myIv, decryptedLine);
        parseLine(decryptedLine);
    }
}

void freeClassStudents(struct Student *student) {
    while (student != NULL) {
        struct Student *temp = student;
        student = student->next;
        free(temp);
    }
}

void freeDB() {
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
void freeCourseStudents(struct StudentCourseNode *studentCourseNode) {
    while (studentCourseNode != NULL) {
        struct StudentCourseNode *temp = studentCourseNode;
        studentCourseNode = studentCourseNode->next;
        free(temp);
    }
}
void freeCourses() {
    for (int level = 0; level < MAX_LEVELS; level++) {
        for (int course = 0; course < MAX_COURSES; course++) {
            struct StudentCourseNode* studentCourseNode = school.courses[level][course];
            if(studentCourseNode != NULL)
            {
                freeCourseStudents(studentCourseNode);
                school.courses[level][course] = NULL;
            }
        }
    }
}

void exportDatabaseToFile(FILE *outputFile) {
    unsigned char line[MAX_INPUT_LINE];
    unsigned char encryptedLine[MAX_INPUT_LINE];
    for (int level = 0; level < MAX_LEVELS; level++) {
        for (int class = 0; class < MAX_CLASSES; class++) {
            struct Student *student = school.DB[level][class];
            while (student != NULL) {
                sprintf((char *)line, "%s %s %s %d %d %d %d %d %d %d %d %d %d %d %d",
                        student->firstName, student->lastName, student->tel, level + 1, class + 1,
                        student->grades[0], student->grades[1], student->grades[2], student->grades[3],
                        student->grades[4], student->grades[5], student->grades[6], student->grades[7],
                        student->grades[8], student->grades[9]);
                int encryptedLen = encrypt(line, strlen ((char *)line) + 1, myKey, myIv, encryptedLine);
                fwrite(&encryptedLen, sizeof(encryptedLen),1, outputFile);
                fwrite(encryptedLine, sizeof(encryptedLine[0]), encryptedLen, outputFile);
                student = student->next;
            }
        }
    }
}

void exportDatabase() {
    FILE *outputFile = openOutputFile(DB_FILE_PATH);
    exportDatabaseToFile(outputFile);
    fclose(outputFile);
}

void handleClosing() {
    freeCourses();
    freeDB();
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

void deleteStudentCourse(struct Student *student, int level, int course) {
    struct StudentCourseNode *current = school.courses[level][course];
    struct StudentCourseNode *prev = NULL;
    while (current != NULL &&
           strcmp(current->student->firstName, student->firstName) != 0 &&
           strcmp(current->student->lastName, student->lastName) != 0
            ) {
        prev = current;
        current = current->next;
    }
    if (prev == NULL) {
        // student is the first in the course's grades list
        school.courses[level][course] = current->next;
    } else {
        prev->next = current->next;
    }
    free(current); // delete the student's course node
}

void deleteAllStudentCourses(struct Student *student, int level) {
    for (int courseNum = 0; courseNum < MAX_COURSES; courseNum++) {
        deleteStudentCourse(student, level, courseNum);
    }
}

void deleteStudent() {
    char firstName[MAX_NAME_LEN];
    char lastName[MAX_NAME_LEN];
    printf("%s", GET_STUDENT_NAME_MSG);
    scanf("%s %s", firstName, lastName); //TODO validation
    getc(stdin); // read the new line after student's name
    for (int level = 0; level < MAX_LEVELS; level++) {
        for (int class = 0; class < MAX_CLASSES; class++) {
            struct Student *current = school.DB[level][class];
            struct Student *prev = NULL;
            while (current != NULL) {
                if (strcmp(current->firstName, firstName) == 0 &&
                    strcmp(current->lastName, lastName) == 0) { // student found
                    deleteAllStudentCourses(current, level);
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

struct Student *getStudentByName(char *firstName, char *lastName, int *studentLevel, int *studentClass) {
    for (int level = 0; level < MAX_LEVELS; level++) {
        for (int class = 0; class < MAX_CLASSES; class++) {
            struct Student *student = school.DB[level][class];
            while (student != NULL) {
                if (strcmp(student->firstName, firstName) == 0 &&
                    strcmp(student->lastName, lastName) == 0) { // student found
                    *studentLevel = level;
                    *studentClass = class;
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
    int level = -1;
    int class = -1;
    struct Student *student = getStudentByName(firstName, lastName, &level, &class);
    printStudent(student, level, class);
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
    int level = -1;
    int class = -1;
    struct Student *student = getStudentByName(firstName, lastName, &level, &class);
    if (student != NULL) {
        student->grades[course] = grade;
        deleteStudentCourse(student, level, course);
        addStudentToCourse(student, level, course);
    }
}

void printTopNStudentsPerCourse() {
    char courseStr[MAX_NAME_LEN];
    printf("Please enter the course number: ");
    scanf("%s", courseStr);
    getc(stdin);
    long course = strtol(courseStr, NULL, DECIMAL) - 1;
    for (int level = 0; level < MAX_LEVELS; level++) {
        struct StudentCourseNode *studentCourseNode = school.courses[level][course];
        if (studentCourseNode == NULL)
            continue;
        printf("Level %d: \n", level + 1);
        for (int studentIndex = 0; studentIndex < TOP_N_STUDENTS; studentIndex++) {
            if (studentCourseNode == NULL)
                break;
            printf("%d. %d %s %s\n",
                   studentIndex + 1,
                   studentCourseNode->student->grades[course],
                   studentCourseNode->student->firstName,
                   studentCourseNode->student->lastName);
            studentCourseNode = studentCourseNode->next;
        }
    }
}
void printAverage()
{
    for (int level = 0; level < MAX_LEVELS; level++) {
        printf("Level %d: \n", level + 1);
        for (int course = 0; course < MAX_COURSES; course++) {
            struct StudentCourseNode *studentCourseNode = school.courses[level][course];
            int sum = 0;
            int numStudentLevel = 0;
            while(studentCourseNode != NULL) {
                numStudentLevel++;
                sum += studentCourseNode->student->grades[course];
                studentCourseNode = studentCourseNode->next;
            }
            if(numStudentLevel == 0)
                printf("The course %d is empty for this level\n", course+1);
            else
                printf("Course %d: Average %d\n", course + 1, sum/numStudentLevel);
        }
    }
}

void printUnderperformedStudents()
{
    for (int level = 0; level < MAX_LEVELS; level++) {
        for (int class = 0; class < MAX_CLASSES; class++) {
            struct Student* student = school.DB[level][class];
            while(student != NULL) {
                int sumFail = 0;
                for(int course = 0; course < MAX_COURSES; course++)
                {
                    if(student->grades[course] < COURSE_PASS_GRADE)
                        sumFail++;
                }
                if(sumFail >= MINIMUM_NUM_OF_FAILS)
                    printf("Level %d Class %d: %s %s\n", level+1, class+1, student->firstName, student->lastName);

                student = student->next;
            }

        }
    }
}
enum menu_inputs {
    Insert = '0',
    Delete = '1',
    Edit = '2',
    Search = '3',
    Showall = '4',
    Top10 = '5',
    UnderperformedStudents = '6',
    Average = '7',
    Export = '8',
    Exit = '9'
};

void menu() {
    char input;
    school.name = "Rabin";
    do {
        printf("\n|School Manager<::>Home|\n");
        printf("--------------------------------------------------------------------------------\n");
        printf("Welcome to ( %s ) School!\nYou have inserted ( %zu ) students.\n\n", school.name,
               school.numOfStudents);
        printf("\t[0] |--> Insert\n");
        printf("\t[1] |--> Delete\n");
        printf("\t[2] |--> Edit\n");
        printf("\t[3] |--> Search\n");
        printf("\t[4] |--> Show All\n");
        printf("\t[5] |--> Top 10 students per course\n");
        printf("\t[6] |--> Underperformed students\n");
        printf("\t[7] |--> Average per course\n");
        printf("\t[8] |--> Export\n");
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
                break;
            case Edit:
                editStudentGrade();
                break;
            case Search:
                searchStudent();
                break;
            case Showall:
                printAllStudents();
                break;
            case Top10:
                printTopNStudentsPerCourse();
                break;
            case UnderperformedStudents:
                printUnderperformedStudents();
                break;
            case Average:
                printAverage();
                break;
            case Export:
                exportDatabase();
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
