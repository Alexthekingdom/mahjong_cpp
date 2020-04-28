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
int shu_remain[28] = { 0 }, feng_remain[5] = { 0 }, jian_remain[4] = { 0 };//数牌按饼条万的1-9顺序排列，数组数字代表该牌的数量
int shu_quan[28][6] = { 0 }, feng_quan[5] = { 0 }, jian_quan[4] = { 0 };//数牌按饼条万的1-9顺序排列，数组数字代表该牌的鸣牌时的权重
//数牌的6个数分别表示总权重，碰或杠的权重，连续吃在左权重，连续吃在右权重，分开吃在左权重，分开吃在右权重
//eg.如手牌同时有2345万，4万连续吃在左权重即为45万吃的权重，为3万和6万剩余牌数量和，连续吃在右权重即34万吃，分开吃在右权重即24万吃
int shu_quan_[28] = { 0 }, feng_quan_[5] = { 0 }, jian_quan_[4] = { 0 };//数牌按饼条万的1-9顺序排列，数组数字代表该牌本身的权重，仅用于散牌判断出牌顺序
int shu_ting[28][6] = { 0 }, feng_ting[5][2] = { 0 }, jian_ting[4][2] = { 0 };
//数牌的6个数分别表示是否听该牌，是否能碰，是否能杠，是否左两位吃，是否右两位吃，是否左右吃；风和箭的两个数分别为是否能碰，是否能杠

void paiquanzhong() {
    int sanpai = 10, weizhi = 1, type = 0;//权重值的数值，方便之后增加调整权重的内容，type是采用的鸣牌权重类型，此处是鸣牌总权重
    for (int i = 0; i < 3; ++i) {
        for (int j = 1; j <= 9; ++j) {
            shu_quan_[i * 9 + j] += shu_quan[i * 9 + j][type] * sanpai;
            if (j == 1 || j == 9) {
                shu_quan_[i * 9 + j] += 1;
            }
            else if (j == 2 || j == 8) {
                shu_quan_[i * 9 + j] += 2;
            }
            else {
                shu_quan_[i * 9 + j] += 3;
            }
        }
    }
    for (int i = 1; i <= 4; ++i) {
        feng_quan_[i] += feng_quan[i];
    }
    for (int i = 1; i <= 3; ++i) {
        jian_quan_[i] += jian_quan[i];
    }
}

void canmingpai() {
    int l, r;
    //计算数牌的鸣牌情况
    for (int i = 0; i < 3; ++i) {
        for (int j = 1; j <= 9; ++j) {
            //是否能碰
            if (shu[i * 9 + j] > 1) {
                shu_ting[i * 9 + j][1] = 1;
                //是否能杠
                if (shu[i * 9 + j] > 2) {
                    shu_ting[i * 9 + j][2] = 1;
                }
                shu_quan[i * 9 + j][0] += shu_remain[i * 9 + j];
                shu_quan[i * 9 + j][1] += shu_remain[i * 9 + j];
                shu_ting[i * 9 + j][0] = 1;
            }

            //是否左两位吃
            l = j - 2;
            r = j - 1;
            if (l > 0 && r > 0) {
                if (shu[i * 9 + l] > 0 && shu[i * 9 + r] > 0) {
                    shu_ting[i * 9 + j][3] = 1;
                    shu_ting[i * 9 + j][0] = 1;
                    shu_quan[i * 9 + l][0] += shu_remain[i * 9 + j];
                    shu_quan[i * 9 + l][2] += shu_remain[i * 9 + j];
                    shu_quan[i * 9 + r][0] += shu_remain[i * 9 + j];
                    shu_quan[i * 9 + r][3] += shu_remain[i * 9 + j];
                }
            }

            //是否右两位吃
            l = j + 1;
            r = j + 2;
            if (l < 10 && r < 10) {
                if (shu[i * 9 + l] > 0 && shu[i * 9 + r] > 0) {
                    shu_ting[i * 9 + j][4] = 1;
                    shu_ting[i * 9 + j][0] = 1;
                    shu_quan[i * 9 + l][0] += shu_remain[i * 9 + j];
                    shu_quan[i * 9 + l][2] += shu_remain[i * 9 + j];
                    shu_quan[i * 9 + r][0] += shu_remain[i * 9 + j];
                    shu_quan[i * 9 + r][3] += shu_remain[i * 9 + j];
                }
            }

            //是否左右吃
            l = j - 1;
            r = j + 1;
            if (l > 0 && r < 10) {
                if (shu[i * 9 + l] > 0 && shu[i * 9 + r] > 0) {
                    shu_ting[i * 9 + j][5] = 1;
                    shu_ting[i * 9 + j][0] = 1;
                    shu_quan[i * 9 + l][0] += shu_remain[i * 9 + j];
                    shu_quan[i * 9 + l][4] += shu_remain[i * 9 + j];
                    shu_quan[i * 9 + r][0] += shu_remain[i * 9 + j];
                    shu_quan[i * 9 + r][5] += shu_remain[i * 9 + j];
                }
            }
        }
    }

    //计算风牌和箭牌的鸣牌情况
    for (int i = 1; i <= 4; ++i) {
        if (feng[i] > 1) {
            feng_ting[i][0] = 1;
            if (feng[i] > 2) {
                 feng_ting[i][1] = 1;
            }
            feng_quan[i] += feng_remain[i];
            feng_quan[i] += feng_remain[i];
        }
    }
    for (int i = 1; i <= 3; ++i) {
        if (jian[i] > 1) {
            jian_ting[i][0] = 1;
            if (jian[i] > 2) {
                jian_ting[i][1] = 1;
            }
            jian_quan[i] += jian_remain[i];
            jian_quan[i] += jian_remain[i];
        }
    }
}

