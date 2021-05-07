#include "HiddenMarkovModel.hpp"

class FirstOrderHiddenMarkovModel: public HiddenMarkovModel{
    protected:
        void show(vector<float>& nums);
        void show(vector<vector<float>>& nums);
    public:
        FirstOrderHiddenMarkovModel(){
            this->start_pro.clear();
            this->transition_pro.clear();
            this->emission_pro.clear();
        }
        FirstOrderHiddenMarkovModel(vector<float>& _start_pro, vector<vector<float>>& _transition_pro,
                                    vector<vector<float>>& _emission_pro):
                                    HiddenMarkovModel(_start_pro, _emission_pro, _transition_pro){
            toLog();
        }
        vector<vector<int>> generate(int length);
        void show();
        float predict(vector<int>& observation, vector<int>& state);
};

vector<vector<int>> FirstOrderHiddenMarkovModel::generate(int length){
    vector<double> pi = logToCdf(start_pro);              // 初始状态概率向量 
    vector<vector<double>> A = logToCdf(transition_pro);  // 状态转移概率矩阵
    vector<vector<double>> B = logToCdf(emission_pro);    // 观测概率矩阵

    vector<vector<int>> xy(2, vector<int>(length));
    xy[1][0] = drawFrom(pi);            // 初始状态值
    xy[0][0] = drawFrom(B[xy[1][0]]);   // 初始状态对应观测值

    for(int t=1; t<length; t++){
        xy[1][t] = drawFrom(A[xy[1][t-1]]);
        xy[0][t] = drawFrom(B[xy[1][t]]);
    }
    
    return xy;
}

float FirstOrderHiddenMarkovModel::predict(vector<int>& observation, vector<int>& state){
    int time = observation.size();
    int max_s = start_pro.size();

    vector<float> score(max_s);
    vector<vector<int>> link(time, vector<int>(max_s));

    for(int cur_s=0; cur_s < max_s; ++cur_s)
        score[cur_s] = start_pro[cur_s] + emission_pro[cur_s][observation[0]];
    
    vector<float> pre(max_s);
    for(int t=1; t<time; ++t){
        swap(pre, score);
        score.clear();
        for(int s=0; s<max_s; ++s){
            score[s] = INT_MIN;
            for(int f=0; f<max_s; ++f){
                float p = pre[f] + transition_pro[f][s] + emission_pro[s][observation[t]];
                if(p > score[s]){
                    score[s] = p;
                    link[t][s] = f;
                }
            }
        }
    }

    float max_score = INT_MIN;
    int best_s = 0;
    for(int s=0; s<max_s; s++){
        if(score[s] > max_score){
            max_score = score[s];
            best_s = s;
        }
    }

    for(int t=time-1; t>=0; --t){
        state[t] = best_s;
        best_s = link[t][best_s];
    }

    return max_score;
}

void FirstOrderHiddenMarkovModel::show(vector<float>& nums){
    for(auto num: nums) cout<<num<<", ";
    cout<<endl;
}

void FirstOrderHiddenMarkovModel::show(vector<vector<float>>& nums){
    for(auto& num: nums){ 
            for(auto i: num){
                cout<<i<<", ";
            }
        cout<<endl;
    }
}

void FirstOrderHiddenMarkovModel::show(){
    unLog();
    cout<<"start probability: "<<endl;
    show(this->start_pro);
    cout<<"transition probability: "<<endl;
    show(this->transition_pro);
    // cout<<"emission probability: "<<endl;
    // show(this->emission_pro);
    toLog();
}