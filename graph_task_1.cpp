#include <stdio.h>
#include <limits.h>
#include <cmath>
#include <algorithm>

using namespace std;

#define INF INT_MAX

//wczytuje graf do macierzy sąsiedztwa, ustawia koszt, przepływ na początku wszędzie równy 0
//krawędzie pomiędzy zawodnikami a koszt to jaka opłata nas czeka aby zawodnik z początku krawędzi wygrał
int *** readGraph (int n) 
{
    int ***G = new int** [n+1];
    for (int i=0; i<n+1; i++)
    {
        G[i] = new int* [n+1];
        for (int j=0; j<n+1; j++)
        {
            G[i][j]=new int [2];
            G[i][j][0]=0;
            G[i][j][1]=0;
        }

    }
    for (int i=0; i<(0.5*n*(n-1)); i++)
    {
        int x, y, w, b;
        scanf ("%d %d %d %d", &x, &y, &w, &b);
        if (y==w)
        {
            y=x;
            x=w;
        }
        G[x][y][1]=(-1)*b;
        G[y][x][1]=b;
    }
    return G;
}

//szukanie maksymalnego przepływu metodą zachłanną
//tworzy sieć residualną a więc tworzy krawędzie umożliwiając przepływ tak, że jeśli tam popłyniemy to zmienimy wynik
int prepareGraph (int ***G, int n, int number_win, int B)
{
    int cost = 0;
    for (int j=1; j<=number_win; j++)
    {
        if (G[0][j][1]>0) cost+=G[0][j][1];
        G[j][0][0]=1;
    }

    for (int i=1; i<n; i++)
    {
        int win = 0;
        for (int j=0; j<n; j++)
        {
            if (i==j) continue;
            if (G[i][j][0]==G[j][i][0])
            {
                win++;
                if (G[i][j][1]>0) cost+=G[i][j][1];
                G[j][i][0]=1;
            }
            if (win==number_win) break;
        }
        G[i][n][0]=win;
        G[n][i][0]=number_win-win;
    }
    return cost;
}

int swap (int ***G, int edge[3])
{
    int u = edge[0];
    int v = edge[1];
    G[u][v][0]-=1;
    G[v][u][0]+=1;
    return edge[2];
}

int _DFS (int ***G, int n, int s, bool* visited, int res)
{
    visited[s]=true;
    int result=0;
    for (int i=0; i<=n; i++)
        if (G[s][i][0]>0)
        {
            if (!visited[i])
            {
                result = max(result, _DFS (G, n, i, visited, res+1));
            }

        }
    return max(res, result);
}


int DFS (int ***G, int n)
{
    int result=0;
    bool *visited = new bool [n+1];
    for (int i=0; i<=n; i++)
        visited[i]=false;
    for (int i=0; i<=n; i++)
    {
        int tmp=0;
        if (!visited[i])
        {
            tmp = _DFS (G, n, i, visited, 0);
            result=max(result, tmp);
        }
    }
    delete [] visited;
    return result;
}

