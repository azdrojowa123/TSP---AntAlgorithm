#include <iostream>
#include <fstream>
#include <iostream>
#include <queue>
#include <windows.h>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#include <unordered_map>
using namespace std;

struct Ant{
    vector<int> visited;
    vector<int> unVisited;
    int start;
};

Ant* newAnt(int start, int n){
    vector<int> temp;
    for(int i = 0; i<n ; i++){
        if(i != start){
            temp.push_back(i);
        }

    }
    Ant* ant = new Ant;
    ant->start = start;
    ant->visited = vector<int>();
    ant->unVisited = temp;

    return ant;
}



void fillStart(vector<vector<double>> &pheromones, int &n, vector<vector<int>>&graph, int &b){
    double startVal;
    for(int j = 0; j<n; j++){
        pheromones[j].resize(n);
    }
    for(int i = 0; i<n ;i++){
        for(int j = 0; j<n ;j++){
            startVal = 1/(pow(graph[i][j],b));
            pheromones[i][j] = startVal;
        }
    }
}

int findNextVertex(unordered_map<int,double>&possibilities){
    unordered_map<int,double>:: iterator itr, best;
    double sum = 0, randNumber= ((double)rand() / RAND_MAX);
    int vertex;
    for(itr = possibilities.begin(); itr != possibilities.end(); itr++){
        sum += itr->second;
        best = itr;
        if(sum > randNumber){
            vertex = itr->first;
            break;
        }
    }

    return vertex;
}

int findBestChoice(vector<int>&unvisited,vector<vector<double>>&pheromones,vector<vector<int>>&graph, int&start, int &b, int&a){
    unordered_map<int,double> possibilities;
    double possible, nominator, denominator, sum = 0; // nominator to licznik
    for(int i = 0; i<unvisited.size();i++){
        sum += pheromones[start][unvisited[i]];
    }
    for(int i = 0; i<unvisited.size(); i++){
        double temp = static_cast<double>(1)/graph[start][unvisited[i]];
        nominator = pow(pheromones[start][unvisited[i]],a)*pow(temp,b);
        denominator = pow(sum,a)*pow(sum,b);
        possible =  nominator / denominator;
        possibilities[unvisited[i]] = possible;
    }

    return findNextVertex(possibilities);
}

int costRoute(vector<int>&route, vector<vector<int>>&graph){
    int sum=0;
    for (int i=0;i<route.size()-1;i++){
        sum += graph[route[i]][route[i+1]];
        cout<<"sum: "<<sum<<endl;
    }
    sum += graph[route[route.size()-1]][route[0]];
    return sum;
}

vector<int> findBestRoute(unordered_map<int, vector<int>>&lists){
    unordered_map<int,vector<int>>:: iterator itr;
    double min = DBL_MAX;
    vector<int> bestRoute;
    for(itr = lists.begin(); itr != lists.end(); itr++){
        if(itr->first < min){
            min = itr->first;
            bestRoute = itr->second;
        }
    }

    return bestRoute;
}

void refreshPheromones(vector<vector<double>>&pheromones,vector<int>&route, int&cost, double&p){
    for (int i=0;i<route.size()-1;i++){
        pheromones[route[i]][route[i+1]] = pheromones[route[i]][route[i+1]]*p + 1/cost;
    }
}


void AntAlgorithm(vector<vector<int>>&graph, int amount,int &finalCost,vector<int>&finalRoute, int&n, int&b, int&a, double&p) {

    unordered_map<int,vector<int>> routes;
    vector<vector<double>> pheromones (n);
    int start, bestChoice;
    fillStart(pheromones, n, graph, b);
    for(int k = 0;k<10; k++) {
        for (int i = 0; i < amount; i++) {
            start = rand() % n;
            Ant *ant = newAnt(start, n);
            ant->visited.push_back(start);
            for (int j = 0; j < n; j++) {
                bestChoice = findBestChoice(ant->unVisited, pheromones, graph, start, b, a);
                cout<<bestChoice;
                ant->visited.push_back(bestChoice);
                cout<<"Visited"<<endl;
                for(int k = 0;k<ant->visited.size();k++){
                    cout<<ant->visited[k]<<" ";
                } cout<<endl;
                ant->unVisited.erase(remove(ant->unVisited.begin(), ant->unVisited.end(), bestChoice), ant->unVisited.end());
                cout<<"UNVisited"<<endl;
                for(int k = 0;k<ant->unVisited.size();k++){
                    cout<<ant->unVisited[k]<<" ";
                } cout<<endl;
            }
            routes[costRoute(ant->visited, graph)] = ant->visited;
            delete ant;

        }
        finalRoute = findBestRoute(routes);
        finalCost = costRoute(finalRoute, graph);
        refreshPheromones(pheromones, finalRoute, finalCost, p);
    }
}


