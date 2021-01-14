#include <iostream>
#include <fstream>
#include <queue>
#include <windows.h>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>
#include <unordered_map>

using namespace std;

//klasa licząca czas w milisekundach
class HighResTimer
{
    LARGE_INTEGER start;
    LARGE_INTEGER stop;
    double frequency;
public:
    HighResTimer()
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        frequency = (double)freq.QuadPart;
    }
    void StartTimer()//rozpoczęcie liczenia czasu
    {
        QueryPerformanceCounter(&start);
    }
    double StopTimer()//skończenie liczenia czasu
    {
        QueryPerformanceCounter(&stop);
        return ((LONGLONG)(stop.QuadPart - start.QuadPart)*1000.0/frequency); //wyliczenie wyniku w ms
    }
};

//struktura mrówki
struct Ant{
    vector<int> visited;
    vector<int> unVisited;
    int start;
};

//inicjalizacja nowej mrówki
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

//wygenerowanie cyklu metodą NN
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

//wyliczenie kosztu ściezki
int costRoute(vector<int>&route, vector<vector<int>>&graph){
    int sum=0;
    for (int i=0;i<route.size()-1;i++){
        sum += graph[route[i]][route[i+1]];
    }
    sum += graph[route[route.size()-1]][route[0]];
    return sum;
}

//wypełnienie wszystkich krawędzi feromonów wartością poczatkową
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


//wybranie wierzchołka na podstawie mapy z prawdopodobieństwami
int findNextVertex(map<double,int>&possibilities){
    map<double,int>:: iterator itr;
    int vertex;
    for(itr = possibilities.begin(); itr != possibilities.end(); itr++){
        vertex = itr->second;
    }
    return vertex;
}


//funckja wyliczająca prawdopodobieństwa przejścia do kolejnych nieodwiedzonych jeszcze przez mrówkę miast
int findBestChoice(vector<int>&unvisited,vector<vector<double>>&pheromones,vector<vector<int>>&graph, int&start, double &b, double&a){
    map<double,int> possibilities;
    double possible, nominator, sum = 0; // nominator to licznik
    for(int i = 0; i<unvisited.size();i++){
        sum += (double)pow(pheromones[start][unvisited[i]],a)*(double)pow(pheromones[start][unvisited[i]],b);
    }
    for(int i = 0; i<unvisited.size(); i++){
        double temp = static_cast<double>(1)/graph[start][unvisited[i]];
        nominator = (double) pow((double)pheromones[start][unvisited[i]],a)*(double)pow(temp,b);
        possible =  (double)nominator / (double)sum;
        possibilities[possible] = unvisited[i];
    }

    return findNextVertex(possibilities); //przekierowanie do metody podejmującej decyzję w kwestii wyboru miasta
}


//znalezienie najlepszej ścieżki w mapie
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

//cykliczne odparowanie feromonów w algorytmie CAS
void refreshPheromonesCAS(vector<vector<double>>&pheromones,vector<int>&route, int&cost, double&p, int &n){

    double temp = (double)(n)/(double)(cost);
    for (int i=0;i<route.size()-1;i++){
        pheromones[route[i]][route[i+1]] += temp;
    }
}

//odparowanie feromonów na trasie route
void refreshPheromonesEvaporate(vector<vector<double>>&pheromones,vector<int>&route, int&cost, double&p, int &n){

    for (int i=0;i<route.size()-1;i++){
        pheromones[route[i]][route[i+1]] = pheromones[route[i]][route[i+1]]*p;
    }
}

//dodanie feromonów na krawędziach QAS
void refreshPheromonesQAS(vector<vector<double>>&pheromones,vector<int>&route, double&p, int &n, vector<vector<int>>&graph){

    int size, last, previous;
    size = route.size();
    last = route.at(size-1);
    previous = route.at(size-2);

    pheromones[previous][last] +=  (double)(n)/(double)(graph[previous][last]);
}

//sprawdzenie czy drogi poprzednia i nowa są takie same
void checkIfRouteTheSame(vector<int>&last,vector<int>&previous,boolean &check){
    for(int i = 0; i<last.size(); i++){
        if(last.at(i) != previous.at(i)){
            check = true;
        }
    }
}


