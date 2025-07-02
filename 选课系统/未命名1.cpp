#include <stdio.h>          // ��׼���������
#include <stdlib.h>         // ��׼�⺯��
#include <string.h>         // �ַ���������

// ����ϵͳ����
#define MAX_SELECT_NUM 10   // ÿ��ѧ��ѡ����������
#define MAX_COURSE_NUM 100  // ϵͳ�пγ���������
#define MAX_STU_NUM 1000    // ϵͳ��ѧ����������
#define MAX_ADMIN_NUM 10    // ϵͳ�й���Ա��������
#define MAX_COURSE_TITLE 50 // �γ����Ƴ�������
#define MAX_NAME 20         // ������������
#define MAX_PWD 20          // ���볤������

// �γ̽ṹ�嶨��
typedef struct
{
    int ID;                         // �γ̱��
    char title[MAX_COURSE_TITLE];   // �γ�����
    char teacher[MAX_NAME];         // ��ʦ����
    int maxNum;                     // �ÿγ��������ɵ�ѧ������
    int selectNum;                  // ��ѡ�ÿγ̵�ѧ������
}Course;

// �û��ṹ�嶨�壨����ѧ���û��͹���Ա�û���
typedef struct
{
    int ID;                                 // �û�ID��ѧ�Ż����Ա��ţ�
    char name[MAX_NAME];                    // �û�����
    char pwd[MAX_PWD];                      // �û�����
    int type;                               // �û����ͣ�1Ϊ����Ա��2Ϊѧ��
    int selectNum;                          // ��ѡ�γ�����
    int selectCourse[MAX_SELECT_NUM];       // ��ѡ�γ̱������
}User;

// ��������
int InputCourse(Course course[]);                                               // ����γ���Ϣ
int InputStudent(User stu[]);                                                  // ����ѧ����Ϣ
int InputAdmin(User ad[]);                                                      // �������Ա��Ϣ
User* login(User admin[], int adminNum, User stu[], int stuNum);               // �û���¼
int stuOperate(User* stu, Course course[], int courseNum);                     // ѧ����������
void showCourses(Course course[], int courseNum);                              // ��ʾ���пγ�
void showSelectedCourses(User* stu, Course course[], int courseNum);           // ��ʾ��ѡ�γ�
Course* findCourse(int courseID, Course course[], int courseNum, int* index); // ���ݿγ̱�Ų��ҿγ̲���������
int select(User* stu, Course course[], int courseNum);                         // ѡ��
int drop(User* stu, Course course[], int courseNum);                           // �˿�
int adminOperate(User* ad, Course course[], int* courseNum, User stu[], int* stuNum, User admin[], int adminNum); // ����Ա��������
int addCourse(Course course[], int* courseNum);                                // ��ӿγ�
int delCourse(Course course[], int* courseNum, User stu[], int stuNum);        // ɾ���γ�
int editCourse(Course course[], int courseNum);                                // �޸Ŀγ���Ϣ
int courseDetail(Course course[], int courseNum, User stu[], int stuNum);      // �鿴�γ�����
void showStu(User stu[], int stuNum);                                          // ��ʾ����ѧ����Ϣ
void showAdmin(User admin[], int adminNum);                                    // ��ʾ���й���Ա��Ϣ
int addStu(User stu[], int* stuNum, User admin[], int adminNum);               // ���ѧ��
int findStu(int id, User stu[], int stuNum);                                   // ����ѧ�Ų���ѧ��
int delStu(Course course[], int courseNum, User stu[], int* stuNum);           // ɾ��ѧ��
int editStu(User stu[], int stuNum);                                           // �޸�ѧ����Ϣ
int selectByAdmin(User stu[], int stuNum, Course course[], int courseNum);     // ����ԱΪѧ��ѡ��
int dropByAdmin(User stu[], int stuNum, Course course[], int courseNum);       // ����ԱΪѧ���˿�

// ������
int main(int argc, char *argv[]) {
    int stuNum = 0;         // ѧ������
    int adminNum = 0;       // ����Ա����
    int courseNum = 0;      // �γ�����
    int userType = 0;       // �û����ͣ�1Ϊ����Ա��2Ϊѧ����0Ϊδ��¼
    User* currentUser = NULL; // ��ǰ��¼�û�ָ��

    // ��������洢ϵͳ����
    Course course[MAX_COURSE_NUM];  // �γ�����
    User admin[MAX_ADMIN_NUM];      // ����Ա����
    User stu[MAX_STU_NUM];          // ѧ������

    // ��ʼ��ϵͳ����
    courseNum = InputCourse(course);    // ����γ���Ϣ
    stuNum = InputStudent(stu);         // ����ѧ����Ϣ
    adminNum = InputAdmin(admin);       // �������Ա��Ϣ

    // ϵͳ��ѭ��
    while(1)
    {
        if(userType == 0)   // �û�δ��¼״̬
        {
            currentUser = login(admin, adminNum, stu, stuNum);  // �û���¼
            if(currentUser != NULL)
                userType = currentUser->type;   // �����û�����
        }
        else if(currentUser->type == 1) // ����Ա�û�
            userType = adminOperate(currentUser, course, &courseNum, stu, &stuNum, admin, adminNum);
        else if(currentUser->type == 2) // ѧ���û�
            userType = stuOperate(currentUser, course, courseNum);
    }

    return 0;
}

// ����γ���Ϣ����
int InputCourse(Course course[])
{
    int i = 0;                          // ������
    FILE *file;                         // �ļ�ָ��
    char buffer[200];                   // ������
    
    file = fopen("course.txt", "r");    // �򿪿γ���Ϣ�ļ�
    if (file == NULL) {                 // �ļ���ʧ��
        printf("�޷���course.txt�ļ�\n");
        return 0;
    }
    
    // �����ļ�ͷ��������
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        // ��ȡ�γ���Ϣ
        while (fscanf(file, "%d %s %s %d", &course[i].ID, course[i].title, 
                     course[i].teacher, &course[i].maxNum) == 4) {
            course[i].selectNum = 0;    // ��ʼ����ѡ����Ϊ0
            i++;                        // �γ̼�������
            if (i >= MAX_COURSE_NUM) break; // ��ֹ����Խ��
        }
    }
    
    fclose(file);                       // �ر��ļ�
    printf("����γ̳ɹ���������%d�ſγ�\n", i);
    return i;                           // ���ؿγ�����
}

// ����ѧ����Ϣ����
int InputStudent(User stu[])
{
    int i = 0;                          // ������
    FILE *file;                         // �ļ�ָ��
    char buffer[200];                   // ������
    
    file = fopen("student.txt", "r");   // ��ѧ����Ϣ�ļ�
    if (file == NULL) {                 // �ļ���ʧ��
        printf("�޷���student.txt�ļ�\n");
        return 0;
    }
    
    // �����ļ�ͷ��������
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        // ��ȡѧ����Ϣ
        while (fscanf(file, "%d %s %s", &stu[i].ID, stu[i].name, stu[i].pwd) == 3) {
            stu[i].selectNum = 0;       // ��ʼ����ѡ�γ�����Ϊ0
            stu[i].type = 2;            // �����û�����Ϊѧ��
            i++;                        // ѧ����������
            if (i >= MAX_STU_NUM) break; // ��ֹ����Խ��
        }
    }
    
    fclose(file);                       // �ر��ļ�
    printf("����ѧ����Ϣ�ɹ���������%d��ѧ��\n", i);
    return i;                           // ����ѧ������
}

// �������Ա��Ϣ����
int InputAdmin(User ad[])
{
    int i = 0;                          // ������
    FILE *file;                         // �ļ�ָ��
    char buffer[200];                   // ������
    
    file = fopen("admin.txt", "r");     // �򿪹���Ա��Ϣ�ļ�
    if (file == NULL) {                 // �ļ���ʧ��
        printf("�޷���admin.txt�ļ�\n");
        return 0;
    }
    
    // �����ļ�ͷ��������
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        // ��ȡ����Ա��Ϣ
        while (fscanf(file, "%d %s %s", &ad[i].ID, ad[i].name, ad[i].pwd) == 3) {
            ad[i].type = 1;             // �����û�����Ϊ����Ա
            i++;                        // ����Ա��������
            if (i >= MAX_ADMIN_NUM) break; // ��ֹ����Խ��
        }
    }
    
    fclose(file);                       // �ر��ļ�
    printf("�������Ա��Ϣ�ɹ���������%d������Ա\n", i);
    return i;                           // ���ع���Ա����
}

