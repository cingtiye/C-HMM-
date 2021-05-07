#include <iostream>
#include <climits> 
#include <ctime>
#include <string>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include "include/FirstOrderHiddenMarkovModel.hpp"
#include "include/TransChinese.hpp"
using namespace std;
using namespace transchinese;

struct LinkList{
    int state;
    int obser;
    LinkList* next;
    LinkList(int _state, int _obser): state(_state), obser(_obser), next(nullptr){}
};

unordered_map<string, int> CHAR2INDEX;
unordered_map<string, int> STATE2INDEX = {{"B", 0}, {"M", 1}, {"E", 2}, {"S", 3}};
int UNK = 0;
unordered_map<int, string> INDEX2CHAR;
unordered_map<int, string> INDEX2STATE = {{0, "B"}, {1, "M"}, {2, "E"}, {3, "S"}};

LinkList* Add(int x, int y, LinkList* link){
    // x: obser y: state
    LinkList* xy = new LinkList(y, x);
    link->next = xy;
    link = link->next;
    return link;
}

void Cut(FirstOrderHiddenMarkovModel* FoHMM, string const train_file){
    ifstream ifs(train_file, ios::in);
    if(!ifs){throw invalid_argument("Open Error !"); return;}
    
    string sentence;
    int num = 0, sentences_size = 0;
    LinkList* root = new LinkList(-1, -1);
    LinkList* sequenceLink = root;
    while(getline(ifs, sentence)){
        wstring wsentence = transchinese::s2ws(sentence);
        int senten_len = wsentence.size(); sentences_size++;
        for(int i=0; i<senten_len; i++){
            string word = transchinese::ws2s(wsentence.substr(i, 1));
            if(CHAR2INDEX.find(word) == CHAR2INDEX.end()) {
                num++;
                CHAR2INDEX[word] = num;
                INDEX2CHAR[num] = word;
            }
        }
        
        istringstream iss(sentence);
        string words;
        while(iss >> words){
            wstring wwords = transchinese::s2ws(words);
            int x = -1, y = -1;  
            int n = wwords.size();
            if(n == 1)
                sequenceLink = Add(CHAR2INDEX[transchinese::ws2s(wwords.substr(0, 1))], STATE2INDEX["S"], sequenceLink);
            else{
                sequenceLink = Add(CHAR2INDEX[transchinese::ws2s(wwords.substr(0, 1))], STATE2INDEX["B"], sequenceLink);

                for(int i=1; i<n-1; i++)     
                    sequenceLink = Add(CHAR2INDEX[transchinese::ws2s(wwords.substr(i, 1))], STATE2INDEX["M"], sequenceLink);   

                sequenceLink = Add(CHAR2INDEX[transchinese::ws2s(wwords.substr(n-1, 1))], STATE2INDEX["E"], sequenceLink);   
            }
        }
        sequenceLink = Add(-1, -1, sequenceLink);   
    }
    ifs.close();

    vector<vector<vector<int>>> samples(sentences_size);
    vector<vector<int>> temp(2);
    int N = 0;
    root = root->next;
    while(root != nullptr){
        if(root->state == -1 &&  root->obser == -1){
            samples[N++] = temp;
            temp[0].clear(); temp[1].clear();
        }
        else{
            temp[0].emplace_back(root->obser);
            temp[1].emplace_back(root->state);
        }
        root = root->next; 
    }

    // FirstOrderHiddenMarkovModel* trained_FoHMM = new FirstOrderHiddenMarkovModel();
    FoHMM->train(samples);
    // trained_FoHMM->show();
    // cout<<endl;

}

void Segment(FirstOrderHiddenMarkovModel* FoHMM, string const test_file, 
                                                 string const output_file=""){
    ifstream ifs(test_file, ios::in);
    if(!ifs){throw invalid_argument("Open Error !"); return;}
    ofstream ofs(output_file, ios::out | ios::trunc);
    if(!ofs){throw invalid_argument("Open Error !"); return;}

    string sentence;
    while(getline(ifs, sentence)){
        wstring wsentence = transchinese::s2ws(sentence);
        int senten_len = wsentence.size();

        vector<int> observation(senten_len);
        vector<int> tagArray(senten_len);
        vector<string> output;
        string result;
        for(int i=0; i<senten_len; i++){
            string word = transchinese::ws2s(wsentence.substr(i, 1));
            if(CHAR2INDEX.find(word) != CHAR2INDEX.end()) observation[i] = CHAR2INDEX[word];
            else observation[i] = UNK;
        }
        float p = FoHMM->predict(observation, tagArray);
        result.append(transchinese::ws2s(wsentence.substr(0, 1)));
        for(int i=1; i<senten_len; i++){
            if(INDEX2STATE[tagArray[i]] == "B" || INDEX2STATE[tagArray[i]] == "S"){
                output.emplace_back(result);
                result.clear();
            }
            result.append(transchinese::ws2s(wsentence.substr(i, 1)));
        }
        if(result.size() != 0)
            output.emplace_back(result);
        
        
        for(int i=0; i<(int)output.size() - 1; i++){
            ofs << output[i] << "  ";
        }
        ofs << output[(int)output.size() - 1]; // 换行符
    }
    ifs.close();
    ofs.close();
}

int main(){

    string train_file = "./data/MSR/training/msr_training.utf8";
    string test_file = "./data/MSR/testing/msr_test.utf8";
    string output_file = "./data/MSR/testing/hmm_output.txt";
    FirstOrderHiddenMarkovModel* trained_FoHMM = new FirstOrderHiddenMarkovModel();
    cout << "Traing HMM ..."<<endl;
    Cut(trained_FoHMM, train_file);
    cout << "Segment ..."<<endl;
    Segment(trained_FoHMM, test_file, output_file);
    cout << "Result has been saved to "<<output_file <<endl;
    return 0;
}