int shu_location(string s) { //返回数牌的位置
    int num = s[1] - '0';
    switch (s[0]) {
    case 'B':
        return num;
    case 'T':
        return num + 9;
    case 'W':
        return num + 18;
    default:
        return -1;
    }
}

void del_remain(string stmp, int n) {
    int num = stmp[1] - '0';
    switch (stmp[0]) {
    case 'F':
        feng_remain[num] -= n;
        break;
    case 'J':
        jian_remain[num] -= n;
        break;
    default:
        shu_remain[shu_location(stmp)] -= n;
    }
}

int main()
{
    for (int i = 1; i < 28; i++) {
        shu_remain[i] = 4;
    }
    for (int i = 1; i < 5; i++) {
        feng_remain[i] = 4;
    }
    for (int i = 1; i < 4; i++) {
        jian_remain[i] = 4;
    }


    int turnID;
    string stmp;

    cin >> turnID;
    turnID--;
    getline(cin, stmp);
    for (int i = 0; i < turnID; i++) {
        getline(cin, stmp);
        request.push_back(stmp);
        getline(cin, stmp);
        response.push_back(stmp);
    }
    getline(cin, stmp);
    request.push_back(stmp);

    if (turnID < 2) { // round 1
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
        for (int j = 0; j < 5; j++) {
            sin >> itmp;
        }
        for (int j = 0; j < 13; j++) {
            sin >> stmp;
            del_remain(stmp, 1);
            hand.push_back(stmp);
        }

        // 之后的每轮，需要根据request判断自己手牌变化情况，以及当前场面剩余牌的形势
        for (int i = 2; i < turnID; i++) {
            sin.clear();
            sin.str(request[i]);
            sin >> itmp;
            if (itmp == 2) { //如果是自己摸牌
                sin >> stmp;
                hand.push_back(stmp);
                del_remain(stmp, 1);
                sin.clear();
            }
            else if (itmp == 3) { // 如果是其他情况
                int now_id;
                sin >> now_id; //当前轮玩家id
                string op;
                sin >> op; //当前轮操作

                if (op == "PLAY") {
                    sin >> stmp;
                    if (now_id == myPlayerID) {
                        hand.erase(find(hand.begin(), hand.end(), stmp)); //如果我play了某牌，就从手里删掉
                    }
                    //别人打出的牌，从剩余牌库里删掉
                    else {
                        del_remain(stmp, 1);
                    }
                }
                else if (op == "PENG") {
                    sin >> stmp;
                    if (now_id == myPlayerID) { // 先清除打出去的牌
                        hand.erase(find(hand.begin(), hand.end(), stmp));
                    }
                    else {
                        del_remain(stmp, 1);
                    }
                    sin.clear();

                    sin.str(request[i - 1]);
                    sin >> itmp >> itmp >> stmp;
                    if (stmp != "CHI") {
                        sin >> stmp >> stmp;
                    }
                    else {
                        sin >> stmp;
                    }

                    //以下对PENG进行手牌变更
                    if (now_id == myPlayerID) {
                        ke_zi.push_back(stmp);
                        // PENG有两张，都从手里删掉
                        hand.erase(find(hand.begin(), hand.end(), stmp));
                        hand.erase(find(hand.begin(), hand.end(), stmp));
                    }
                    //是其他人碰的牌，从剩余牌库删掉其亮出来的两张牌
                    else {
                        del_remain(stmp, 2);
                    }

                }
                else if (op == "CHI") {
                    sin.clear();
                    sin.str(request[i - 1]);
                    sin >> itmp >> itmp >> stmp;
                    if (stmp != "CHI") {
                        sin >> stmp >> stmp;
                    }
                    else {
                        sin >> stmp;
                    }

                    if (now_id == myPlayerID) { //先加上吃的是哪张牌，后续再删掉或打出
                        hand.push_back(stmp);
                    }
                    else {
                        del_remain(stmp, -1);
                    }

                    sin.clear();
                    sin.str(request[i]);
                    sin >> itmp >> itmp >> stmp;
                    if (now_id == myPlayerID) {
                        sin >> stmp;
                        shun_zi.push_back(stmp);

                        string tmp = stmp; // 清理掉吃的三张牌
                        hand.erase(find(hand.begin(), hand.end(), stmp));
                        tmp[1] = ((stmp[1] - '0') + 1) + '0';
                        hand.erase(find(hand.begin(), hand.end(), tmp));
                        tmp[1] = ((stmp[1] - '0') - 1) + '0';
                        hand.erase(find(hand.begin(), hand.end(), tmp));

                        sin >> stmp; // 打出另一张牌
                        hand.erase(find(hand.begin(), hand.end(), stmp));
                    }
                    else {
                        sin >> stmp;
                        del_remain(stmp, 1);
                        string tmp = stmp;
                        tmp[1] = ((stmp[1] - '0') + 1) + '0';
                        del_remain(tmp, 1);
                        tmp[1] = ((stmp[1] - '0') - 1) + '0';
                        del_remain(tmp, 1);

                        sin >> stmp; // 打出另一张牌
                        del_remain(stmp, 1);
                    }
                }
                else if (op == "GANG") {
                    if (now_id == myPlayerID) {
                        sin.clear();
                        //以下对GANG进行手牌变更
                        sin.str(request[i - 1]);
                        sin >> itmp;
                        if (itmp == 2) { // 如果上回合摸牌，则为暗杠
                            sin >> stmp;
                        }
                        else { // 否则为明杠
                            sin >> itmp >> stmp;
                            if (stmp != "CHI") {
                                sin >> stmp >> stmp;
                            }
                            else {
                                sin >> stmp;
                            }
                        }
                        gang_zi.push_back(stmp);
                        // GANG有3张，都从手里删掉
                        hand.erase(find(hand.begin(), hand.end(), stmp));
                        hand.erase(find(hand.begin(), hand.end(), stmp));
                        hand.erase(find(hand.begin(), hand.end(), stmp));
                    }
                    else {
                        sin.clear();
                        sin.str(request[i - 1]);
                        sin >> itmp >> itmp >> stmp;
                        if (stmp != "DRAW") { //如果是DRAW，那我也不知道剩余牌库是否发生了变化
                            if (stmp != "CHI") {
                                sin >> stmp >> stmp;
                            }
                            else {
                                sin >> stmp;
                            }
                            del_remain(stmp, 3); //杠会亮出来三张，所以牌库减3
                        }
                    }
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

        if (itmp == 2) { // 如果当前轮是自己摸牌
            random_shuffle(hand.begin(), hand.end());
            sout << "PLAY " << *hand.rbegin();
            hand.pop_back();
            will_pass = 0;
        }
        else if (itmp == 3) { //如果当前轮是别人打了某牌
            sin >> itmp;
            if (itmp != myPlayerID) {
                sin >> stmp;
                if (stmp == "PLAY" || stmp == "PENG") {
                    sin >> stmp;

                }
                else if (stmp == "CHI") {
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
