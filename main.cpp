#include<iostream>
#include<vector>
#include<thread>
#include<mutex>
#include<stdlib.h>
#include<condition_variable>
#include <time.h>
#include<string>

using namespace std;
vector<int> load_order(5);
vector<int> pickup_order(5);//一定是两个或者三个位置有数字，记得区别
vector<int> Global_buffer(5);
const vector<int> restraints{5,5,4,3,3};
const vector<int> partW_manufacture_time{50, 50, 60, 60, 70};
const vector<int> move_time{20, 20, 30, 30,40};
const vector<int> prodW_assemble_time{60, 60, 70, 70, 80};
const vector<string> status_string{"New Load Order","New Pickup Order",
								   "Wakeup-Notified","Wakeup-Timeout"};
mutex m1;
condition_variable partW, productW;
bool is_initial_state(const vector<int>&vec){//check whether if it's 0,0,0,0,0
    for(auto const& item:vec){
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
bool has_left(const vector<int>&vec){
	for(auto const&i:vec){
		if(i!=0){
			return true;
		}
	}
	return false;
}
bool is_theSame(const vector<int>& vec1, const vector<int>& vec2)
{
	if (vec1.size()!=vec2.size()) {
		return false;
	}
	for (int i = 0; i<vec1.size(); i++) {
		if (vec1[i]!=vec2[i]) {
			return false;
		}
	}
	return true;
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
	unique_lock<mutex> u1(m1);
	cout << "part_worker: " << thread_id << " running" << endl;

	vector<int> save_loadOrder = load_order;
	//cout << "is theSame? loadOrder: " << save_loadOrder << endl;
    vector<int> curr_loadOrder=generate_load_order(load_order);

	//cout << "is theSame? curr_loadOrder: " << curr_loadOrder << endl;
	for (int i = 0; i<5;i++) {
		//manufacture parts?
		if(curr_loadOrder[i]!=save_loadOrder[i]){
			int sleep_time = (curr_loadOrder[i]-save_loadOrder[i])*partW_manufacture_time[i];
			//cout << sleep_time << endl;
			this_thread::sleep_for(chrono::microseconds(sleep_time));
		}
	}

	vector<int> tmp;
	cout << "Load Order: " << load_order << endl;
	cout << "Buffer State: " << Global_buffer << endl;
    if(is_overFlow(Global_buffer,curr_loadOrder)==false){//no overflow, direct merge two
		Global_buffer = Global_buffer+curr_loadOrder;
		for (int i = 0; i<5;i++) {//calculate move_time
			if(curr_loadOrder[i]!=0){
				int sleep_time = curr_loadOrder[i]*move_time[i];
				this_thread::sleep_for(chrono::microseconds(sleep_time));
			}
		}
		curr_loadOrder={0,0,0,0,0};
    }
    else{//overflow happens
		tmp = curr_loadOrder+Global_buffer;
		for (int i = 0; i<5;i++) {
			if(tmp[i]>restraints[i]){
				int sleep_time = (restraints[i]-Global_buffer[i])*move_time[i];
				this_thread::sleep_for(chrono::microseconds(sleep_time));
				Global_buffer[i] = restraints[i];
				curr_loadOrder[i] = tmp[i]-restraints[i];
			}
			else{
				int sleep_time = (tmp[i]-Global_buffer[i])*move_time[i];
				this_thread::sleep_for(chrono::microseconds(sleep_time));
				Global_buffer[i] = tmp[i];
				curr_loadOrder[i] = 0;
			}
		}
    }
    //this place should notify another thread to
	load_order = curr_loadOrder;

	cout << "Updated Buffer State: " << Global_buffer << endl;
	cout << "Update Load Order: " << load_order << endl;
	cout << endl;
	while(has_left(load_order)){
		partW.wait(u1);
	}
	productW.notify_one();


}

void product_worker(int thread_id) {
	unique_lock<mutex> u1(m1);
	cout << "product_worker: " << thread_id << " running" << endl;
	//cout << "is theSame?: " << pickup_order << endl;
	vector<int> save_pickupOrder = pickup_order;
    vector<int> curr_pickupOrder=generate_pickup_order(pickup_order);
	//cout << "is theSame?: " << curr_pickupOrder << endl;
//	for (int i = 0; i<5;i++) {
//		//calculate assemble time
//		if(save_pickupOrder[i]!=curr_pickupOrder[i]){
//			int sleep_time = (curr_pickupOrder[i]-save_pickupOrder[i])*prodW_assemble_time[i];
//			cout << sleep_time << endl;
//			this_thread::sleep_for(std::chrono::microseconds (sleep_time));
//		}
//	}
    cout<<"before curr_pickupOrder is: "<<curr_pickupOrder<<endl;
	cout << "before global_buffer is: " << Global_buffer << endl;
	vector<int> tmp;
	if(is_notEnough(Global_buffer,curr_pickupOrder)==false){
		//ideal situation
		//...
		//...
		Global_buffer = Global_buffer-curr_pickupOrder;
		curr_pickupOrder={0,0,0,0,0};
		for (int i = 0; i<5;i++) {
			if(curr_pickupOrder[i]!=0) {
				//move also manufacture
				this_thread::sleep_for(chrono::microseconds(move_time[i]+prodW_assemble_time[i]));
			}
		}
	}
	else{//some pickup order cannot be fulfilled
		tmp = Global_buffer-curr_pickupOrder;
		//cout << "tmp is: " << tmp << endl;
		for (int i = 0; i<5;i++) {
			if(tmp[i]<0){
				int sleep_time = (Global_buffer[i])*(move_time[i]+prodW_assemble_time[i]);
				this_thread::sleep_for(chrono::microseconds(sleep_time));
				Global_buffer[i] = 0;
				curr_pickupOrder[i] = (-tmp[i]);
			}
			else{
				int sleep_time = curr_pickupOrder[i]*(move_time[i]+prodW_assemble_time[i]);
				this_thread::sleep_for(chrono::microseconds(sleep_time));
				Global_buffer[i] = tmp[i];
				curr_pickupOrder[i] = 0;
			}
		}
	}
	pickup_order = curr_pickupOrder;

	cout<<"Updated pickup order: "<<pickup_order<<endl;
	cout << "Updated Buffer State: " << Global_buffer << endl<<endl;
	while(has_left(pickup_order)){
		productW.wait(u1);
	}
	partW.notify_one();

}



int main(){

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