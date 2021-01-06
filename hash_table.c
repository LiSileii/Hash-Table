#include <stdlib.h>
#include <string.h>

#include "hash_table.h"
#include "prime.h"

/*创建一个新键值对*/
static ht_item* ht_new_item(const char* k, const char* v) //静态函数只能在声明它的文件中可见，其他文件不能引用该函数
{
	ht_item* i = malloc(sizeof(ht_item));
	i->key = strdup(k); //strdup()函数是c语言中常用的一种字符串拷贝库函数，一般和free()函数成对出现
						//strdup可以直接把要复制的内容复制给没有初始化的指针，
						//因为它会自动分配空间给目的指针，strcpy的目的指针一定是已经分配内存的指针
	i->value = strdup(v);
	return i;
}

ht_hash_table* ht_new()
{
	/*
	ht_hash_table* ht = malloc(sizeof(ht_hash_table));
	ht->size = 53;
	ht->count = 0;
	ht->items = calloc((size_t)ht->size, sizeof(ht_item*)); //calloc()在内存的动态存储区中分配num个长度为size的连续空间
	return ht;
	*/
	return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

static ht_hash_table* ht_new_sized(const int base_size) //可缩放
{
	ht_hash_table* ht = malloc(sizeof(ht_hash_table));
	ht->base_size = base_size;
	ht->size = next_prime(ht->base_size);
	ht->count = 0;
	ht->items = calloc((size_t)ht->size, sizeof(ht_item*));
	return ht;
}

static void ht_del_item(ht_item* i)
{
	free(i->key); //初始化时用了strdup()函数，需要用free()函数释放内存
	free(i->value);
	free(i);
}

void ht_del_hash_table(ht_hash_table* ht)
{
	for (int i = 0; i < ht->size; ++i)
	{
		ht_item* item = ht->items[i];
		if (item != NULL)
			ht_del_item(item);
	}
	free(ht->items);
	free(ht);
}

static int ht_hash(const char* s, const int a, const int m)
{
	long hash = 0;
	const int len_s = strlen(s);
	for (int i = 0; i < len_s; ++i)
		hash += (long)pow(a, len_s - (i + 1)) * s[i];
	hash = hash % m;
	return (int)hash;
}

static int ht_get_hash(const char* s, const int num_buckets, const int attempt)
{
	const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
	const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
	return (hash_a + attempt * (hash_b + 1)) % num_buckets;
}

static ht_item HT_DELETE_ITEM = { NULL, NULL }; //标记为被删除元素的item
void ht_delete(ht_hash_table* ht, const char* key)
{
	//检查是否需要缩放哈希表
	const int load = ht->count * 100 / ht->size; //乘100是为了避免做浮点数计算
	if (load < 10)
		ht_resize_down(ht);

	int index = ht_get_hash(key, ht->size, 0);
	ht_item* item = ht->items[index];
	int i = 1;
	while (item != NULL)
	{
		if (item != &HT_DELETE_ITEM) //若item没有被标记为删除
		{
			if (strcmp(item->key, key) == 0)
			{
				ht_del_item(item);
				ht->count--;
				ht->items[index] = &HT_DELETE_ITEM; //标记为删除
				return;
			}
		}
		index = ht_get_hash(key, ht->size, i);
		item = ht->items[index];
		i++;
	}
}

void ht_insert(ht_hash_table* ht, const char* key, const char* value)
{
	//检查是否需要缩放哈希表
	const int load = ht->count * 100 / ht->size; //乘100是为了避免做浮点数计算
	if (load > 70)
		ht_resize_up(ht);

	ht_item* item = ht_new_item(key, value); //根据键和值创建新键值对
	int index = ht_get_hash(item->key, ht->size, 0); //哈希计算获取索引值
	ht_item* cur_item = ht->items[index]; //索引值对应键值对
	int i = 1;
	while (cur_item != NULL && cur_item != &HT_DELETE_ITEM && strcmp(cur_item->key, key) != 0) 
	{
		index = ht_get_hash(item->key, ht->size, i);
		cur_item = ht->items[index];
		i++;
	}
	if (strcmp(cur_item->key, key) == 0)
		ht_del_item(cur_item);
	ht->items[index] = item;
	ht->count++;
}

char* ht_search(ht_hash_table* ht, const char* key)
{
	int index = ht_get_hash(key, ht->size, 0);
	ht_item* item = ht->items[index];
	int i = 1;
	while (item != NULL)
	{
		if (item != &HT_DELETE_ITEM)
		{
			if (strcmp(item->key, key) == 0)
				return item->value;
		}
		index = ht_get_hash(key, ht->size, i);
		item = ht->items[index];
		i++;
	}
	return NULL;
}

static void ht_resize(ht_hash_table* ht, const int base_size)
{
	if (base_size < HT_INITIAL_BASE_SIZE) //哈希表大小最小为HT_INITIAL_BASE_SIZE
		return;
	ht_hash_table* new_ht = ht_new_sized(base_size);
	for (int i = 0; i < ht->size; ++i)
	{
		ht_item* item = ht->items[i];
		if (item != NULL && item != &HT_DELETE_ITEM)
			ht_insert(new_ht, item->key, item->value);
	}

	//把new_ht的内容和ht的内容交换，再删除new_ht内容
	ht->base_size = new_ht->base_size;
	ht->count = new_ht->count;

	const int tmp_size = ht->size;
	ht->size = new_ht->size;
	new_ht->size = tmp_size;

	ht_item** tmp_items = ht->items;
	ht->items = new_ht->items;
	new_ht->items = tmp_items;

	ht_del_hash_table(new_ht);

	//或将指针ht指向new_ht，再删除原ht指向的内容？
	/*
	ht_hash_table* tmp_ht = ht;
	ht = new_ht;
	ht_del_hash_table(tmp_ht);
	*/
}

static void ht_resize_up(ht_hash_table* ht)
{
	const int new_size = ht->base_size * 2;
	ht_resize(ht, new_size);
}

static void ht_resize_down(ht_hash_table* ht)
{
	const int new_size = ht->base_size / 2;
	ht_resize(ht, new_size);
}