// �û���¼����
User* login(User admin[], int adminNum, User stu[], int stuNum)
{
    int id;                             // �û�ID
    char pwd[21];                       // ����
    
    while(1)
    {
        printf("�������û�����");
        scanf("%d", &id);               // �����û�ID
        printf("���������룺");
        scanf("%s", pwd);               // ��������
        
        // ����Ƿ�Ϊ����Ա
        for (int i = 0; i < adminNum; i++)
        {
            if(id == admin[i].ID && strcmp(pwd, admin[i].pwd) == 0)
            {
                printf("����Ա��¼�ɹ�����ӭ %s\n", admin[i].name);
                return &admin[i];       // ���ع���Աָ��
            }
        }
        
        // ����Ƿ�Ϊѧ��
        for (int i = 0; i < stuNum; i++)
        {
            if(id == stu[i].ID && strcmp(pwd, stu[i].pwd) == 0)
            {
                printf("ѧ����¼�ɹ�����ӭ %s\n", stu[i].name);
                return &stu[i];         // ����ѧ��ָ��
            }
        }
        
        printf("�û���������������������룡\n");
    }
    
    return NULL;                        // ��¼ʧ�ܷ���NULL
}

// ѧ���������溯��
int stuOperate(User* stu, Course course[], int courseNum)
{
    int option;                         // ѡ��
    
    while(1)
    {
        // ��ʾѧ���˵�
        printf("==== ѧ�ţ�%d ������%s ====\n", stu->ID, stu->name);
        printf(" 1.�鿴���пγ�\n");
        printf(" 2.�鿴��ѡ�γ�\n");
        printf(" 3.ѡ��\n");
        printf(" 4.�˿�\n");
        printf(" 5.�˳���¼\n");
        printf("======================================\n");
        printf("��ѡ��");
        scanf("%d", &option);           // ����ѡ��
        
        // ����ѡ��ִ����Ӧ����
        switch(option)
        {
            case 1:
                showCourses(course, courseNum);                 // �鿴���пγ�
                break;
            case 2:
                showSelectedCourses(stu, course, courseNum);    // �鿴��ѡ�γ�
                break;
            case 3:
                select(stu, course, courseNum);                 // ѡ��
                break;
            case 4:
                drop(stu, course, courseNum);                   // �˿�
                break;
            case 5:
                printf("�˳���¼�ɹ���\n");
                return 0;                                       // �˳���¼
            default:
                printf("������Ч��������ѡ��\n");
                break;
        }
    }
}

// ��ʾ���пγ̺���
void showCourses(Course course[], int courseNum)
{
    printf("====�γ̱�� �γ�����                �ον�ʦ   ������� ��ѡ����====\n");
    for(int i = 0; i < courseNum; i++)
    {
        // ��ʽ������γ���Ϣ����֤�ж���
        printf("%10d %-20s %-10s %8d %8d\n", 
               course[i].ID, course[i].title, course[i].teacher, 
               course[i].maxNum, course[i].selectNum);
    }
    printf("========================================================================\n");
}

// ��ʾѧ����ѡ�γ̺���
void showSelectedCourses(User* stu, Course course[], int courseNum)
{
    if(stu->selectNum == 0)             // δѡ���κογ�
    {
        printf("����û��ѡ���κογ̣�\n");
        return;
    }
    
    printf("====�γ̱�� �γ�����                �ον�ʦ   ������� ��ѡ����====\n");
    for(int i = 0; i < stu->selectNum; i++)
    {
        Course * c = findCourse(stu->selectCourse[i], course, courseNum, NULL);
        if(c != NULL)
        {
            // ��ʽ�������ѡ�γ���Ϣ
            printf("%10d %-20s %-10s %8d %8d\n", 
                   c->ID, c->title, c->teacher, c->maxNum, c->selectNum);
        }
    }
    printf("========================================================================\n");
}

// ��ʾ����ѧ����Ϣ����
void showStu(User stu[], int stuNum)
{
    printf("====    ѧ��      ����      ����  ��ѡ�γ��� ��ѡ�γ� ====\n");
    for(int i = 0; i < stuNum; i++)
    {
        printf("%10d %10s %10s %10d ", 
               stu[i].ID, stu[i].name, stu[i].pwd, stu[i].selectNum);
        
        // �����ѡ�γ̱��
        for(int j = 0; j < stu[i].selectNum; j++)
        {
            printf("%d ", stu[i].selectCourse[j]);
        }
        printf("\n");
    }
    printf("============================================================\n");
}

// ��ʾ���й���Ա��Ϣ����
void showAdmin(User admin[], int adminNum)
{
    printf("====    �˺�      ����      ���� ====\n");
    for(int i = 0; i < adminNum; i++)
    {
        printf("%10d %10s %10s\n", 
               admin[i].ID, admin[i].name, admin[i].pwd);
    }
    printf("=====================================\n");
}