//szuka ujemnych cykli w sieci residualnej, korzysta z algorytmu Bellmana-Forda
bool isNegativeCycle (int ***G, int n, int* COST)
{
    //(n+1) - dodatkowy wierzchołek aby mieć wierzchołek połączony z każdym innym wierzchołkiem
    int range = 0.5*n*(n-1)+3*n+1;
    int **list_edges = new int* [range];
    for (int i=0; i<range; i++) list_edges[i] = new int [3];
    range=0;
    for (int i=0; i<=n; i++)
    {
        list_edges[range][0]=n+1;
        list_edges[range][1]=i;
        list_edges[range][2]=0;
        range++;
    }
    for (int i=0; i<=n; i++)
    {
        for (int j=0; j<=n; j++)
            if (G[i][j][0]>0)
            {
                list_edges[range][0]=i;
                list_edges[range][1]=j;
                list_edges[range][2]=G[i][j][1];
                range++;
            }
    }
    int *d=new int [n+2];
    int *in=new int [n+2];
    for (int i=0; i<n+2; i++)
    {
        d[i]=INF;
        in[i]=-1;
    }
    d[n+1]=0;
    int edge_of_cycle=-1;
    //int depth=DFS(G,n)+2;
    //depth = min (depth, n+1);
    int depth = n+1;
    for (int i=0; i<=depth; i++)
        for (int j=0; j<range; j++)
        {
            int u = list_edges[j][0];
            int v = list_edges[j][1];
            int c = list_edges[j][2]; //krawędź z u -> v o koszcie c
            if (d[v] > d[u]+c)
            {
                if (d[u]!=INF) d[v] = d[u]+c;
                in[v] = j;
                if (i==depth) edge_of_cycle = j;
            }
        }
    if (edge_of_cycle==-1) 
    {
        delete [] d;
        delete [] in;
        for (int i=0; i<0.5*n*(n-1)+3*n+1; i++) delete [] list_edges [i];
        delete [] list_edges;
        return false;
    }
    int cost=0;
    for (int i=0; i<depth; i++) edge_of_cycle=in[list_edges[edge_of_cycle][0]];
    cost+=swap(G, list_edges[edge_of_cycle]);
    int edge=in[list_edges[edge_of_cycle][0]];

    while (edge!=edge_of_cycle)
    {

        cost+=swap(G, list_edges[edge]);
        edge=in[list_edges[edge][0]];
    }
    *COST+=cost;
    delete [] d;
    delete [] in;
    for (int i=0; i<0.5*n*(n-1)+3*n+1; i++) delete [] list_edges [i];
    delete [] list_edges;
    return true;
}

//zwiększa liczbę zwycięstw Bitomruka o 1
int changeGraph (int ***G, int n)
{
    int res;
    for (int i=1; i<n; i++)
        if (G[0][i][0]==1)
        {
            res=G[0][i][1];
            G[0][i][0]-=1;
            G[i][0][0]+=1;
            G[n][i][0]+=1;
            break;
        }
    for (int i=1; i<n; i++)
        G[n][i][0]+=1;
    return res;
}

//ustawia minimum win jako liczbę minimalną potrzebnych zwycięstw Bitomruka
//maksimum win to liczba zwycięstw jaką może osiągnąć bitomruk przekupując graczy tak aby budżet wystarczył
//w każdej pętli zmieniamy liczbę osiąganych zwycięstw przez Bitomruka zwiekszając o jeden 
//i za każdym razem sprawdzając czy istnieje przepływ o koszcie mniejszym niż budżet
bool bitomrukWin (int ***G, int B, int n)
{
    int minimum_win = n/2;
    int count_0 = 0;
    for (int i=1; i<n; i++)
        if (G[0][i][1]<0)
            count_0++;
    minimum_win=max(minimum_win, count_0);
    int maximum_win = n-1;
    int *list_cost = new int [n-1];
    for (int i=1; i<n; i++)
        list_cost[i-1]=G[0][i][1];
    sort (list_cost, list_cost+(n-1));
    int cost = 0;
    for (int i=0; i<n-1; i++)
    {
        if (cost<=B) cost+=list_cost[i];
        else
        {
            maximum_win=i-1;
            break;
        }
    }
    delete [] list_cost;
    cost = prepareGraph(G,n,minimum_win,B);
    for (int number_win = minimum_win; number_win<=maximum_win; number_win++)
    {
        while (cost>B && isNegativeCycle(G, n, &cost));
        if (cost<=B) return true;
        cost+=changeGraph (G, n);
    }
    return false;
}

int main ()
{
    int T;
    scanf ("%d", &T);
    while (T--)
    {
        int B, n;
        scanf ("%d", &B);
        scanf ("%d", &n);
        int ***G = readGraph (n);
        if (bitomrukWin(G, B, n))
            printf("%s\n", "TAK");
        else
            printf("%s\n", "NIE");
        for (int i=0; i<=n; i++)
        {
            for (int j=0; j<=n; j++)
                delete [] G[i][j];
            delete [] G[i];
        }
        delete [] G;

    }

}