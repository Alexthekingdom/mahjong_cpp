//
//  main.cpp
//  mahjong test
//
//  Created by LiShuCheng on 2020/4/25.adsfadsf
//  Copyright © 2020 LiShuCheng. All rights reserved.
//

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>


using namespace std;

vector<string> request, response;
vector<string> hand;
vector<string> shun_zi, ke_zi, gang_zi;

int shu[28] = { 0 }, feng[5] = { 0 }, jian[4] = { 0 };//数牌按饼条万的1-9顺序排列，数组数字代表该牌的数量
int shu_ting[28][6] = { 0 }, feng_ting[5] = { 0 }, jian_ting[4] = { 0};
//数牌的6个数分别表示是否听该牌，是否能杠，是否能碰，是否左两位吃，是否右两位吃，是否左右吃


void canmingpai() {
    int l, r;
    //计算数牌的鸣牌情况
    for (int i = 0; i < 3; ++i) {
        for (int j = 1; j <= 9; ++j) {
            //是否能碰
            if (shu[i * 9 + j] > 1) {
                //是否能杠
                if (shu[i * 9 + j] > 2) {
                    shu_ting[i * 9 + j][1] = 1;
                }
                else {
                    shu_ting[i * 9 + j][2] = 1;
                }
                shu_ting[i * 9 + j][0] = 1;
            }

            //是否左两位吃
            l = j - 2;
            r = j - 1;
            if (l > 0 && r > 0) {
                if (shu[i * 9 + l] > 0 && shu[i * 9 + r] > 0) {
                    shu_ting[i * 9 + j][3] = 1;
                    shu_ting[i * 9 + j][0] = 1;
                }
            }

            //是否右两位吃
            l = j + 1;
            r = j + 2;
            if (l < 10 && r < 10) {
                if (shu[i * 9 + l] > 0 && shu[i * 9 + r] > 0) {
                    shu_ting[i * 9 + j][4] = 1;
                    shu_ting[i * 9 + j][0] = 1;
                }
            }

            //是否左右吃
            l = j - 1;
            r = j + 1;
            if (l > 0 && r < 10) {
                if (shu[i * 9 + l] > 0 && shu[i * 9 + r] > 0) {
                    shu_ting[i * 9 + j][5] = 1;
                    shu_ting[i * 9 + j][0] = 1;
                }
            }
        }
    }

    //计算风牌和箭牌的鸣牌情况
    for (int i = 1; i <= 4; ++i) {
        if (feng[i] > 1) {
            feng_ting[i] = 1;
        }
    }
    for (int i = 1; i <= 3; ++i) {
        if (jian[i] > 1) {
            jian_ting[i] = 1;
        }
    }
}