// �ѱ� findCourse(int courseID, Course course[], int courseNum, int* index) ����������ظ�����

// ���ݿγ̱�Ų��ҿγ̲�������������
Course * findCourse(int courseID, Course course[], int courseNum, int* index)
{
    for(int i = 0; i < courseNum; i++)
    {
        if(course[i].ID == courseID)    // �ҵ�ƥ��Ŀγ�
        {
            if(index) *index = i;       // ��������
            return &course[i];          // ���ؿγ�ָ��
        }
    }
    return NULL;                        // δ�ҵ�����NULL
}

// ѧ��ѡ�κ���
int select(User* stu, Course course[], int courseNum)
{
    int num;                            // �γ̱��
    
    if(stu->selectNum >= MAX_SELECT_NUM) // ѡ�������Ѵ�����
    {
        printf("ѡ�������Ѵ����ޣ��޷�ѡ�Σ�\n");
        return 0;
    }
    
    while(1)
    {
        printf("������γ̱�ţ���Ҫȡ��������0��");
        scanf("%d", &num);              // ����γ̱��
        
        if(num == 0)                    // ȡ������
            return 0;
        
        Course* c = findCourse(num, course, courseNum, NULL); // ���ҿγ�
        if(c == NULL)                   // �γ̲�����
        {
            printf("�γ̲����ڣ����������룡\n");
        }
        else
        {
            if(c->selectNum >= c->maxNum) // �γ���������
            {
                printf("�ÿγ�������������ѡ�������γ̣�\n");
            }
            else
            {
                // ����Ƿ���ѡ��ÿγ�
                int i;
                for(i = 0; i < stu->selectNum; i++)
                {
                    if(stu->selectCourse[i] == c->ID)
                    {
                        printf("��ѡ�ÿγ̣������ظ�ѡ��\n");
                        return 0;
                    }
                }
                
                // ִ��ѡ�β���
                stu->selectCourse[stu->selectNum] = c->ID;  // ��ӿγ̵�ѧ��ѡ���б�
                stu->selectNum++;                           // ѧ��ѡ����������
                c->selectNum++;                             // �γ�ѡ����������
                printf("ѡ�γɹ�����ѡ�γ� %d-%s\n", c->ID, c->title);
                return 1;
            }
        }
    }
}

// ѧ���˿κ���
int drop(User* stu, Course course[], int courseNum)
{
    int num;                            // �γ̱��
    
    if(stu->selectNum <= 0)             // δѡ���κογ�
    {
        printf("��δѡ���κογ�\n");
        return 0;
    }
    
    while(1)
    {
        printf("������Ҫ�˵Ŀγ̱�ţ���Ҫȡ��������0��");
        scanf("%d", &num);              // ����γ̱��
        
        if(num == 0)                    // ȡ������
            return 0;
        
        Course* c = findCourse(num, course, courseNum, NULL); // ���ҿγ�
        if(c == NULL)                   // �γ̲�����
        {
            printf("�γ̲����ڣ����������룡\n");
        }
        else
        {
            // ����ѧ���Ƿ�ѡ���˸ÿγ�
            int i;
            for(i = 0; i < stu->selectNum; i++)
            {
                if(stu->selectCourse[i] == c->ID)
                    break;
            }
            
            if(i == stu->selectNum)     // δѡ��˿γ�
                printf("δѡ��˿γ̣����������룡\n");
            else
            {
                // ִ���˿β���
                for(int j = i; j < stu->selectNum - 1; j++)
                {
                    stu->selectCourse[j] = stu->selectCourse[j + 1]; // �ƶ�����Ԫ��
                }
                stu->selectNum--;       // ѧ��ѡ����������
                c->selectNum--;         // �γ�ѡ����������
                printf("�˿γɹ������˿γ� %d-%s\n", c->ID, c->title);
                return 1;
            }
        }
    }
}

