#include <stdio.h>          // 标准输入输出库
#include <stdlib.h>         // 标准库函数
#include <string.h>         // 字符串处理函数

// 定义系统常量
#define MAX_SELECT_NUM 10   // 每个学生选课数量上限
#define MAX_COURSE_NUM 100  // 系统中课程数量上限
#define MAX_STU_NUM 1000    // 系统中学生数量上限
#define MAX_ADMIN_NUM 10    // 系统中管理员数量上限
#define MAX_COURSE_TITLE 50 // 课程名称长度上限
#define MAX_NAME 20         // 姓名长度上限
#define MAX_PWD 20          // 密码长度上限

// 课程结构体定义
typedef struct
{
    int ID;                         // 课程编号
    char title[MAX_COURSE_TITLE];   // 课程名称
    char teacher[MAX_NAME];         // 教师姓名
    int maxNum;                     // 该课程所能容纳的学生上限
    int selectNum;                  // 已选该课程的学生数量
}Course;

// 用户结构体定义（包括学生用户和管理员用户）
typedef struct
{
    int ID;                                 // 用户ID（学号或管理员编号）
    char name[MAX_NAME];                    // 用户姓名
    char pwd[MAX_PWD];                      // 用户密码
    int type;                               // 用户类型，1为管理员，2为学生
    int selectNum;                          // 已选课程数量
    int selectCourse[MAX_SELECT_NUM];       // 已选课程编号数组
}User;

// 函数声明
int InputCourse(Course course[]);                                               // 导入课程信息
int InputStudent(User stu[]);                                                  // 导入学生信息
int InputAdmin(User ad[]);                                                      // 导入管理员信息
User* login(User admin[], int adminNum, User stu[], int stuNum);               // 用户登录
int stuOperate(User* stu, Course course[], int courseNum);                     // 学生操作界面
void showCourses(Course course[], int courseNum);                              // 显示所有课程
void showSelectedCourses(User* stu, Course course[], int courseNum);           // 显示已选课程
Course* findCourse(int courseID, Course course[], int courseNum, int* index); // 根据课程编号查找课程并返回索引
int select(User* stu, Course course[], int courseNum);                         // 选课
int drop(User* stu, Course course[], int courseNum);                           // 退课
int adminOperate(User* ad, Course course[], int* courseNum, User stu[], int* stuNum, User admin[], int adminNum); // 管理员操作界面
int addCourse(Course course[], int* courseNum);                                // 添加课程
int delCourse(Course course[], int* courseNum, User stu[], int stuNum);        // 删除课程
int editCourse(Course course[], int courseNum);                                // 修改课程信息
int courseDetail(Course course[], int courseNum, User stu[], int stuNum);      // 查看课程详情
void showStu(User stu[], int stuNum);                                          // 显示所有学生信息
void showAdmin(User admin[], int adminNum);                                    // 显示所有管理员信息
int addStu(User stu[], int* stuNum, User admin[], int adminNum);               // 添加学生
int findStu(int id, User stu[], int stuNum);                                   // 根据学号查找学生
int delStu(Course course[], int courseNum, User stu[], int* stuNum);           // 删除学生
int editStu(User stu[], int stuNum);                                           // 修改学生信息
int selectByAdmin(User stu[], int stuNum, Course course[], int courseNum);     // 管理员为学生选课
int dropByAdmin(User stu[], int stuNum, Course course[], int courseNum);       // 管理员为学生退课