int main()
{
    int turnID;
    string stmp;

    cin >> turnID;
    turnID--;
    getline(cin, stmp);
    for(int i = 0; i < turnID; i++) {
        getline(cin, stmp);
        request.push_back(stmp);
        getline(cin, stmp);
        response.push_back(stmp);
    }
    getline(cin, stmp);
    request.push_back(stmp);

    if(turnID < 2) { // round 1
        response.push_back("PASS");
    }
    else {
        int itmp, myPlayerID, quan;
        ostringstream sout;
        istringstream sin;
        sin.str(request[0]);
        sin >> itmp >> myPlayerID >> quan;
        sin.clear();
        
        // round 2 初始摸牌，花牌还没考虑进去
        sin.str(request[1]);
        for (int j = 0; j < 5; j++){
            sin >> itmp;
        }
        for (int j = 0; j < 13; j++) {
            sin >> stmp;
            hand.push_back(stmp);
        }
        
        // 之后的每轮，需要根据request判断自己手牌变化情况
        for(int i = 2; i < turnID; i++) {
            sin.clear();
            sin.str(request[i]);
            sin >> itmp;
            if(itmp == 2) {
                sin >> stmp;
                hand.push_back(stmp);
                sin.clear();
            }
            else if (itmp == 3) {
                int now_id;
                sin >> now_id; //当前轮玩家id
                if (now_id!=myPlayerID) {
                    continue; // 如果当前轮id不是我，那不会影响我的手牌，故直接过
                }
                // 否则影响手牌，看下一步
                string op;
                sin >> op; //当前轮操作
                if (op == "PLAY") {
                    sin >> stmp;
                    hand.erase(find(hand.begin(), hand.end(), stmp)); //如果我play了某牌，就从手里删掉
                }
                else if (op == "PENG"){
                    sin >> stmp;
                    hand.erase(find(hand.begin(), hand.end(), stmp));
                    sin.clear();
                    //以下对PENG进行手牌变更
                    sin.str(request[i-1]);
                    sin >> itmp >> itmp >> stmp;
                    if (stmp!="CHI") {
                        sin >> stmp >> stmp;
                    }
                    else{
                        sin >> stmp;
                    }
                    ke_zi.push_back(stmp);
                    // PENG有两张，都从手里删掉
                    hand.erase(find(hand.begin(), hand.end(), stmp));
                    hand.erase(find(hand.begin(), hand.end(), stmp));
                }
                else if (op == "CHI"){
                    sin >> stmp;
                    shun_zi.push_back(stmp);
                    sin >> stmp;
                    hand.erase(find(hand.begin(), hand.end(), stmp));
                    sin.clear();
                }
                else if (op == "GANG"){
                    sin.clear();
                    //以下对GANG进行手牌变更
                    sin.str(request[i-1]);
                    sin >> itmp;
                    if (itmp == 2) { // 如果上回合摸牌，则为暗杠
                        sin >> stmp;
                    }
                    else { // 否则为明杠
                        sin >> itmp >> stmp;
                        if (stmp!="CHI") {
                            sin >> stmp >> stmp;
                        }
                        else{
                            sin >> stmp;
                        }
                    }
                    gang_zi.push_back(stmp);
                    // GANG有3张，都从手里删掉
                    hand.erase(find(hand.begin(), hand.end(), stmp));
                    hand.erase(find(hand.begin(), hand.end(), stmp));
                    hand.erase(find(hand.begin(), hand.end(), stmp));
                }
            }
        }// 结束更新手牌
        
        // 当前轮
        // 先判断手上的牌能鸣的牌
        string s;
        int num;
        for (int i = 0; i < hand.size(); i++) {
            s = hand[i];
            num = s[1] - '0';
            if (s[0] == 'S') {
                break;//用于终止输入
            }
            switch (s[0]) {
            case 'B':
                ++shu[num];
                break;
            case 'T':
                ++shu[num + 9];
                break;
            case 'W':
                ++shu[num + 18];
                break;
            case 'F':
                ++feng[num];
                break;
            case 'J':
                ++jian[num];
                break;
            }
        }
        canmingpai();
        
        // 进行当前轮操作
        sin.clear();
        sin.str(request[turnID]);
        sin >> itmp;
        
        bool will_pass = 1; // 本轮操作是否输出pass
        
        if(itmp == 2) { // 如果当前轮是自己摸牌
            random_shuffle(hand.begin(), hand.end());
            sout << "PLAY " << *hand.rbegin();
            hand.pop_back();
            will_pass = 0;
        }
        else if (itmp == 3){ //如果当前轮是别人打了某牌
            sin >> itmp;
            if (itmp != myPlayerID) {
                sin >> stmp;
                if (stmp=="PLAY" || stmp=="PENG") {
                    sin >> stmp;
                    
                }
                else if (stmp == "CHI"){
                    sin >> stmp >> stmp;
                    
                }
            }
        }
        
        if (will_pass) {
            sout << "PASS";
        }
        response.push_back(sout.str());
    }

    cout << response[turnID] << endl;
    return 0;
}