// ����Ա�������溯��
int adminOperate(User* ad, Course course[], int* courseNum, User stu[], int* stuNum, User admin[], int adminNum)
{
    int option;                         // ѡ��

    while(1)
    {
        // ��ʾ����Ա�˵�
        printf("==== �˺ţ�%d ������%s ====\n", ad->ID, ad->name);
        printf(" 1.�鿴���пγ�\n");
        printf(" 2.�鿴�γ�����\n");
        printf(" 3.��ӿγ�\n");
        printf(" 4.ɾ���γ�\n");
        printf(" 5.�޸Ŀγ���Ϣ\n");
        printf(" 6.�鿴����ѧ����Ϣ\n");
        printf(" 7.���ѧ��\n");
        printf(" 8.ɾ��ѧ��\n");
        printf(" 9.�޸�ѧ����Ϣ\n");
        printf(" 10.Ϊѧ��ѡ��\n");
        printf(" 11.Ϊѧ���˿�\n");
        printf(" 12.�鿴���й���Ա��Ϣ\n");
        printf(" 13.�˳���¼\n");
        printf("======================================\n");
        printf("��ѡ��");
        scanf("%d", &option);           // ����ѡ��

        // ����ѡ��ִ����Ӧ����
        switch(option)
        {
            case 1:
                showCourses(course, *courseNum);                         // �鿴���пγ�
                break;
            case 2:
                courseDetail(course, *courseNum, stu, *stuNum);           // �鿴�γ�����
                break;
            case 3:
                addCourse(course, courseNum);                           // ��ӿγ�
                break;
            case 4:
                delCourse(course, courseNum, stu, *stuNum);              // ɾ���γ�
                break;
            case 5:
                editCourse(course, *courseNum);                          // �޸Ŀγ���Ϣ
                break;
            case 6:
                showStu(stu, *stuNum);                                   // �鿴����ѧ����Ϣ
                break;
            case 7:
                addStu(stu, stuNum, admin, adminNum);                   // ���ѧ��
                break;
            case 8:
                delStu(course, *courseNum, stu, stuNum);                 // ɾ��ѧ��
                break;
            case 9:
                editStu(stu, *stuNum);                                   // �޸�ѧ����Ϣ
                break;
            case 10:
                selectByAdmin(stu, *stuNum, course, *courseNum);          // Ϊѧ��ѡ��
                break;
            case 11:
                dropByAdmin(stu, *stuNum, course, *courseNum);            // Ϊѧ���˿�
                break;
            case 12:
                showAdmin(admin, adminNum);                             // �鿴���й���Ա��Ϣ
                break;
            case 13:
                printf("�˳���¼�ɹ���\n");
                return 0;                                               // �˳���¼
            default:
                printf("������Ч��������ѡ��\n");
                break;
        }
    }
    return 0;
}

// ��ӿγ̺���
int addCourse(Course course[], int* courseNum)
{
    Course newCourse;                   // �¿γ̽ṹ��

    if(*courseNum >= MAX_COURSE_NUM)    // �γ������Ѵ�����
    {
        printf("�γ������Ѵ����ޣ��޷�����¿γ̣�\n");
        return 0;
    }

    printf("������γ̱�ţ�");
    scanf("%d", &newCourse.ID);         // ����γ̱��

    // ���γ̱���Ƿ��ظ�
    for(int i = 0; i < *courseNum; i++)
    {
        if(course[i].ID == newCourse.ID)
        {
            printf("�γ̱���Ѵ��ڣ����ʧ�ܣ�\n");
            return 0;
        }
    }

    printf("������γ����ƣ�");
    scanf("%s", newCourse.title);       // ����γ�����
    printf("�������ον�ʦ��");
    scanf("%s", newCourse.teacher);     // �����ον�ʦ
    printf("���������������");
    scanf("%d", &newCourse.maxNum);     // �����������
    newCourse.selectNum = 0;            // ��ʼ����ѡ����Ϊ0

    course[*courseNum] = newCourse;     // ����¿γ̵�����
    (*courseNum)++;                     // �γ���������
    printf("��ӿγ̳ɹ����γ̱�ţ�%d���γ����ƣ�%s\n", newCourse.ID, newCourse.title);
    return 1;
}

// ɾ���γ̺���
int delCourse(Course course[], int* courseNum, User stu[], int stuNum)
{
    int courseID;                       // Ҫɾ���Ŀγ̱��
    int index;                          // �γ��������е�����

    if(*courseNum <= 0)                 // ��ǰ�����ڿγ�
    {
        printf("��ǰ�����ڿγ̣�\n");
        return 0;
    }

    printf("������Ҫɾ���Ŀγ̱�ţ�");
    scanf("%d", &courseID);             // ����γ̱��

    Course* c = findCourse(courseID, course, *courseNum, &index); // ���ҿγ�
    if(c == NULL)                       // �γ̲�����
    {
        printf("�γ̲����ڣ�\n");
        return 0;
    }

    // ������ѧ����ѡ���б���ɾ���ÿγ�
    for(int i = 0; i < stuNum; i++)
    {
        for(int j = 0; j < stu[i].selectNum; j++)
        {
            if(stu[i].selectCourse[j] == courseID)
            {
                // �Ƴ��ÿγ�
                for(int k = j; k < stu[i].selectNum - 1; k++)
                {
                    stu[i].selectCourse[k] = stu[i].selectCourse[k + 1];
                }
                stu[i].selectNum--;     // ѧ��ѡ����������
                j--;                    // ��������
            }
        }
    }

    // �ӿγ�������ɾ���ÿγ�
    for(int i = index; i < *courseNum - 1; i++)
    {
        course[i] = course[i + 1];      // �ƶ�����Ԫ��
    }
    (*courseNum)--;                     // �γ���������

    printf("ɾ���γ̳ɹ����γ̱�ţ�%d\n", courseID);
    return 1;
}

