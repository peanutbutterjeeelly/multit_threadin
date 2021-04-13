#include<iostream>
#include<vector>
#include<thread>
#include<mutex>
#include<stdlib.h>
#include<condition_variable>
#include <time.h>

using namespace std;
vector<int> load_order(5);
vector<int> pickup_order(5);
vector<int> Global_buffer(5);
const vector<int> partW_manufacture_time{50, 50, 60, 60, 70};
const vector<int> move_time{20, 20, 30, 30,40};
const vector<int> prodW_assemble_time{60, 60, 70, 70, 80};
mutex m1;
condition_variable partW, productW;
bool is_initial_state(const vector<int>&Global_buffer){//check whether if it's 0,0,0,0,0
    for(auto const& item:Global_buffer){
        if(item!=0){
            return false;
        }
    }
    return true;
}
template<class T>
ostream& operator<<(ostream& str, const vector<T>& vec)
{
    str << "<";
    for (auto const& i : vec) {
        str << " " << i;
    }
    str << " >";
    return str;
}
vector<int> operator+(const vector<int>&order, const vector<int>& buffer_state){
    //cout<< "order size is: "<<order.size()<<endl;
    vector<int> res(order.size());
    //res.resize(order.size());
    for(int i=0;i<order.size();i++){
        res[i]=order[i]+buffer_state[i];
        cout<<res[i]<<endl;
    }
    //cout<<res;
    return res;
}
vector<int> operator-(const vector<int>& buffer_state, const vector<int>&order){
    vector<int> res(order.size());
    //res.resize(order.size());
    for(int i=0;i<order.size();i++){
        res[i]=buffer_state[i]-order[i];
        //cout<<res[i]<<endl;
    }
    //cout<<res;
    return res;
}

int getSum(const vector<int>& vec){
    int sum=0;
    for(auto const& i:vec){
        sum+=i;
    }
    return sum;
}

vector<int> generate_order(vector<int>& order) {
    srand((unsigned) time(NULL));
    int curr_loadOrder_sum=getSum(order);
    for (int i = curr_loadOrder_sum; i < 5; i++) {
        int pos_to_add = rand() % 5;
        ++order[pos_to_add];
    }
    return load_order;
}
bool is_overFlow(const vector<int>& order, const vector<int>& buffer_state){

}
void part_worker(int thread_id) {
    vector<int> curr_loadOrder=generate_order(load_order);
    cout <<"curr_loadOrder is: " <<curr_loadOrder<<endl;

}

void product_worker(int thread_id) {


}



int main(){
    part_worker(1);
//    vector<int> try_generate_load_order=generate_order(load_order);
//    cout<<"try generate load order: " <<try_generate_load_order<<endl;
//    cout<< is_initial_state(Global_buffer)<<endl;
//    srand((unsigned)time(NULL));
//    int someRandnumber=rand()%5;
    //int someR=rand()%5;

//    cout << someRandnumber<<endl;
    cout << "curr_Global_buffer is: "<<Global_buffer<<endl;
    vector<int> a1{0,1,1};
    vector<int> a2{2,2,2};
    vector<int> adding_res=a1+a2;
    cout << adding_res<<endl;
    vector<int> minus_res=a1-a2;
    cout<<minus_res<<endl;
    //cout<<"adding is: "<<res<<endl;
//    const int m = 20, n = 16; //m: number of Part Workers
//n: number of Product Workers
//m>n
//    thread partW[m];
//    thread prodW[n];
//    for (int i = 0; i < n; i++) {
//        partW[i] = thread(part_worker, i);
//        prodW[i] = thread (product_worker, i);
//    }
//    for (int i = n; i < m; i++) {
//        partW[i] = thread(part_worker, i);
//    }
//    /* Join the threads to the main threads */
//    for (int i = 0; i < n; i++) {
//        partW[i].join();
//        prodW[i].join();
//    }
//    for (int i = n; i < m; i++) {
//        partW[i].join();
//    }
//    cout << "Finish!" << endl;
    return 0;

}