#include <stdio.h>

extern int mm_init (void);
extern void *mm_malloc (size_t size);
extern void mm_free (void *ptr);
extern void *mm_realloc(void *ptr, size_t size);


/* 
 * Students work in teams of one or two.  Teams enter their team name, 
 * personal names and login IDs in a struct of this
 * type in their bits.c file.
 * 中文：学生以一人或两人组队，在其 bits.c 文件中通过本类型的结构体填写团队名称、姓名和
 * 登录 ID。（bits.c 是原注释中的旧名称；Malloc Lab 的实际团队信息位于 mm.c。）
 */
typedef struct {
    char *teamname; /* ID1+ID2 or ID1  中文：两人用 ID1+ID2，一人用 ID1。 */
    char *name1;    /* full name of first member  中文：第一位成员的全名。 */
    char *id1;      /* login ID of first member  中文：第一位成员的登录 ID。 */
    char *name2;    /* full name of second member (if any)  中文：第二位成员的全名；若有第二位成员则填写。 */
    char *id2;      /* login ID of second member  中文：第二位成员的登录 ID。 */
} team_t;

extern team_t team;