// �޸Ŀγ���Ϣ����
int editCourse(Course course[], int courseNum)
{
    int courseID;                       // �γ̱��
    
    if(courseNum <= 0)                  // ��ǰ�����ڿγ�
    {
        printf("��ǰ�����ڿγ̣�\n");
        return 0;
    }
    
    printf("������Ҫ�޸ĵĿγ̱�ţ�");
    scanf("%d", &courseID);             // ����γ̱��
    
    Course* c = findCourse(courseID, course, courseNum, NULL); // ���ҿγ�
    if(c == NULL)                       // �γ̲�����
    {
        printf("�γ̲����ڣ�\n");
        return 0;
    }
    
    // ��ʾ��ǰ�γ���Ϣ
    printf("��ǰ�γ���Ϣ��\n");
    printf("�γ̱�ţ�%d\n", c->ID);
    printf("�γ����ƣ�%s\n", c->title);
    printf("�ον�ʦ��%s\n", c->teacher);
    printf("���������%d\n", c->maxNum);
    printf("��ѡ������%d\n", c->selectNum);
    
    // �޸Ŀγ���Ϣ
    printf("�������µĿγ����ƣ�");
    scanf("%s", c->title);              // �޸Ŀγ�����
    printf("�������µ��ον�ʦ��");
    scanf("%s", c->teacher);            // �޸��ον�ʦ
    printf("�������µ����������");
    scanf("%d", &c->maxNum);            // �޸��������
    
    printf("�޸Ŀγ���Ϣ�ɹ���\n");
    return 1;
}

// �鿴�γ����麯��
int courseDetail(Course course[], int courseNum, User stu[], int stuNum)
{
    if(courseNum <= 0)                  // ��ǰ�����ڿγ�
    {
        printf("��ǰ�����ڿγ̣�\n");
        return 0;
    }
    
    int n;                              // Ҫ��ѯ�Ŀγ̱��
    printf("������Ҫ��ѯ�Ŀγ̱�ţ�");
    scanf("%d", &n);                    // ����γ̱��
    
    Course* c = findCourse(n, course, courseNum, NULL); // ���ҿγ�
    if(c == NULL)                       // �γ̲�����
    {
        printf("�γ̲����ڣ�\n");
        return 0;
    }
    
    // ��ʾ�γ���ϸ��Ϣ
    printf("�γ̱�ţ�%d\n", c->ID);
    printf("�γ����ƣ�%s\n", c->title);
    printf("�ον�ʦ��%s\n", c->teacher);
    printf("���������%d\n", c->maxNum);
    printf("��ѡ������%d\n", c->selectNum);
    printf("��ѡѧ����\n");
    
    // ��ʾѡ��ÿγ̵�����ѧ��
    for(int i = 0; i < stuNum; i++)
    {
        for(int j = 0; j < stu[i].selectNum; j++)
        {
            if(stu[i].selectCourse[j] == n)
            {
                printf("%d %s\n", stu[i].ID, stu[i].name);
                break;
            }
        }
    }
    return 1;
}

