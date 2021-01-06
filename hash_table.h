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
	ht_item** items; //指针的指针
} ht_hash_table;

static ht_item* ht_new_item(const char* k, const char* v); //创建新键值对
ht_hash_table* ht_new(); //创建新哈希表
static ht_hash_table* ht_new_sized(const int base_size);
static void ht_del_item(ht_item* i); //删除键值对
void ht_del_hash_table(ht_hash_table* ht); //删除哈希表
static int ht_hash(const char* s, const int a, const int m); //哈希计算
static int ht_get_hash(const char* s, const int num_buckets, const int attempt); //双哈希获取哈希值

//API接口
void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* ht, const char* key);

static void ht_resize(ht_hash_table* ht, const int base_size); //缩放哈希表
static void ht_resize_up(ht_hash_table* ht); //放大哈希表
static void ht_resize_down(ht_hash_table* ht); //缩小哈希表