// 主函数
int main(int argc, char *argv[]) {
    int stuNum = 0;         // 学生数量
    int adminNum = 0;       // 管理员数量
    int courseNum = 0;      // 课程数量
    int userType = 0;       // 用户类型：1为管理员，2为学生，0为未登录
    User* currentUser = NULL; // 当前登录用户指针

    // 定义数组存储系统数据
    Course course[MAX_COURSE_NUM];  // 课程数组
    User admin[MAX_ADMIN_NUM];      // 管理员数组
    User stu[MAX_STU_NUM];          // 学生数组

    // 初始化系统数据
    courseNum = InputCourse(course);    // 导入课程信息
    stuNum = InputStudent(stu);         // 导入学生信息
    adminNum = InputAdmin(admin);       // 导入管理员信息

    // 系统主循环
    while(1)
    {
        if(userType == 0)   // 用户未登录状态
        {
            currentUser = login(admin, adminNum, stu, stuNum);  // 用户登录
            if(currentUser != NULL)
                userType = currentUser->type;   // 设置用户类型
        }
        else if(currentUser->type == 1) // 管理员用户
            userType = adminOperate(currentUser, course, &courseNum, stu, &stuNum, admin, adminNum);
        else if(currentUser->type == 2) // 学生用户
            userType = stuOperate(currentUser, course, courseNum);
    }

    return 0;
}

// 导入课程信息函数
int InputCourse(Course course[])
{
    int i = 0;                          // 计数器
    FILE *file;                         // 文件指针
    char buffer[200];                   // 缓冲区
    
    file = fopen("course.txt", "r");    // 打开课程信息文件
    if (file == NULL) {                 // 文件打开失败
        printf("无法打开course.txt文件\n");
        return 0;
    }
    
    // 跳过文件头部标题行
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        // 读取课程信息
        while (fscanf(file, "%d %s %s %d", &course[i].ID, course[i].title, 
                     course[i].teacher, &course[i].maxNum) == 4) {
            course[i].selectNum = 0;    // 初始化已选人数为0
            i++;                        // 课程计数增加
            if (i >= MAX_COURSE_NUM) break; // 防止数组越界
        }
    }
    
    fclose(file);                       // 关闭文件
    printf("导入课程成功，共导入%d门课程\n", i);
    return i;                           // 返回课程数量
}

// 导入学生信息函数
int InputStudent(User stu[])
{
    int i = 0;                          // 计数器
    FILE *file;                         // 文件指针
    char buffer[200];                   // 缓冲区
    
    file = fopen("student.txt", "r");   // 打开学生信息文件
    if (file == NULL) {                 // 文件打开失败
        printf("无法打开student.txt文件\n");
        return 0;
    }
    
    // 跳过文件头部标题行
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        // 读取学生信息
        while (fscanf(file, "%d %s %s", &stu[i].ID, stu[i].name, stu[i].pwd) == 3) {
            stu[i].selectNum = 0;       // 初始化已选课程数量为0
            stu[i].type = 2;            // 设置用户类型为学生
            i++;                        // 学生计数增加
            if (i >= MAX_STU_NUM) break; // 防止数组越界
        }
    }
    
    fclose(file);                       // 关闭文件
    printf("导入学生信息成功，共导入%d个学生\n", i);
    return i;                           // 返回学生数量
}

// 导入管理员信息函数
int InputAdmin(User ad[])
{
    int i = 0;                          // 计数器
    FILE *file;                         // 文件指针
    char buffer[200];                   // 缓冲区
    
    file = fopen("admin.txt", "r");     // 打开管理员信息文件
    if (file == NULL) {                 // 文件打开失败
        printf("无法打开admin.txt文件\n");
        return 0;
    }
    
    // 跳过文件头部标题行
    if (fgets(buffer, sizeof(buffer), file) != NULL) {
        // 读取管理员信息
        while (fscanf(file, "%d %s %s", &ad[i].ID, ad[i].name, ad[i].pwd) == 3) {
            ad[i].type = 1;             // 设置用户类型为管理员
            i++;                        // 管理员计数增加
            if (i >= MAX_ADMIN_NUM) break; // 防止数组越界
        }
    }
    
    fclose(file);                       // 关闭文件
    printf("导入管理员信息成功，共导入%d个管理员\n", i);
    return i;                           // 返回管理员数量
}