// ���ѧ������
int addStu(User stu[], int* stuNum, User admin[], int adminNum)
{
    User newStu;                        // ��ѧ���ṹ��

    if(*stuNum >= MAX_STU_NUM)          // ѧ�������Ѵ�����
    {
        printf("ѧ�������Ѵ����ޣ��޷������ѧ����\n");
        return 0;
    }

    printf("������ѧ�ţ�");
    scanf("%d", &newStu.ID);            // ����ѧ��

    // ���ѧ���Ƿ�������ѧ���ظ�
    for(int i = 0; i < *stuNum; i++)
    {
        if(stu[i].ID == newStu.ID)
        {
            printf("ѧ���Ѵ��ڣ����ʧ�ܣ�\n");
            return 0;
        }
    }

    // ���ѧ���Ƿ������Ա����ظ�
    for(int i = 0; i < adminNum; i++)
    {
        if(admin[i].ID == newStu.ID)
        {
            printf("ѧ�������Ա����ظ������ʧ�ܣ�\n");
            return 0;
        }
    }

    printf("������������");
    scanf("%s", newStu.name);           // ��������
    printf("���������룺");
    scanf("%s", newStu.pwd);            // ��������

    newStu.type = 2;                    // �����û�����Ϊѧ��
    newStu.selectNum = 0;               // ��ʼ����ѡ�γ�����Ϊ0

    stu[*stuNum] = newStu;              // �����ѧ��������
    (*stuNum)++;                        // ѧ����������
    printf("���ѧ���ɹ���ѧ�ţ�%d��������%s\n", newStu.ID, newStu.name);
    return 1;
}

// ����ѧ�Ų���ѧ������
int findStu(int id, User stu[], int stuNum)
{
    for(int i = 0; i < stuNum; i++)
    {
        if(stu[i].ID == id)             // �ҵ�ƥ���ѧ��
            return i;                   // ����ѧ���������е�����
    }
    return -1;                          // δ�ҵ�����-1
}

// ɾ��ѧ������
int delStu(Course course[], int courseNum, User stu[], int* stuNum)
{
    int stuID;                          // Ҫɾ����ѧ��ѧ��

    if(*stuNum <= 0)                    // ��ǰ������ѧ��
    {
        printf("��ǰ������ѧ����\n");
        return 0;
    }

    printf("������Ҫɾ����ѧ��ѧ�ţ�");
    scanf("%d", &stuID);                // ����ѧ��ѧ��

    int index = findStu(stuID, stu, *stuNum); // ����ѧ��
    if(index == -1)                     // ѧ��������
    {
        printf("ѧ�������ڣ�\n");
        return 0;
    }

    // �����пγ��м��ٸ�ѧ����ѡ������
    for(int i = 0; i < stu[index].selectNum; i++)
    {
        Course* c = findCourse(stu[index].selectCourse[i], course, courseNum, NULL);
        if(c != NULL)
        {
            c->selectNum--;             // �γ�ѡ����������
        }
    }

    // ��ѧ��������ɾ����ѧ��
    for(int i = index; i < *stuNum - 1; i++)
    {
        stu[i] = stu[i + 1];            // �ƶ�����Ԫ��
    }
    (*stuNum)--;                        // ѧ����������

    printf("ɾ��ѧ���ɹ���ѧ�ţ�%d\n", stuID);
    return 1;
}

// �޸�ѧ����Ϣ����
int editStu(User stu[], int stuNum)
{
    int stuID;                          // ѧ��ѧ��
    
    if(stuNum <= 0)                     // ��ǰ������ѧ��
    {
        printf("��ǰ������ѧ����\n");
        return 0;
    }
    
    printf("������Ҫ�޸ĵ�ѧ��ѧ�ţ�");
    scanf("%d", &stuID);                // ����ѧ��ѧ��
    
    int index = findStu(stuID, stu, stuNum); // ����ѧ��
    if(index == -1)                     // ѧ��������
    {
        printf("ѧ�������ڣ�\n");
        return 0;
    }
    
    // ��ʾ��ǰѧ����Ϣ
    printf("��ǰѧ����Ϣ��\n");
    printf("ѧ�ţ�%d\n", stu[index].ID);
    printf("������%s\n", stu[index].name);
    printf("���룺%s\n", stu[index].pwd);
    printf("��ѡ�γ�����%d\n", stu[index].selectNum);
    
    // �޸�ѧ����Ϣ
    printf("�������µ�������");
    scanf("%s", stu[index].name);       // �޸�����
    printf("�������µ����룺");
    scanf("%s", stu[index].pwd);        // �޸�����
    
    printf("�޸�ѧ����Ϣ�ɹ���\n");
    return 1;
}

