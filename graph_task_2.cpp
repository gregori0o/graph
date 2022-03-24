#include <stdio.h>
#include <vector>

using namespace std;

//struktury wykorzystane do budowania linked list
struct node;
struct vertex;



//struktura wierzchołka, każdy wierzchołek ma jedno takie pole
//służy do budowania linked list wewnątrz setów w lexBFS, zawiera informację w którym secie się znajduje (clas)
struct vertex {
	int v;
	node* clas = NULL;
	vertex* prev = NULL;
	vertex* next = NULL;
};


//linked list która jest setem wewnątrz node
class LListVertex
{
private:
	vertex *head;
	vertex *tail;
public:
	LListVertex ()
	{
		head = new vertex ();
		tail = new vertex ();
		head -> next = tail;
		tail -> prev = head;
	}
	~LListVertex ()
	{
		delete head;
		delete tail;
	}
	bool is_empty ()
	{
		return head -> next == tail;
	}
	void add (vertex* );
	void remove (vertex* );
	int ret_first ();
};

void LListVertex :: add (vertex* new_element)
{
	new_element->next = tail;
	new_element->prev = tail->prev;
	tail->prev->next = new_element;
	tail->prev = new_element;
}

void LListVertex :: remove (vertex* act)
{
	act->prev->next=act->next;
	act->next->prev=act->prev;
}

int LListVertex :: ret_first ()
{
	return head->next->v;
}



//węzeł linked list przechowujący set w lexBFS
struct node {
	LListVertex data = LListVertex();
	int last = 0;
	node* prev = NULL;
	node* next = NULL;
};


//linked list, struktura danych stworzona do wykonania algorytmu lexBFS
class LList
{
private:
	node *head;
	node *tail;
	vertex **list;
public:
	LList ()
	{
		head = new node ();
		tail = new node ();
		head -> next = tail;
		tail -> prev = head;
		list = NULL;
	}
	~LList ()
	{
		delete head;
		delete tail;
		delete [] list;
	}
	bool is_empty ()
	{
		return head -> next == tail;
	}
	void start (int n);
	void add_after (node* );
	void remove (node* );
	int ret_pivot ();
	void make_partition (vector<int> , int );
};

//początek lexBFSa
void LList :: start (int n)
{
	add_after(head);
	node* actual = head->next;
	list = new vertex* [n+1];
	for (int i=1; i<=n; i++)
	{
		list[i]=new vertex;
		list[i]->v = i;
		list[i]->clas = actual;
		actual->data.add(list[i]);
	}
}

void LList :: add_after (node* actual)
{
	node* new_node = new node ();
	new_node->prev = actual;
	new_node->next = actual->next;

	actual->next->prev = new_node;
	actual->next = new_node;
}

void LList :: remove (node* act)
{
	act->prev->next=act->next;
	act->next->prev=act->prev;

	delete act;
}

//zwraca kolejny wierzchołek w kolejnosci lexBFS
int LList :: ret_pivot ()
{
	node* last = tail->prev;
	int result = last->data.ret_first();
	last->data.remove(list[result]);
	delete list[result];
	list[result] = NULL;
	if (last->data.is_empty())
		remove (last);
	return result;
}

//funkcja dokonująca podział setów w kolejnej iteracji lexBFS
void LList :: make_partition (vector<int> vertices, int v)
{
	for (vector<int>::iterator it = vertices.begin(); it != vertices.end(); it++)
	{
		if (list[*it]==NULL) continue;
		node* actual = list[*it]->clas;
		actual->data.remove(list[*it]);
		if (actual->last==v)
		{
			actual->next->data.add(list[*it]);
		}
		else
		{
			actual->last=v;
			add_after(actual);
			actual->next->data.add(list[*it]);
		}
		list[*it]->clas=actual->next;
		if (actual->data.is_empty())
			remove(actual);
	}
}




//wczytanie grafu
vector<int>* readGraph (int n, int m)
{
	vector<int> *G = new vector<int> [n+1];
	for (int i=0; i<m; i++)
	{
		int x, y;
        scanf ("%d %d", &x, &y);
        G[x].push_back(y);
        G[y].push_back(x);
	}
	return G;
}


//lexBFS, zwraca PEO bo graf przekątniowy
int* makePEO (vector<int>* G, int n)
{
	LList classes = LList();
	classes.start (n);
	int* result = new int [n];
	for (int i=0; i<n; i++)
	{
		int pivot = classes.ret_pivot();
		result[i]=pivot;
		classes.make_partition(G[pivot], pivot);
	}
	return result;
}


//szuka liczby klikowej tak jak na laboratorium
int findCliqueNumber (int* PEO, vector<int>* G, int n)
{
	int max_clique = 1;
	bool* is_in_PEO = new bool [n+1];
	for (int i=0; i<=n; i++)
		is_in_PEO[i] = false;

	is_in_PEO[PEO[0]] = true;
	is_in_PEO[PEO[1]] = true;
	for (int i=2; i<n; i++)
	{
		int tmp = 0;
		int v = PEO[i];
		is_in_PEO[v] = true;
		for (vector<int>::iterator it = G[v].begin(); it != G[v].end(); it++)
		{
			if (is_in_PEO[*it])
				tmp++;
		}
		tmp++;
		if (tmp > max_clique)
			max_clique = tmp;
	}
	delete [] is_in_PEO;
	return max_clique;
}


//co najmniej 2, liczba klikowa pomniejszona o 1
int numberToWin (vector<int>* G, int n)
{
	int* PEO = makePEO (G, n);
	int clique_number = findCliqueNumber (PEO, G, n);
	delete [] PEO;
	if (clique_number <= 2) return 2;
	return clique_number-1;
}


int main ()
{
	int T;
	scanf ("%d", &T);
	while (T--)
	{
		int n, m;
        scanf ("%d %d", &n, &m);
        vector<int>* G = readGraph(n, m);
        printf("%d\n", numberToWin(G, n));

        delete [] G;
	}
}