// 用户登录函数
User* login(User admin[], int adminNum, User stu[], int stuNum)
{
    int id;                             // 用户ID
    char pwd[21];                       // 密码
    
    while(1)
    {
        printf("请输入用户名：");
        scanf("%d", &id);               // 输入用户ID
        printf("请输入密码：");
        scanf("%s", pwd);               // 输入密码
        
        // 检查是否为管理员
        for (int i = 0; i < adminNum; i++)
        {
            if(id == admin[i].ID && strcmp(pwd, admin[i].pwd) == 0)
            {
                printf("管理员登录成功！欢迎 %s\n", admin[i].name);
                return &admin[i];       // 返回管理员指针
            }
        }
        
        // 检查是否为学生
        for (int i = 0; i < stuNum; i++)
        {
            if(id == stu[i].ID && strcmp(pwd, stu[i].pwd) == 0)
            {
                printf("学生登录成功！欢迎 %s\n", stu[i].name);
                return &stu[i];         // 返回学生指针
            }
        }
        
        printf("用户名或密码错误，请重新输入！\n");
    }
    
    return NULL;                        // 登录失败返回NULL
}

// 学生操作界面函数
int stuOperate(User* stu, Course course[], int courseNum)
{
    int option;                         // 选项
    
    while(1)
    {
        // 显示学生菜单
        printf("==== 学号：%d 姓名：%s ====\n", stu->ID, stu->name);
        printf(" 1.查看所有课程\n");
        printf(" 2.查看已选课程\n");
        printf(" 3.选课\n");
        printf(" 4.退课\n");
        printf(" 5.退出登录\n");
        printf("======================================\n");
        printf("请选择：");
        scanf("%d", &option);           // 输入选项
        
        // 根据选项执行相应操作
        switch(option)
        {
            case 1:
                showCourses(course, courseNum);                 // 查看所有课程
                break;
            case 2:
                showSelectedCourses(stu, course, courseNum);    // 查看已选课程
                break;
            case 3:
                select(stu, course, courseNum);                 // 选课
                break;
            case 4:
                drop(stu, course, courseNum);                   // 退课
                break;
            case 5:
                printf("退出登录成功！\n");
                return 0;                                       // 退出登录
            default:
                printf("输入无效，请重新选择！\n");
                break;
        }
    }
}

// 显示所有课程函数
void showCourses(Course course[], int courseNum)
{
    printf("====课程编号 课程名称                任课教师   最大人数 已选人数====\n");
    for(int i = 0; i < courseNum; i++)
    {
        // 格式化输出课程信息，保证列对齐
        printf("%10d %-20s %-10s %8d %8d\n", 
               course[i].ID, course[i].title, course[i].teacher, 
               course[i].maxNum, course[i].selectNum);
    }
    printf("========================================================================\n");
}

// 显示学生已选课程函数
void showSelectedCourses(User* stu, Course course[], int courseNum)
{
    if(stu->selectNum == 0)             // 未选择任何课程
    {
        printf("您还没有选择任何课程！\n");
        return;
    }
    
    printf("====课程编号 课程名称                任课教师   最大人数 已选人数====\n");
    for(int i = 0; i < stu->selectNum; i++)
    {
        Course * c = findCourse(stu->selectCourse[i], course, courseNum, NULL);
        if(c != NULL)
        {
            // 格式化输出已选课程信息
            printf("%10d %-20s %-10s %8d %8d\n", 
                   c->ID, c->title, c->teacher, c->maxNum, c->selectNum);
        }
    }
    printf("========================================================================\n");
}

// 显示所有学生信息函数
void showStu(User stu[], int stuNum)
{
    printf("====    学号      姓名      密码  已选课程数 已选课程 ====\n");
    for(int i = 0; i < stuNum; i++)
    {
        printf("%10d %10s %10s %10d ", 
               stu[i].ID, stu[i].name, stu[i].pwd, stu[i].selectNum);
        
        // 输出已选课程编号
        for(int j = 0; j < stu[i].selectNum; j++)
        {
            printf("%d ", stu[i].selectCourse[j]);
        }
        printf("\n");
    }
    printf("============================================================\n");
}

