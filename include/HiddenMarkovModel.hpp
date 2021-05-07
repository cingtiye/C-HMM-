#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
using namespace std;

class HiddenMarkovModel{
    protected:
        vector<float> start_pro;
        vector<vector<float>> emission_pro;
        vector<vector<float>> transition_pro;

        void toLog();
        vector<double> logToCdf(vector<float>& log);
        vector<vector<double>> logToCdf(vector<vector<float>>& log);
        int drawFrom(vector<double>& cdf);
        void normalize(vector<float>& freq);
        void estimateTransitionProbability(vector<vector<vector<int>>>& samples, int max_state);
        void estimateStartProbability(vector<vector<vector<int>>>& samples, int max_state);
        void estimateEmissionProbability(vector<vector<vector<int>>>& samples, int max_state, int max_obser);
    public:
        HiddenMarkovModel(){}
        HiddenMarkovModel(vector<float>& _start_pro, vector<vector<float>>& _emission_pro, 
                            vector<vector<float>>& _transition_pro): start_pro(_start_pro), 
                            emission_pro(_emission_pro), transition_pro(_transition_pro){}
        ~HiddenMarkovModel(){}
        void unLog();
        virtual vector<vector<int>> generate(int length) = 0;
        vector<vector<vector<int>>> generate(int minLength, int maxLength, int size);
        void train(vector<vector<vector<int>>>& samples);
};

void HiddenMarkovModel::train(vector<vector<vector<int>>>& samples){
    if(samples.empty()) return;
    int max_state = 0;
    int max_obser = 0;
    int N = -1;
    vector<int> empty_vec;
    for(auto& sample: samples){
        N++;
        if(sample.empty() || sample[0].empty() || sample[1].empty()) {
            empty_vec.emplace_back(N);
            continue;
        }
        if(sample.size() != 2 || sample[0].size() != sample[1].size())
            throw invalid_argument("sample.size() == 2 && sample[0].size() == sample[1].size()");
        
        max_obser  = max(max_obser, *max_element(sample[0].begin(), sample[0].end()));
        max_state  = max(max_state, *max_element(sample[1].begin(), sample[1].end()));
    }
    for(int i = (int)empty_vec.size()-1; i>=0; i--) samples.erase(samples.begin()+empty_vec[i]);
    estimateStartProbability(samples, max_state);
    estimateTransitionProbability(samples, max_state);
    estimateEmissionProbability(samples, max_state, max_obser);
    toLog();
}

void HiddenMarkovModel::toLog(){
    if(start_pro.empty() || transition_pro.empty() || emission_pro.empty()) return;
    for(int i = 0; i < (int)start_pro.size(); i++){
        start_pro[i] = (float)log(start_pro[i]);
        for(int j = 0; j < (int)start_pro.size(); j++)
            transition_pro[i][j] = (float)log(transition_pro[i][j]);
        for(int j = 0; j < (int)emission_pro[0].size(); j++)
            emission_pro[i][j] = (float)log(emission_pro[i][j]);
    }
}

void HiddenMarkovModel::unLog(){
    if(start_pro.empty() || transition_pro.empty() || emission_pro.empty()) return;
    for(int i = 0; i < (int)start_pro.size(); i++){
        start_pro[i] = (float)exp(start_pro[i]);
        for(int j = 0; j < (int)start_pro.size(); j++)
            transition_pro[i][j] = (float)exp(transition_pro[i][j]);
        for(int j = 0; j < (int)emission_pro[0].size(); j++)
            emission_pro[i][j] = (float)exp(emission_pro[i][j]);
    }
}

vector<double> HiddenMarkovModel::logToCdf(vector<float>& log){
    vector<double> cdf((int)log.size());
    cdf[0] = exp(log[0]);
    for(int i=1; i<(int)cdf.size()-1; i++) 
        cdf[i] = cdf[i-1] + exp(log[i]);
    cdf[(int)cdf.size() - 1] = 1.0;
    return cdf;
}
vector<vector<double>> HiddenMarkovModel::logToCdf(vector<vector<float>>& log){
    vector<vector<double>> cdf((int)log.size(), vector<double>((int)log[0].size()));
    for(int i=0; i<(int)log.size(); i++)
        cdf[i] = logToCdf(log[i]);
    return cdf;
}

int HiddenMarkovModel::drawFrom(vector<double>& cdf){
    return lower_bound(cdf.begin(), cdf.end(), rand()/double(RAND_MAX)) - cdf.begin();
}

vector<vector<vector<int>>> HiddenMarkovModel::generate(int minLength, int maxLength, int size){
    vector<vector<vector<int>>> samples(size);
    for(int i=0; i<size; i++){
        int _size = (int)(rand() % (maxLength - minLength + 1)) + minLength;
        samples[i] = generate(_size);
    }
    return samples;
}

void HiddenMarkovModel::normalize(vector<float>& freq){
    float sum = accumulate(freq.begin(), freq.end(), 0.);
    for(auto& num: freq) num /= sum;
};

void HiddenMarkovModel::estimateTransitionProbability(vector<vector<vector<int>>>& samples, 
                                                        int max_state){
    transition_pro.resize(max_state+1, vector<float>(max_state+1));
    for(auto& sample: samples){
        int prev_s = sample[1][0];
        for(int i=1; i<(int)sample[1].size(); i++){
            int cur_s = sample[1][i];
            ++transition_pro[prev_s][cur_s];
            prev_s = cur_s;
        }
    }
    
    for(auto& nums: transition_pro) normalize(nums);
}

void HiddenMarkovModel::estimateStartProbability(vector<vector<vector<int>>>& samples, 
                                                 int max_state){
    start_pro.resize(max_state+1);
    for(auto& sample: samples)
        ++start_pro[sample[1][0]];

    normalize(start_pro);
}

void HiddenMarkovModel::estimateEmissionProbability(vector<vector<vector<int>>>& samples, 
                                                    int max_state, int max_obser){
    emission_pro.resize(max_state+1, vector<float>(max_obser + 1));
    for(auto& sample: samples)
        for(int i=0; i<(int)sample[1].size(); i++)
            ++emission_pro[sample[1][i]][sample[0][i]];

    for(auto& nums: emission_pro) normalize(nums);
}