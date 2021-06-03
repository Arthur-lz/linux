/* 单向链表反转，直接在原链表上反转，不再申请额外内存（数组、链表）
 * 假设：输入[1,2,3,4,5]，输出[5,4,3,2,1]
 * 已经链表结构如下，
 * struct dlist{
 * 	int val;
 *	struct dlist *next;
 * };
 * 链表初始状态：1->2->3->4->5
 *
 * 目标链表状态：5->4->3->2->1
 * */

#include <stdio.h>

#define N	5
struct dlist{
	int val;
      	struct dlist *next;
};

void output_list(struct dlist *head);

struct dlist * reverseList(struct dlist* head){
    if (head == NULL || head->next == NULL)
        return head;
    
    struct dlist *pre = head; 
    struct dlist *cur = head->next;
    struct dlist *tmp = head->next->next;
    
    while(cur)
    {
        tmp = cur->next; 
        cur->next = pre; 
        pre = cur;       
        cur = tmp;	 
	output_list(pre);

    }
    head->next = NULL;
    
    return pre;

}


struct dlist* reverseListD(struct dlist* head){
    if (head == NULL || head->next == NULL)
        return head;
    else
    {
        struct dlist *newhead = reverseListD(head->next);
        head->next->next = head;
        head->next = NULL;
        return newhead;
    }
}


struct dlist * init_list(int n)
{
	int i = 1;
	struct dlist *cur, *first;
	if (n <= 0)
		return NULL;

	first = malloc(sizeof(struct dlist));
	first->val = i;
	first->next = first;
	cur = first;
	while(i < n) {
		i++;
		 
		struct dlist *next = malloc(sizeof(struct dlist));
		next->val = i;
		cur->next = next;
		cur = next;
	}
	cur->next = NULL;
	return first;
}

void output_list(struct dlist *head)
{
	int i = 0;
	if (head == NULL) {
		printf("list is null\n"); 
		return;
	}

	while(head && i < N) {
		i++;
		printf("%d", head->val);
		head = head->next;
	}
	printf("\n"); 
}

void freelist(struct dlist *head)
{
	struct dlist *tmp, *del;
	tmp = head;
	while(tmp) {
		del = tmp;
		tmp = tmp->next;
		free(del);
	}
}

void main()
{
	struct dlist *l;
	l = init_list(N);	
	
	output_list(l);
	l = reverseList(l);
	//list = reverseListD(list);
	printf("reverse list\n");
	output_list(l);
	freelist(l);	
}