// 显示所有管理员信息函数
void showAdmin(User admin[], int adminNum)
{
    printf("====    账号      姓名      密码 ====\n");
    for(int i = 0; i < adminNum; i++)
    {
        printf("%10d %10s %10s\n", 
               admin[i].ID, admin[i].name, admin[i].pwd);
    }
    printf("=====================================\n");
}

// 已被 findCourse(int courseID, Course course[], int courseNum, int* index) 替代，无需重复定义

// 根据课程编号查找课程并返回索引函数
Course * findCourse(int courseID, Course course[], int courseNum, int* index)
{
    for(int i = 0; i < courseNum; i++)
    {
        if(course[i].ID == courseID)    // 找到匹配的课程
        {
            if(index) *index = i;       // 保存索引
            return &course[i];          // 返回课程指针
        }
    }
    return NULL;                        // 未找到返回NULL
}

// 学生选课函数
int select(User* stu, Course course[], int courseNum)
{
    int num;                            // 课程编号
    
    if(stu->selectNum >= MAX_SELECT_NUM) // 选课数量已达上限
    {
        printf("选课数量已达上限，无法选课！\n");
        return 0;
    }
    
    while(1)
    {
        printf("请输入课程编号，若要取消请输入0：");
        scanf("%d", &num);              // 输入课程编号
        
        if(num == 0)                    // 取消操作
            return 0;
        
        Course* c = findCourse(num, course, courseNum, NULL); // 查找课程
        if(c == NULL)                   // 课程不存在
        {
            printf("课程不存在，请重新输入！\n");
        }
        else
        {
            if(c->selectNum >= c->maxNum) // 课程人数已满
            {
                printf("该课程人数已满，请选择其他课程！\n");
            }
            else
            {
                // 检查是否已选择该课程
                int i;
                for(i = 0; i < stu->selectNum; i++)
                {
                    if(stu->selectCourse[i] == c->ID)
                    {
                        printf("已选该课程，请勿重复选择！\n");
                        return 0;
                    }
                }
                
                // 执行选课操作
                stu->selectCourse[stu->selectNum] = c->ID;  // 添加课程到学生选课列表
                stu->selectNum++;                           // 学生选课数量增加
                c->selectNum++;                             // 课程选课人数增加
                printf("选课成功！已选课程 %d-%s\n", c->ID, c->title);
                return 1;
            }
        }
    }
}

// 学生退课函数
int drop(User* stu, Course course[], int courseNum)
{
    int num;                            // 课程编号
    
    if(stu->selectNum <= 0)             // 未选择任何课程
    {
        printf("尚未选择任何课程\n");
        return 0;
    }
    
    while(1)
    {
        printf("请输入要退的课程编号，若要取消请输入0：");
        scanf("%d", &num);              // 输入课程编号
        
        if(num == 0)                    // 取消操作
            return 0;
        
        Course* c = findCourse(num, course, courseNum, NULL); // 查找课程
        if(c == NULL)                   // 课程不存在
        {
            printf("课程不存在，请重新输入！\n");
        }
        else
        {
            // 查找学生是否选择了该课程
            int i;
            for(i = 0; i < stu->selectNum; i++)
            {
                if(stu->selectCourse[i] == c->ID)
                    break;
            }
            
            if(i == stu->selectNum)     // 未选择此课程
                printf("未选择此课程，请重新输入！\n");
            else
            {
                // 执行退课操作
                for(int j = i; j < stu->selectNum - 1; j++)
                {
                    stu->selectCourse[j] = stu->selectCourse[j + 1]; // 移动数组元素
                }
                stu->selectNum--;       // 学生选课数量减少
                c->selectNum--;         // 课程选课人数减少
                printf("退课成功！已退课程 %d-%s\n", c->ID, c->title);
                return 1;
            }
        }
    }
}

