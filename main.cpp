#include <iostream>
#include <fstream>
#include <iostream>
#include <queue>
#include <windows.h>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>
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

//funkcja sprawdzająca czy w wektorze znajduje się już dany wierzchołek
bool checkIfExsist(vector<int> route, int i){
    for(int temp : route){
        if (temp == i ){
            return true;
        }
    }
    return false;
}

vector<int> generateRandomCycleNN(vector<vector<int>> &graph, int& n){
    vector<int> route;
    int j = 0, max, next = 0, temp = 0;
    while(j<n){
        max = INT_MAX;
        next = temp;
        route.push_back(next);
        for(int i = 0 ; i<n ; i++ ){
            if(!checkIfExsist(route,i) && i!=next){
                if (graph[next][i] < max) {
                    temp = i;
                    max = graph[next][i];
                }
            }
        }
        j++;
    }
    return route;
}

int costRoute(vector<int>&route, vector<vector<int>>&graph){
    int sum=0;
    for (int i=0;i<route.size()-1;i++){
        sum += graph[route[i]][route[i+1]];
    }
    sum += graph[route[route.size()-1]][route[0]];
    return sum;
}


void fillStart(vector<vector<double>> &pheromones, int &n, vector<vector<int>>&graph, double &b){
    for(int j = 0; j<n; j++){
        pheromones[j].resize(n);
    }
    vector<int> route = generateRandomCycleNN(graph,n);
    double costRandom = (double)costRoute(route, graph);
    for(int i = 0; i<n ;i++){
        for(int j = 0; j<n ;j++){
            pheromones[i][j] = (double)n/costRandom;
        }
    }
}



int findNextVertex(map<double,int>&possibilities){
    map<double,int>:: iterator itr;
    double sum = 0, randNumber = ((double)rand() / RAND_MAX);
    int vertex;
    for(itr = possibilities.begin(); itr != possibilities.end(); itr++){
        sum += itr->first;
        vertex = itr->second;
        if(sum > randNumber){
            break;
        }
    }

    return vertex;
}



int findBestChoice(vector<int>&unvisited,vector<vector<double>>&pheromones,vector<vector<int>>&graph, int&start, double &b, double&a){
    map<double,int> possibilities;
    double possible, nominator, denominator, sum = 0; // nominator to licznik
    for(int i = 0; i<unvisited.size();i++){
        sum += pheromones[start][unvisited[i]];
    }
    for(int i = 0; i<unvisited.size(); i++){
        double temp = static_cast<double>(1)/graph[start][unvisited[i]];
        nominator = (double) pow((double)pheromones[start][unvisited[i]],a)*(double)pow(temp,b);
        denominator = (double)pow(sum,a)*(double)pow(sum,b);
        possible =  (double)nominator / (double)denominator;
        possibilities[possible] = unvisited[i];
    }

    return findNextVertex(possibilities);
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

void refreshPheromones(vector<vector<double>>&pheromones,vector<int>&route, int&cost, double&p, int &n){

    for(int j = 0; j<n ;j++){
        for(int k = 0; k<n ;k++){
            pheromones[j][k] *= p;
        }
    }
    double temp = (double)(n)/(double)(cost);
    for (int i=0;i<route.size()-1;i++){
        pheromones[route[i]][route[i+1]] += temp;
        pheromones[route[i+1]][route[i]] += temp;

    }
}

void refreshPheromonesQAS(vector<vector<double>>&pheromones,vector<int>&route, int&cost, double&p, int &n, vector<vector<int>>&graph){

    for(int j = 0; j<n ;j++){
        for(int k = 0; k<n ;k++){
            pheromones[j][k] *= p;
        }
    }
    for (int i=0;i<route.size()-1;i++){
        pheromones[route[i]][route[i+1]] = pheromones[route[i]][route[i+1]]*p + (double)(n)/(double)(graph[route[i]][route[i+1]]);
        //pheromones[route[i+1]][route[i]] += (double)(n)/(double)(graph[route[i+1]][route[i]]);

    }
}

void AntAlgorithmQAS(vector<vector<int>>&graph, int amount, int &finalCost, vector<int>&finalRoute, int&n, double&b, double&a, double&p) {


    vector<vector<int>> finalll;
    vector<vector<double>> pheromones (n);
    int start, bestChoice, finalTemp;
    fillStart(pheromones, n, graph, b);
    for(int k = 0;k<1000; k++) {
        unordered_map<int,vector<int>> routes;
        for (int i = 0; i < amount; i++) {
            start = rand() % n;
            Ant *ant = newAnt(start, n);
            ant->visited.push_back(start);
            for (int j = 0; j < n-1; j++) {
                bestChoice = findBestChoice(ant->unVisited, pheromones, graph, start, b, a);
                ant->visited.push_back(bestChoice);
                ant->unVisited.erase(remove(ant->unVisited.begin(), ant->unVisited.end(), bestChoice), ant->unVisited.end());
                start = bestChoice;
            }
            routes[costRoute(ant->visited, graph)] = ant->visited;
            delete ant;

        }
        finalRoute = findBestRoute(routes);
        finalTemp = costRoute(finalRoute, graph);
        if(finalTemp < finalCost){
            finalCost = finalTemp;
        }
        refreshPheromonesQAS(pheromones, finalRoute, finalTemp, p, n, graph);
    }
}


void AntAlgorithmCAS(vector<vector<int>>&graph, int amount, int &finalCost, vector<int>&finalRoute, int&n, double&b, double&a, double&p) {

    vector<vector<double>> pheromones (n);
    int start, bestChoice, tempFinal;
    fillStart(pheromones, n, graph, b);
    for(int k = 0;k<1000; k++) {
        unordered_map<int,vector<int>> routes;
        for (int i = 0; i < amount; i++) {
            start = rand() % n;
            Ant *ant = newAnt(start, n);
            ant->visited.push_back(start);
            for (int j = 0; j < n-1; j++) {
                bestChoice = findBestChoice(ant->unVisited, pheromones, graph, start, b, a);
                ant->visited.push_back(bestChoice);
                ant->unVisited.erase(remove(ant->unVisited.begin(), ant->unVisited.end(), bestChoice), ant->unVisited.end());
                start = bestChoice;
            }
            routes[costRoute(ant->visited, graph)] = ant->visited;
            delete ant;

        }
        finalRoute = findBestRoute(routes);
        tempFinal = costRoute(finalRoute, graph);
        if(tempFinal < finalCost){
            finalCost = tempFinal;
        }
        refreshPheromones(pheromones, finalRoute, tempFinal, p, n);
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
                int finalCost = INT_MAX;
                vector<int>finalRoute;
                double p = 0.5;
                double a = 1;
                double b = 3;
                //AntAlgorithmCAS(graph, n, finalCost, finalRoute, n, b, a, p);
                AntAlgorithmQAS(graph, n, finalCost, finalRoute, n, b, a, p);
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
