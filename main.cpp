#include<iostream>
#include<vector>
#include<thread>
#include<mutex>
#include<stdlib.h>
#include<condition_variable>
#include <time.h>

using namespace std;
vector<int> load_order(5);
vector<int> pickup_order(5);//一定是两个或者三个位置有数字，记得区别
vector<int> Global_buffer(5);
const vector<int> restraints{5,5,4,3,3};
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
    vector<int> res(order.size());
    for(int i=0;i<order.size();i++){
        res[i]=order[i]+buffer_state[i];
    }
    return res;
}
vector<int> operator-(const vector<int>& buffer_state, const vector<int>&order){
    vector<int> res(order.size());
    for(int i=0;i<order.size();i++){
        res[i]=buffer_state[i]-order[i];
    }
    return res;
}


int getSum(const vector<int>& vec){
    int sum=0;
    for(auto const& i:vec){
        sum+=i;
    }
    return sum;
}

vector<int> generate_load_order(vector<int>& order) {
    srand((unsigned) time(NULL));
    int curr_loadOrder_sum=getSum(order);
    for (int i = curr_loadOrder_sum; i < 5; i++) {
        int pos_to_add = rand() % 5;
        ++order[pos_to_add];
    }
    return order;
}
int hasNumber_onSpot(const vector<int>& order){
    int hasNumber{0};
    for(const auto&i:order){
        if(i!=0){
            hasNumber++;
        }
    }
    return hasNumber;
}
vector<int> generate_pickup_order(vector<int>& order){
    srand((unsigned) time(NULL));
    vector<int> pos_hasNumber;
    int curr_pickupOrder_sum=getSum(order);
    for(int i=curr_pickupOrder_sum;i<5;i++){
        if(hasNumber_onSpot(order)==3){
            break;
        }
        ++order[rand()%5];
    }
    if(getSum(order)!=5){
        for(int i=0;i<5;i++){
            if(order[i]!=0){
                pos_hasNumber.push_back(i);
            }
        }
    }
    //cout<< "position has num: "<<pos_hasNumber<<"size is: "<<pos_hasNumber.size()<<endl;
    while(getSum(order)!=5){
        //++order[pos_hasNumber[rand()%pos_hasNumber.size()]];
		int pos_to_add = rand()%5;
		for(auto const&i:pos_hasNumber){
			if(pos_to_add==i){
				++order[pos_to_add];
			}
		}
    }
    return order;
}
bool is_overFlow(const vector<int>& order, const vector<int>& buffer_state){
    vector<int> res=order+buffer_state;
    for(int i=0;i<order.size();i++){
        if(res[i]>restraints[i]){
            return true;
        }
    }
    return false;
}
bool is_notEnough(const vector<int>& buffer_state, const vector<int>& order){
    vector<int> res=buffer_state-order;
    for(int i=0;i<order.size();i++){
        if(res[i]<0){
            return true;
        }
    }
    return false;
}

void part_worker(int thread_id) {
    vector<int> curr_loadOrder=generate_load_order(load_order);
	vector<int> tmp;
    //cout <<"curr_loadOrder is: " <<curr_loadOrder<<endl;
    if(is_overFlow(Global_buffer,curr_loadOrder)==false){//no overflow, direct merge two
		Global_buffer = Global_buffer+curr_loadOrder;
		curr_loadOrder={0,0,0,0,0};
    }
    else{//overflow happens
		//cout << "global buffer: " << Global_buffer << endl;
		tmp = curr_loadOrder+Global_buffer;
		//cout << "tmp is: " << tmp << endl;
		for (int i = 0; i<5;i++) {
			if(tmp[i]>restraints[i]){
				Global_buffer[i] = restraints[i];
				curr_loadOrder[i] = tmp[i]-restraints[i];
			}
			else{
				Global_buffer[i] = tmp[i];
				curr_loadOrder[i] = 0;
			}
		}

		//cout << "tmp is: " << tmp << endl;
    }
	load_order = curr_loadOrder;



}

void product_worker(int thread_id) {
    vector<int> curr_pickupOrder=generate_pickup_order(pickup_order);
    cout<<"curr_pickupOrder is: "<<curr_pickupOrder<<endl;
	vector<int> tmp;
	if(is_notEnough(Global_buffer,curr_pickupOrder)==false){
		Global_buffer = Global_buffer-curr_pickupOrder;
		curr_pickupOrder={0,0,0,0,0};
	}
	else{//some pickup order cannot be fulfilled
		tmp = Global_buffer-curr_pickupOrder;
		cout << "tmp is: " << tmp << endl;
		for (int i = 0; i<5;i++) {
			if(tmp[i]<0){
				Global_buffer[i] = 0;
				curr_pickupOrder[i] = (-tmp[i]);
			}
			else{
				Global_buffer[i] = tmp[i];
				curr_pickupOrder[i] = 0;
			}
		}
	}
	pickup_order = curr_pickupOrder;

}



int main(){
	cout << "curr_loadorder: " << load_order << endl;
	cout <<"Global: " <<Global_buffer << endl;
	cout << "curr_pickup: " << pickup_order << endl<<endl;
    part_worker(1);
	cout << "curr_loadorder: " << load_order << endl;
	cout <<"Global: " <<Global_buffer << endl;
	cout << "curr_pickup: " << pickup_order << endl<<endl;
    product_worker(2);
	cout << "curr_loadorder: " << load_order << endl;
	cout <<"Global: " <<Global_buffer << endl;
	cout << "curr_pickup: " << pickup_order << endl<<endl;
//    vector<int> try_generate_load_order=generate_order(load_order);
//    cout<<"try generate load order: " <<try_generate_load_order<<endl;
//    cout<< is_initial_state(Global_buffer)<<endl;
//    srand((unsigned)time(NULL));
//    int someRandnumber=rand()%5;
    //int someR=rand()%5;

//    cout << someRandnumber<<endl;
//    cout << "curr_Global_buffer is: "<<Global_buffer<<endl;
//    vector<int> a1{0,1,1};
//    vector<int> a2{2,2,2};
//    vector<int> adding_res=a1+a2;
//    cout << adding_res<<endl;
//    vector<int> minus_res=a1-a2;
//    cout<<minus_res<<endl;
   // vector<int> try_gen_pickup_order=generate_pickup_order(vector<int> pickup_order);
    //cout<<"adding is: "<<res<<endl;
    const int m = 20, n = 16; //m: number of Part Workers
//n: number of Product Workers
//m>n
    thread partW[m];
    thread prodW[n];
    for (int i = 0; i < n; i++) {
        partW[i] = thread(part_worker, i);
        prodW[i] = thread (product_worker, i);
    }
    for (int i = n; i < m; i++) {
        partW[i] = thread(part_worker, i);
    }
    /* Join the threads to the main threads */
    for (int i = 0; i < n; i++) {
        partW[i].join();
        prodW[i].join();
    }
    for (int i = n; i < m; i++) {
        partW[i].join();
    }
    cout << "Finish!" << endl;
    return 0;

}