#pragma once

#define HT_INITIAL_BASE_SIZE 50

typedef struct
{
	char* key;
	char* value;
} ht_item;

typedef struct
{
	int base_size;
	int size;
	int count;
	ht_item** items; //ָ���ָ��
} ht_hash_table;

static ht_item* ht_new_item(const char* k, const char* v); //�����¼�ֵ��
ht_hash_table* ht_new(); //�����¹�ϣ��
static ht_hash_table* ht_new_sized(const int base_size);
static void ht_del_item(ht_item* i); //ɾ����ֵ��
void ht_del_hash_table(ht_hash_table* ht); //ɾ����ϣ��
static int ht_hash(const char* s, const int a, const int m); //��ϣ����
static int ht_get_hash(const char* s, const int num_buckets, const int attempt); //˫��ϣ��ȡ��ϣֵ

//API�ӿ�
void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* ht, const char* key);

static void ht_resize(ht_hash_table* ht, const int base_size); //���Ź�ϣ��
static void ht_resize_up(ht_hash_table* ht); //�Ŵ��ϣ��
static void ht_resize_down(ht_hash_table* ht); //��С��ϣ��