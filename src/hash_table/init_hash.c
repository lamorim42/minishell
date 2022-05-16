/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_hash.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmonteir <dmonteir@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/14 19:49:43 by dmonteir          #+#    #+#             */
/*   Updated: 2022/05/16 20:15:35 by dmonteir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

unsigned long hash_function(char *str);
t_hash_table *create_table(int size);
void free_table(t_hash_table *table);
int next_local(int value);
void hash_insert(t_hash_table *table, char *key, char *value);
char *hash_search(t_hash_table *table, char *key);
void print_table(t_hash_table *table);
void print_search(t_hash_table *table, char *key);
void handle_collision(t_hash_table *table, t_hash_item *item);

/* void	init_hash()
{
	char* tabela[size_table];
	int i;
	int option;
	char* value = NULL;
	char* return_value;

	do {
		printf("Digite um numero:\n\t0 - Sair \n\t1 - Inserir \n\t2 - Buscar \n\t3 - Imprimir Tabela\n");
		scanf("%d", &option);
		switch (option)
		{
			case 1:
				printf("Qual valor inserir?\n");
				scanf("%s", &(*value));
				insert_hash(tabela, value);
				break;
			case 2:
				printf("\tQual valor deseja buscar?\n");
				scanf("%s", &(*value));
				return_value = search_table(tabela, value);
				if(return_value != NULL)
					printf("\tValor encontrado: %s\n", return_value);
				else
					printf("\tValor nao encontrado\n");
				break;
			case 3:
				print_table(tabela);
				break;
			default:
				printf("Opção invalida\n");
		}
	} while (option != 0);

	for(i = 0; i < size_table; i++)
		tabela[i] = NULL;
} */

void init_hash()
{
	t_hash_table *table = create_table(size_table);
	hash_insert(table, "usr/bin/ls", "ls");
	hash_insert(table, "24", "oi222");
	print_search(table, "usr/bin/ls");
	print_search(table, "24");
	print_search(table, "3");
	print_table(table);
	free_table(table);
}

unsigned long hash_function(char *str)
{
	unsigned long i = 0;
	for (int j = 0; str[j]; j++)
		i += str[j];
	return i % size_table;
}

t_hash_item *create_item(char *key, char *value)
{
	t_hash_item *item;
	item = (t_hash_item *)malloc(sizeof(t_hash_item));
	item->key = (char *)malloc(ft_strlen(key) + 1);
	item->value = (char *)malloc(ft_strlen(value) + 1);

	item->key = ft_strdup(key);
	item->value = ft_strdup(value);

	return item;
}

t_hash_table *create_table(int size)
{
	int i;

	i = 0;
	t_hash_table *table = (t_hash_table *)malloc(sizeof(t_hash_table));
	table->size = size;
	table->count = 0;
	table->item = (t_hash_item **)ft_calloc(table->size, sizeof(t_hash_item *));
	while (i < table->size)
	{
		table->item[i] = NULL;
		i++;
	}

	return table;
}

void free_item(t_hash_item *item)
{
	free(item->key);
	free(item->value);
	free(item);
}

void free_table(t_hash_table *table)
{
	int i;
	i = 0;
	while (i < table->size)
	{
		if (table->item[i] != NULL)
			free_item(table->item[i]);
		i++;
	}
	free(table->item);
	free(table);
}

void hash_insert(t_hash_table *table, char *key, char *value)
{
	int index = hash_function(key);
	t_hash_item *item = create_item(key, value);
	t_hash_item *current = table->item[index];
	if (current == NULL)
	{
		if (table->count == table->size)
		{
			printf("Insert Error: Hash Table is full\n");
			free_item(item);
			return;
		}
		table->item[index] = item;
		table->count++;
	}
	else
	{
		if (!ft_strcmp(current->key, key))
		{
			table->item[index]->value = ft_strdup(value);
			return;
		}
		else
		{
			handle_collision(table, item, index);
			return;
		}
	}
}

void handle_collision(t_hash_table *table, t_hash_item *item)
{
	while (item[index] != NULL)
	{
		index = next_local(index);
	}

	printf("Collision detected\n");
	printf("%s\n", item->key);
	print_table(table);
	return;
}

char *hash_search(t_hash_table *table, char *key)
{
	int index = hash_function(key);
	t_hash_item *item = table->item[index];
	if (item != NULL)
	{
		if (ft_strcmp(item->key, key))
			return item->value;
	}

	return NULL;
}

void print_search(t_hash_table *table, char *key)
{
	char *val;
	if ((val = hash_search(table, key)) == NULL)
	{
		printf("Key:%s does not exist\n", key);
		return;
	}
	else
	{
		printf("Key:%s, Value:%s\n", key, val);
	}
}

void print_table(t_hash_table *table)
{
	printf("\nHash Table\n-------------------\n");
	for (int i = 0; i < table->size; i++)
	{
		if (table->item[i])
		{
			printf("Index:%d, Key:%s, Value:%s\n", i, table->item[i]->key, table->item[i]->value);
		}
	}
	printf("-------------------\n\n");
}

int	next_local(int value)
{
	return value % size_table;
}
/*
void insert_hash(char* t[], char* value_insert)
{
	int index = hash_function(value_insert);
	printf("%d\n", index);
	while (t[index] != NULL)
	{
		index = next_local(index + 1);
	}
	t[index] = value_insert;
}

char* search_table(char* t[], char* value_search)
{
	int index = hash_function(value_search);
	while (t[index] != NULL)
	{
		if (ft_strcmp(t[index], value_search))
			return t[index];
		else
			index = next_local(index + 1);
	}
	return NULL;
}

void	print_table(char* t[])
{
	int i;
	i = 0;

	while(i < size_table)
	{
		printf("%d = %s\n", i, t[i]);
		i++;
	}
}
 */