//aglorytm QAS
void AntAlgorithmQAS(vector<vector<int>>&graph, int amount, int &finalCost, vector<int>&finalRoute, int&n, double&b, double&a, double&p) {

    vector<vector<int>> finalll;
    vector<vector<double>> pheromones (n);
    vector<int> previous;
    double expectedTime = 1200000;
    HighResTimer timer;
    int start, bestChoice, finalTemp;
    boolean check;
    unordered_map<int,vector<int>>::iterator ite;
    fillStart(pheromones, n, graph, b);
    timer.StartTimer();
    for(int k = 0;k<100; k++) {
        unordered_map<int,vector<int>> routes;
        check = false;
        for (int i = 0; i < amount; i++) {
            start = rand() % n;
            Ant *ant = newAnt(start, n);
            ant->visited.push_back(start);
            for (int j = 0; j < n-1; j++) {
                bestChoice = findBestChoice(ant->unVisited, pheromones, graph, start, b, a);
                ant->visited.push_back(bestChoice);
                ant->unVisited.erase(remove(ant->unVisited.begin(), ant->unVisited.end(), bestChoice), ant->unVisited.end());
                start = bestChoice;
                refreshPheromonesQAS(pheromones, ant->visited, p, n, graph);
            }
            routes[costRoute(ant->visited, graph)] = ant->visited;
            if(i>=1){
                checkIfRouteTheSame(ant->visited,previous,check);
            }
            previous = ant->visited;
            delete ant;
            double t2 = timer.StopTimer();
            if (t2 > expectedTime) {
                return;
            }

        }
        finalRoute = findBestRoute(routes);
        finalTemp = costRoute(finalRoute, graph);
        //cout<<finalTemp<<endl;
        if(finalTemp < finalCost){
            finalCost = finalTemp;
        }
        for (ite = routes.begin(); ite != routes.end(); ite++){
            int costRoute = ite->first;
            refreshPheromonesEvaporate(pheromones, ite->second, costRoute, p, n);
        }
        if(check == false){
            return;
        }

    }
}

//algorytm CAS
void AntAlgorithmCAS(vector<vector<int>>&graph, int amount, int &finalCost, vector<int>&finalRoute, int&n, double&b, double&a, double&p) {

    vector<vector<double>> pheromones (n); // wektor  z feromonami
    unordered_map<int,vector<int>>::iterator ite;
    int start, bestChoice, tempFinal;
    double expectedTime = 1200000; // maksymalny czsa wykonania algorytmu
    HighResTimer timer;
    vector<int>previous;
    fillStart(pheromones, n, graph, b); // wypełenienie wektora ze wszystkimi krawędziami poczatkową wartości feromonów
    boolean check;
    timer.StartTimer();
    for(int k = 0;k<100; k++) {
        unordered_map<int,vector<int>> routes;
        check = false;
        for (int i = 0; i < amount; i++) {
            start = rand() % n; // wylosowanie wierzchołka startowego
            Ant *ant = newAnt(start, n); // stworzenie nowej mrówki
            ant->visited.push_back(start);
            for (int j = 0; j < n-1; j++) {
                bestChoice = findBestChoice(ant->unVisited, pheromones, graph, start, b, a); // wybranie kolejnego wierzchołka
                ant->visited.push_back(bestChoice); // dodanie go do wektora odiwedzonych
                ant->unVisited.erase(remove(ant->unVisited.begin(), ant->unVisited.end(), bestChoice), ant->unVisited.end()); // usunięcie z wektora nieodwiedzonych wierzchołka do którego przeszliśmy
                start = bestChoice;
            }
            int cost = costRoute(ant->visited, graph); // koszt przejścia przebytej ttrasy
            refreshPheromonesCAS(pheromones,ant->visited,cost,p,n); // dodanie feromonów
            routes[costRoute(ant->visited, graph)] = ant->visited; // zapisanie trasy
            if(i>=1){ // sprawdzenie czy nowa trasa jest równa poprzedniej
                checkIfRouteTheSame(ant->visited,previous,check);
            }
            previous = ant->visited;
            delete ant; // usunięcie mrówki
            double t2 = timer.StopTimer();
            if (t2 > expectedTime) {
                return;
            }

        }
        finalRoute = findBestRoute(routes); // znalezienie najlepszej trasy w jednej iteracji
        tempFinal = costRoute(finalRoute, graph); // wyliczenie jej kosztu
        if(tempFinal < finalCost){ // sprawdzenie czy nowy najniższy koszt jest niższy od globalnie najniższego
            finalCost = tempFinal;
        }
        for (ite = routes.begin(); ite != routes.end(); ite++){
            int costRoute = ite->first;
            refreshPheromonesEvaporate(pheromones, ite->second, costRoute, p, n); // wyparowanie feromonów
        }
        if(check == false){ // sprawdzenie czy wszystkie mrówki podczas jednej iteracji przypadkiem nie przebyły tej samej trasy - warunek STOPU
            return;
        }

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
    HighResTimer timer;

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
                double a = 7;
                double b = 1;
                timer.StartTimer();
                AntAlgorithmCAS(graph, n, finalCost, finalRoute, n, b, a, p);
                //AntAlgorithmQAS(graph, n, finalCost, finalRoute, n, b, a, p);
                cout<<finalCost<<endl;
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
                }
            }
            clearVectors(graph,solution);
        }
    }
    return 0;
}