// 管理员操作界面函数
int adminOperate(User* ad, Course course[], int* courseNum, User stu[], int* stuNum, User admin[], int adminNum)
{
    int option;                         // 选项

    while(1)
    {
        // 显示管理员菜单
        printf("==== 账号：%d 姓名：%s ====\n", ad->ID, ad->name);
        printf(" 1.查看所有课程\n");
        printf(" 2.查看课程详情\n");
        printf(" 3.添加课程\n");
        printf(" 4.删除课程\n");
        printf(" 5.修改课程信息\n");
        printf(" 6.查看所有学生信息\n");
        printf(" 7.添加学生\n");
        printf(" 8.删除学生\n");
        printf(" 9.修改学生信息\n");
        printf(" 10.为学生选课\n");
        printf(" 11.为学生退课\n");
        printf(" 12.查看所有管理员信息\n");
        printf(" 13.退出登录\n");
        printf("======================================\n");
        printf("请选择：");
        scanf("%d", &option);           // 输入选项

        // 根据选项执行相应操作
        switch(option)
        {
            case 1:
                showCourses(course, *courseNum);                         // 查看所有课程
                break;
            case 2:
                courseDetail(course, *courseNum, stu, *stuNum);           // 查看课程详情
                break;
            case 3:
                addCourse(course, courseNum);                           // 添加课程
                break;
            case 4:
                delCourse(course, courseNum, stu, *stuNum);              // 删除课程
                break;
            case 5:
                editCourse(course, *courseNum);                          // 修改课程信息
                break;
            case 6:
                showStu(stu, *stuNum);                                   // 查看所有学生信息
                break;
            case 7:
                addStu(stu, stuNum, admin, adminNum);                   // 添加学生
                break;
            case 8:
                delStu(course, *courseNum, stu, stuNum);                 // 删除学生
                break;
            case 9:
                editStu(stu, *stuNum);                                   // 修改学生信息
                break;
            case 10:
                selectByAdmin(stu, *stuNum, course, *courseNum);          // 为学生选课
                break;
            case 11:
                dropByAdmin(stu, *stuNum, course, *courseNum);            // 为学生退课
                break;
            case 12:
                showAdmin(admin, adminNum);                             // 查看所有管理员信息
                break;
            case 13:
                printf("退出登录成功！\n");
                return 0;                                               // 退出登录
            default:
                printf("输入无效，请重新选择！\n");
                break;
        }
    }
    return 0;
}

// 添加课程函数
int addCourse(Course course[], int* courseNum)
{
    Course newCourse;                   // 新课程结构体

    if(*courseNum >= MAX_COURSE_NUM)    // 课程数量已达上限
    {
        printf("课程数量已达上限，无法添加新课程！\n");
        return 0;
    }

    printf("请输入课程编号：");
    scanf("%d", &newCourse.ID);         // 输入课程编号

    // 检查课程编号是否重复
    for(int i = 0; i < *courseNum; i++)
    {
        if(course[i].ID == newCourse.ID)
        {
            printf("课程编号已存在，添加失败！\n");
            return 0;
        }
    }

    printf("请输入课程名称：");
    scanf("%s", newCourse.title);       // 输入课程名称
    printf("请输入任课教师：");
    scanf("%s", newCourse.teacher);     // 输入任课教师
    printf("请输入最大人数：");
    scanf("%d", &newCourse.maxNum);     // 输入最大人数
    newCourse.selectNum = 0;            // 初始化已选人数为0

    course[*courseNum] = newCourse;     // 添加新课程到数组
    (*courseNum)++;                     // 课程数量增加
    printf("添加课程成功！课程编号：%d，课程名称：%s\n", newCourse.ID, newCourse.title);
    return 1;
}