// ����ԱΪѧ��ѡ�κ���
int selectByAdmin(User stu[], int stuNum, Course course[], int courseNum)
{
    int stuID, courseID;                // ѧ��ѧ�źͿγ̱��
    
    if(stuNum <= 0)                     // ��ǰ������ѧ��
    {
        printf("��ǰ������ѧ����\n");
        return 0;
    }
    
    if(courseNum <= 0)                  // ��ǰ�����ڿγ�
    {
        printf("��ǰ�����ڿγ̣�\n");
        return 0;
    }
    
    printf("������ѧ��ѧ�ţ�");
    scanf("%d", &stuID);                // ����ѧ��ѧ��
    
    int stuIndex = findStu(stuID, stu, stuNum); // ����ѧ��
    if(stuIndex == -1)                  // ѧ��������
    {
        printf("ѧ�������ڣ�\n");
        return 0;
    }
    
    printf("������γ̱�ţ�");
    scanf("%d", &courseID);             // ����γ̱��
    
    Course* c = findCourse(courseID, course, courseNum, NULL); // ���ҿγ�
    if(c == NULL)                       // �γ̲�����
    {
        printf("�γ̲����ڣ�\n");
        return 0;
    }
    
    // ���ѧ��ѡ�������Ƿ��Ѵ�����
    if(stu[stuIndex].selectNum >= MAX_SELECT_NUM)
    {
        printf("��ѧ��ѡ�������Ѵ����ޣ��޷�ѡ�Σ�\n");
        return 0;
    }
    
    // ���γ������Ƿ�����
    if(c->selectNum >= c->maxNum)
    {
        printf("�ÿγ������������޷�ѡ�Σ�\n");
        return 0;
    }
    
    // ����Ƿ���ѡ��ÿγ�
    for(int i = 0; i < stu[stuIndex].selectNum; i++)
    {
        if(stu[stuIndex].selectCourse[i] == courseID)
        {
            printf("��ѧ����ѡ��˿γ̣������ظ�ѡ��\n");
            return 0;
        }
    }
    
    // ִ��ѡ�β���
    stu[stuIndex].selectCourse[stu[stuIndex].selectNum] = courseID; // ��ӿγ̵�ѧ��ѡ���б�
    stu[stuIndex].selectNum++;          // ѧ��ѡ����������
    c->selectNum++;                     // �γ�ѡ����������
    
    printf("Ϊѧ��ѡ�γɹ���ѧ�ţ�%d���γ̣�%d-%s\n", stuID, courseID, c->title);
    return 1;
}

// ����ԱΪѧ���˿κ���
int dropByAdmin(User stu[], int stuNum, Course course[], int courseNum)
{
    int stuID, courseID;                // ѧ��ѧ�źͿγ̱��
    
    if(stuNum <= 0)                     // ��ǰ������ѧ��
    {
        printf("��ǰ������ѧ����\n");
        return 0;
    }
    
    if(courseNum <= 0)                  // ��ǰ�����ڿγ�
    {
        printf("��ǰ�����ڿγ̣�\n");
        return 0;
    }
    
    printf("������ѧ��ѧ�ţ�");
    scanf("%d", &stuID);                // ����ѧ��ѧ��
    
    int stuIndex = findStu(stuID, stu, stuNum); // ����ѧ��
    if(stuIndex == -1)                  // ѧ��������
    {
        printf("ѧ�������ڣ�\n");
        return 0;
    }
    
    // ���ѧ���Ƿ���ѡ��
    if(stu[stuIndex].selectNum <= 0)
    {
        printf("��ѧ����δѡ���κογ̣�\n");
        return 0;
    }
    
    printf("������γ̱�ţ�");
    scanf("%d", &courseID);             // ����γ̱��
    
    Course* c = findCourse(courseID, course, courseNum, NULL); // ���ҿγ�
    if(c == NULL)                       // �γ̲�����
    {
        printf("�γ̲����ڣ�\n");
        return 0;
    }
    
    // ����ѧ���Ƿ�ѡ���˸ÿγ�
    int i;
    for(i = 0; i < stu[stuIndex].selectNum; i++)
    {
        if(stu[stuIndex].selectCourse[i] == courseID)
            break;
    }
    
    if(i == stu[stuIndex].selectNum)    // δѡ��˿γ�
    {
        printf("��ѧ��δѡ��˿γ̣�\n");
        return 0;
    }
    
    // ִ���˿β���
    for(int j = i; j < stu[stuIndex].selectNum - 1; j++)
    {
        stu[stuIndex].selectCourse[j] = stu[stuIndex].selectCourse[j + 1]; // �ƶ�����Ԫ��
    }
    stu[stuIndex].selectNum--;          // ѧ��ѡ����������
    c->selectNum--;                     // �γ�ѡ����������
    
    printf("Ϊѧ���˿γɹ���ѧ�ţ�%d���γ̣�%d-%s\n", stuID, courseID, c->title);
    return 1;
}
