#include <queue.h>
#include <x86/memory.h>

/* Pega e remove o primeiro elemento da lista. */
uint8_t queue_get_data(queue_t *q, uint8_t *data)
{
	/* Caso da lista estar vazia. */
	if(q->id_first == q->id_last){
		return 0;
	}

	/* Pega o elemento e incrementa o id do primeiro item. */
	(*data) = q->data[q->id_first++];

	/* Retorna para o começo do array, se id_first>=size. */
	if(q->id_first >= q->size){
		q->id_first = 0;
	}

	return 1;
}

uint8_t queue_insert_data(queue_t *q, uint8_t data)
{
	uint32_t i;

	i = q->id_last + 1;
	if(i >= q->size){
		i = 0;
	}

	/* Caso a lista esteja cheia. */
	if(i == q->id_first){
		return false;
	}

	/* Coloca o dado na lista e seta o id(id_last) da próxima inserção. */
	q->data[q->id_last] = data;
	q->id_last = i;

	return true;
}

uint8_t queue_init(queue_t *q, uint32_t sz)
{
	q->data = kmalloc(sz * sizeof(char));
	q->size = sz;

	return q->data != NULL;
}