// 删除课程函数
int delCourse(Course course[], int* courseNum, User stu[], int stuNum)
{
    int courseID;                       // 要删除的课程编号
    int index;                          // 课程在数组中的索引

    if(*courseNum <= 0)                 // 当前不存在课程
    {
        printf("当前不存在课程！\n");
        return 0;
    }

    printf("请输入要删除的课程编号：");
    scanf("%d", &courseID);             // 输入课程编号

    Course* c = findCourse(courseID, course, *courseNum, &index); // 查找课程
    if(c == NULL)                       // 课程不存在
    {
        printf("课程不存在！\n");
        return 0;
    }

    // 从所有学生的选课列表中删除该课程
    for(int i = 0; i < stuNum; i++)
    {
        for(int j = 0; j < stu[i].selectNum; j++)
        {
            if(stu[i].selectCourse[j] == courseID)
            {
                // 移除该课程
                for(int k = j; k < stu[i].selectNum - 1; k++)
                {
                    stu[i].selectCourse[k] = stu[i].selectCourse[k + 1];
                }
                stu[i].selectNum--;     // 学生选课数量减少
                j--;                    // 调整索引
            }
        }
    }

    // 从课程数组中删除该课程
    for(int i = index; i < *courseNum - 1; i++)
    {
        course[i] = course[i + 1];      // 移动数组元素
    }
    (*courseNum)--;                     // 课程数量减少

    printf("删除课程成功！课程编号：%d\n", courseID);
    return 1;
}

// 修改课程信息函数
int editCourse(Course course[], int courseNum)
{
    int courseID;                       // 课程编号
    
    if(courseNum <= 0)                  // 当前不存在课程
    {
        printf("当前不存在课程！\n");
        return 0;
    }
    
    printf("请输入要修改的课程编号：");
    scanf("%d", &courseID);             // 输入课程编号
    
    Course* c = findCourse(courseID, course, courseNum, NULL); // 查找课程
    if(c == NULL)                       // 课程不存在
    {
        printf("课程不存在！\n");
        return 0;
    }
    
    // 显示当前课程信息
    printf("当前课程信息：\n");
    printf("课程编号：%d\n", c->ID);
    printf("课程名称：%s\n", c->title);
    printf("任课教师：%s\n", c->teacher);
    printf("最大人数：%d\n", c->maxNum);
    printf("已选人数：%d\n", c->selectNum);
    
    // 修改课程信息
    printf("请输入新的课程名称：");
    scanf("%s", c->title);              // 修改课程名称
    printf("请输入新的任课教师：");
    scanf("%s", c->teacher);            // 修改任课教师
    printf("请输入新的最大人数：");
    scanf("%d", &c->maxNum);            // 修改最大人数
    
    printf("修改课程信息成功！\n");
    return 1;
}