//czytanie z pliku
void readFromFile(string s, vector<vector<int>> &graph, int &n){

    int weight;
    ifstream myFile;
    myFile.open(s);
    if(myFile.is_open()){
        myFile>>n;
        for(int i=0;i<n;i++){
            graph.push_back(vector<int>());
            for(int j=0; j < n; j++){
                myFile>>weight;
                graph[i].push_back(weight);
            }
        }
    }
    else{
        cout<<"File is not read properly";
    }
    myFile.close();

}

//funkcja konwertująca wektor intów na string
string printSolution(vector<int> &solution){
    stringstream ss;
    for(int i=0;i<solution.size();i++){

        if(i != 0){
            ss << " ";
        }
        ss << solution[i];
    }
    return ss.str();

}

//funkcja czyszcząca wektory
void clearVectors(vector<vector<int>> &graph ,vector<int> &solution){

    for (int i=0;i<graph.size();i++){
        graph[i].clear();
    }
    graph.clear();
    solution.clear();
}


int main() {
    string line, csvName, dataFile;
    char bracket;
    int repeat, optimumCost, n, temp2;
    vector<int> checked;
    vector<int> solution;
    vector<int> route;
    vector<vector<int>> graph;
    ofstream csvFile;
    //HighResTimer timer;

    ifstream myInitFile;
    myInitFile.open("initialiaze.INI");

    // wyłuskanie z ostatniej linii pliku .INI nazwy pliku wyjściowego
    while (getline(myInitFile, line)) {
        csvName = line;
    }

    myInitFile.close();
    myInitFile.open("initialiaze.INI");

    // ponowne otwarcie pliku
    if (myInitFile.is_open()) {
        while (getline(myInitFile, line)) {             // wczytanie kolejnej lini
            if (line.find(".csv") != std::string::npos) { // sprawdzenie czy linia zawiera rozszerzenie .csv
                break;
            }
            istringstream stream(line);
            stream >> dataFile >> repeat >> optimumCost >>bracket; // wczytanie nazwy instancji, ilosci powtórzeń, kosztu optymalnego
            readFromFile(dataFile,graph,n); // wypełnienie zmiennych graph oraz fullgraph danymi
            for (int i = 0; i <= n; i++) { // wczytanie ścieżki z nawiasu kwadratowego
                stream >> temp2;
                solution.push_back(temp2);
            }

            for (int j = 0; j < repeat; j++) {
                int finalCost = 0;
                vector<int>finalRoute;
                double p = 0.5;
                int a = 1;
                int b = 2;
                AntAlgorithm(graph, 10,finalCost,finalRoute, n, b, a, p);
                cout<<finalCost<<endl;

                /*timer.StartTimer();
                int t2 = timer.StopTimer(); //skończenie liczenia czasu
                if(j == 0){
                    csvFile.open(csvName,  std::ios::out |  std::ios::app);
                    csvFile<<dataFile<<","<<repeat<<","<<optimumCost<<",\n";
                    csvFile<<finalCost<<","<<t2<<"\n";
                    csvFile.close();
                }else{
                    csvFile.open(csvName,  std::ios::out |  std::ios::app);
                    csvFile<<finalCost<<","<<t2<<"\n";
                    csvFile.close();
                }*/
            }
            clearVectors(graph,solution);
        }
    }
    return 0;
}
