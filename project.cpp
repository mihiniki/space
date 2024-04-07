#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <conio.h>

extern "C"
{
#include "curl/curl.h"
}
const int LAST = 7;

namespace fs = std::filesystem;

std::string takePasswdFromUser() 
{ 
    std::string input = ""; 
    char inputSymb; 
    while (true) { 
        inputSymb = getch(); 
  
        if (inputSymb < 32) { 
            std::cout << std::endl; 
            return input; 
        } 
        input.push_back(inputSymb); 
    } 

    
} 

int distanceFromEquator(std::string town){
    if(town == "Kourou"){
        return 1;
    }
    else if(town == "Mahia"){
        return 2;
    }
    else if(town == "Tangeshima"){
        return 3;
    }
    else if(town == "Cape Canaveral"){
        return 4;
    }
    else if(town == "Kodiak"){
        return 5;
    }

    return -1;
}

std::vector<std::vector<std::string>> takeInfoFromFile(std::string fileName){
    std::fstream file;

    file.open(fileName, std::ios::in);

    std::vector<std::vector<std::string>> buffer;
    int line = 0;

    std::string row;
 
    std::string word;

    std::vector <std::string> data;
    while(std::getline(file, row)){
        std::stringstream s(row);

        data.clear();

        while(std::getline(s, word, ' ')){
            data.push_back(word);
        }

        buffer.push_back(data);

    }

    file.close();

return buffer;
}

bool criteriaDay(std::vector<std::string> buffer){
    int temp = std::stoi(buffer[0]);
    int windInt = std::stoi(buffer[1]);
    int hum = std::stoi(buffer[2]);
    int prec = std::stoi(buffer[3]);
    std::string lightining = buffer[4];
    std::string clouds = buffer[5];

    return temp >= 1 && temp <= 32 && windInt <= 11 && hum < 50 && prec == 0 
            && lightining == "No" && clouds != "Cumulus" && clouds != "Nimbus";
}

std::vector<std::vector<std::string>> dataFromColsToRows(std::vector<std::vector<std::string>> table){
    std::vector<std::vector<std::string>> newTable;
        for(int i = 1 ; i < table[i].size(); i++){
                std::vector<std::string> buffer;
                for(int j = 1; j < table.size(); j++){
                    buffer.push_back(table[j][i]);
                }
            
                newTable.push_back(buffer);
            }

    return newTable;
}

std::vector<std::string> bestDayFromAllTowns(std::vector<std::vector<std::string>> table){
    int minDistanceFromEq = 0;
    for(int i = 0; i < table.size(); i++){
        if(distanceFromEquator(table[i][LAST]) < distanceFromEquator(table[minDistanceFromEq][LAST])){
            minDistanceFromEq = i;
        }
    }
    std::vector<std::string> result;
    result.push_back(table[minDistanceFromEq][LAST]);
    result.push_back(table[minDistanceFromEq][LAST - 1]);
    return result;
}
int bestDay(std::vector<std::vector<std::string>> table){
    
    std::vector<std::vector<std::string>> newTable = dataFromColsToRows(table);
  

    int size = newTable.size();

    int minWindSpeedIdx = -1;
    int minHumidityIdx = -1;
    for(int i = 0; i < size; i ++){
        if(criteriaDay(newTable[i])){
            minWindSpeedIdx = i;
            minHumidityIdx = i;
            break;
        }
    }
    if(minWindSpeedIdx == -1){
        throw std::invalid_argument("No good day!");
        return -1;
    }
    for(int i = 0; i < size; i++){
        if(criteriaDay(newTable[i])){
          
            if(std::stoi(newTable[i][1]) < std::stoi(newTable[minWindSpeedIdx][1])){
                minWindSpeedIdx = i;
            }
            if(std::stoi(newTable[i][2]) < std::stoi(newTable[minHumidityIdx][2])){
                minHumidityIdx = i;
            }
        }

    }
    if(minWindSpeedIdx == minHumidityIdx){
        return minWindSpeedIdx + 1;
    }
    else{
        int day;
        std::cout<<"Choose which day you preffer: "<< std::endl;
        std::cout<<"Day " << minWindSpeedIdx + 1<< ": Almost no wind"<<std::endl;
        std::cout<<"Day " << minHumidityIdx + 1<< ": Almost no humidity"<<std::endl;
        std::cout<<"You choise: ";
        std::cin>>day;

        return day;
    }
}

void createNewFile(std::vector<std::vector<std::string>> table){
    std::fstream file;

    file.open("LaunchAnalysisReport.csv", std::ios::out | std::ios::app); 

    file << "Name  " << "Date";
    file << "\n";
    for(int i = 0; i < table.size(); i++){
        file<< table[i][LAST] << "  ";
        file<< table[i][LAST - 1] + ".06";
        file<<"\n";
    }

    file.close();
}

std::vector<std::vector<std::string>> infoForAllFiles(std::string directoryPath){

       std::vector<std::vector<std::string>> infoForAll;
        for (const auto& p : fs::directory_iterator(directoryPath)) {
        if (fs::is_regular_file(p.path())) {

            std::string filename = p.path().filename().generic_string();
            std::string file = directoryPath +"/" +filename;
            std::vector<std::vector<std::string>>buff = takeInfoFromFile(file);
            std::vector<std::vector<std::string>> info = dataFromColsToRows(buff);
            std::string town = filename.erase(filename.find(".csv"), 4);
            std::cout << std::endl << town << std::endl;
            int wantedDay = bestDay(buff);
            
            info[wantedDay - 1].push_back(std::to_string(wantedDay));
            info[wantedDay - 1].push_back(town);
            infoForAll.push_back(info[wantedDay - 1]);
        }else{
            std::cerr<< "Problem with file! " << std::endl;
        }
    }
    return infoForAll;
}

int main() {
    std::string directoryPath;
    std::string emailSender;
    std::string password;
    std::string receiver;

    std::cout<< "Path to directory: ";
    std::cin >> directoryPath;
    std::cout<< std::endl;
    std::cout<< "Sender email: ";
    std::cin >> emailSender;
    std::cout<< std::endl;
    std::cout<< "Password: ";
    takePasswdFromUser();
    std::cout<< std::endl;
    std::cout<< "Receiver email: ";
    std::cin >> receiver;
    std::cout<< std::endl;

    
    std::vector<std::vector<std::string>> infoForAll = infoForAllFiles(directoryPath);
    std::cout << std::endl<< "RESULT FOR EVERY TOWN: "<< std::endl;
    for(int i = 0 ; i < infoForAll.size(); i++){
        int size = infoForAll[i].size();
        std::cout<<infoForAll[i][size - 1] << " " << infoForAll[i][size - 2] + ".06";
        std::cout<<std::endl;
    }

    std::vector<std::string> res = bestDayFromAllTowns(infoForAll);
    std::cout<< std::endl << "BEST DAY FROM ALL TOWNS" << std::endl;
    std::cout<< res[0] << " "<< res[1] + ".06"<< std::endl;

    createNewFile(infoForAll);
   
}