// 查看课程详情函数
int courseDetail(Course course[], int courseNum, User stu[], int stuNum)
{
    if(courseNum <= 0)                  // 当前不存在课程
    {
        printf("当前不存在课程！\n");
        return 0;
    }
    
    int n;                              // 要查询的课程编号
    printf("请输入要查询的课程编号：");
    scanf("%d", &n);                    // 输入课程编号
    
    Course* c = findCourse(n, course, courseNum, NULL); // 查找课程
    if(c == NULL)                       // 课程不存在
    {
        printf("课程不存在！\n");
        return 0;
    }
    
    // 显示课程详细信息
    printf("课程编号：%d\n", c->ID);
    printf("课程名称：%s\n", c->title);
    printf("任课教师：%s\n", c->teacher);
    printf("最大人数：%d\n", c->maxNum);
    printf("已选人数：%d\n", c->selectNum);
    printf("已选学生：\n");
    
    // 显示选择该课程的所有学生
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

// 添加学生函数
int addStu(User stu[], int* stuNum, User admin[], int adminNum)
{
    User newStu;                        // 新学生结构体

    if(*stuNum >= MAX_STU_NUM)          // 学生数量已达上限
    {
        printf("学生数量已达上限，无法添加新学生！\n");
        return 0;
    }

    printf("请输入学号：");
    scanf("%d", &newStu.ID);            // 输入学号

    // 检查学号是否与已有学生重复
    for(int i = 0; i < *stuNum; i++)
    {
        if(stu[i].ID == newStu.ID)
        {
            printf("学号已存在，添加失败！\n");
            return 0;
        }
    }

    // 检查学号是否与管理员编号重复
    for(int i = 0; i < adminNum; i++)
    {
        if(admin[i].ID == newStu.ID)
        {
            printf("学号与管理员编号重复，添加失败！\n");
            return 0;
        }
    }

    printf("请输入姓名：");
    scanf("%s", newStu.name);           // 输入姓名
    printf("请输入密码：");
    scanf("%s", newStu.pwd);            // 输入密码

    newStu.type = 2;                    // 设置用户类型为学生
    newStu.selectNum = 0;               // 初始化已选课程数量为0

    stu[*stuNum] = newStu;              // 添加新学生到数组
    (*stuNum)++;                        // 学生数量增加
    printf("添加学生成功！学号：%d，姓名：%s\n", newStu.ID, newStu.name);
    return 1;
}

// 根据学号查找学生函数
int findStu(int id, User stu[], int stuNum)
{
    for(int i = 0; i < stuNum; i++)
    {
        if(stu[i].ID == id)             // 找到匹配的学生
            return i;                   // 返回学生在数组中的索引
    }
    return -1;                          // 未找到返回-1
}

// 删除学生函数
int delStu(Course course[], int courseNum, User stu[], int* stuNum)
{
    int stuID;                          // 要删除的学生学号

    if(*stuNum <= 0)                    // 当前不存在学生
    {
        printf("当前不存在学生！\n");
        return 0;
    }

    printf("请输入要删除的学生学号：");
    scanf("%d", &stuID);                // 输入学生学号

    int index = findStu(stuID, stu, *stuNum); // 查找学生
    if(index == -1)                     // 学生不存在
    {
        printf("学生不存在！\n");
        return 0;
    }

    // 从所有课程中减少该学生的选课人数
    for(int i = 0; i < stu[index].selectNum; i++)
    {
        Course* c = findCourse(stu[index].selectCourse[i], course, courseNum, NULL);
        if(c != NULL)
        {
            c->selectNum--;             // 课程选课人数减少
        }
    }

    // 从学生数组中删除该学生
    for(int i = index; i < *stuNum - 1; i++)
    {
        stu[i] = stu[i + 1];            // 移动数组元素
    }
    (*stuNum)--;                        // 学生数量减少

    printf("删除学生成功！学号：%d\n", stuID);
    return 1;
}

// 修改学生信息函数
int editStu(User stu[], int stuNum)
{
    int stuID;                          // 学生学号
    
    if(stuNum <= 0)                     // 当前不存在学生
    {
        printf("当前不存在学生！\n");
        return 0;
    }
    
    printf("请输入要修改的学生学号：");
    scanf("%d", &stuID);                // 输入学生学号
    
    int index = findStu(stuID, stu, stuNum); // 查找学生
    if(index == -1)                     // 学生不存在
    {
        printf("学生不存在！\n");
        return 0;
    }
    
    // 显示当前学生信息
    printf("当前学生信息：\n");
    printf("学号：%d\n", stu[index].ID);
    printf("姓名：%s\n", stu[index].name);
    printf("密码：%s\n", stu[index].pwd);
    printf("已选课程数：%d\n", stu[index].selectNum);
    
    // 修改学生信息
    printf("请输入新的姓名：");
    scanf("%s", stu[index].name);       // 修改姓名
    printf("请输入新的密码：");
    scanf("%s", stu[index].pwd);        // 修改密码
    
    printf("修改学生信息成功！\n");
    return 1;
}

// 管理员为学生选课函数
int selectByAdmin(User stu[], int stuNum, Course course[], int courseNum)
{
    int stuID, courseID;                // 学生学号和课程编号
    
    if(stuNum <= 0)                     // 当前不存在学生
    {
        printf("当前不存在学生！\n");
        return 0;
    }
    
    if(courseNum <= 0)                  // 当前不存在课程
    {
        printf("当前不存在课程！\n");
        return 0;
    }
    
    printf("请输入学生学号：");
    scanf("%d", &stuID);                // 输入学生学号
    
    int stuIndex = findStu(stuID, stu, stuNum); // 查找学生
    if(stuIndex == -1)                  // 学生不存在
    {
        printf("学生不存在！\n");
        return 0;
    }
    
    printf("请输入课程编号：");
    scanf("%d", &courseID);             // 输入课程编号
    
    Course* c = findCourse(courseID, course, courseNum, NULL); // 查找课程
    if(c == NULL)                       // 课程不存在
    {
        printf("课程不存在！\n");
        return 0;
    }
    
    // 检查学生选课数量是否已达上限
    if(stu[stuIndex].selectNum >= MAX_SELECT_NUM)
    {
        printf("该学生选课数量已达上限，无法选课！\n");
        return 0;
    }
    
    // 检查课程人数是否已满
    if(c->selectNum >= c->maxNum)
    {
        printf("该课程人数已满，无法选课！\n");
        return 0;
    }
    
    // 检查是否已选择该课程
    for(int i = 0; i < stu[stuIndex].selectNum; i++)
    {
        if(stu[stuIndex].selectCourse[i] == courseID)
        {
            printf("该学生已选择此课程，请勿重复选择！\n");
            return 0;
        }
    }
    
    // 执行选课操作
    stu[stuIndex].selectCourse[stu[stuIndex].selectNum] = courseID; // 添加课程到学生选课列表
    stu[stuIndex].selectNum++;          // 学生选课数量增加
    c->selectNum++;                     // 课程选课人数增加
    
    printf("为学生选课成功！学号：%d，课程：%d-%s\n", stuID, courseID, c->title);
    return 1;
}

// 管理员为学生退课函数
int dropByAdmin(User stu[], int stuNum, Course course[], int courseNum)
{
    int stuID, courseID;                // 学生学号和课程编号
    
    if(stuNum <= 0)                     // 当前不存在学生
    {
        printf("当前不存在学生！\n");
        return 0;
    }
    
    if(courseNum <= 0)                  // 当前不存在课程
    {
        printf("当前不存在课程！\n");
        return 0;
    }
    
    printf("请输入学生学号：");
    scanf("%d", &stuID);                // 输入学生学号
    
    int stuIndex = findStu(stuID, stu, stuNum); // 查找学生
    if(stuIndex == -1)                  // 学生不存在
    {
        printf("学生不存在！\n");
        return 0;
    }
    
    // 检查学生是否有选课
    if(stu[stuIndex].selectNum <= 0)
    {
        printf("该学生尚未选择任何课程！\n");
        return 0;
    }
    
    printf("请输入课程编号：");
    scanf("%d", &courseID);             // 输入课程编号
    
    Course* c = findCourse(courseID, course, courseNum, NULL); // 查找课程
    if(c == NULL)                       // 课程不存在
    {
        printf("课程不存在！\n");
        return 0;
    }
    
    // 查找学生是否选择了该课程
    int i;
    for(i = 0; i < stu[stuIndex].selectNum; i++)
    {
        if(stu[stuIndex].selectCourse[i] == courseID)
            break;
    }
    
    if(i == stu[stuIndex].selectNum)    // 未选择此课程
    {
        printf("该学生未选择此课程！\n");
        return 0;
    }
    
    // 执行退课操作
    for(int j = i; j < stu[stuIndex].selectNum - 1; j++)
    {
        stu[stuIndex].selectCourse[j] = stu[stuIndex].selectCourse[j + 1]; // 移动数组元素
    }
    stu[stuIndex].selectNum--;          // 学生选课数量减少
    c->selectNum--;                     // 课程选课人数减少
    
    printf("为学生退课成功！学号：%d，课程：%d-%s\n", stuID, courseID, c->title);
    return 1;
}
