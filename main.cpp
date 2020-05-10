#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "Mahjong-GB-CPP/MahjongGB/MahjongGB.h" //用于Alex的本地调试
// #include "MahjongGB/MahjongGB.h" 用于线上
#include <utility>

using namespace std;

vector<string> request, response;
vector<string> hand;
vector<string> shun_zi, ke_zi, gang_zi;

int shu[28] = { 0 }, feng[5] = { 0 }, jian[4] = { 0 };//数牌按饼条万的1-9顺序排列，数组数字代表该牌的数量
int shu_remain[28] = { 0 }, feng_remain[5] = { 0 }, jian_remain[4] = { 0 };//数牌按饼条万的1-9顺序排列，数组数字代表该牌的数量
int shu_ting[28][6] = { 0 }, feng_ting[5] = { 0 }, jian_ting[4] = { 0 };
//数牌的6个数分别表示是否听该牌，是否能杠，是否能碰，是否左两位吃，是否右两位吃，是否左右吃


int shu_quan[28][6] = { 0 }, feng_quan[5] = { 0 }, jian_quan[4] = { 0 };//数牌按饼条万的1-9顺序排列，数组数字代表该牌的鸣牌时的权重
//数牌的6个数分别表示总权重，碰或杠的权重，连续吃在左权重，连续吃在右权重，分开吃在左权重，分开吃在右权重
//eg.如手牌同时有2345万，4万连续吃在左权重即为45万吃的权重，为3万和6万剩余牌数量和，连续吃在右权重即34万吃，分开吃在右权重即24万吃
int shu_quan_[28][2] = { 0 }, feng_quan_[5] = { 0 }, jian_quan_[4] = { 0 };//数牌按饼条万的1-9顺序排列，数组数字代表最终权重，其中数牌分别是鸣牌权重和牌使用一张的损失权重

int total_duizi = 0, duizi[6] = { 0 }, paizhong[6] = { 0 }, fanzhong[4] = { 0 };//对子数和各种牌数，顺序为饼条万风箭
//番种三个值分别代表番种类型，牌型，牌大小
//番种类型1为碰碰胡，2为五门齐，3为混一色，4为清龙，5为花龙，6为三色三同顺，7为三色三步高
//牌型用于混一色和清龙，012分别为饼条万，花龙和三色三步高，其中顺子按数分小中大，按饼条万顺序0为小中大，1为大小中，2为中大小，3为大中小，4为小大中，5为中小大

//牌大小用于三色三步高和三色三同顺，其值为三步高中间顺子的中间牌数字，三同顺代表顺子中间牌数字


int shunzi_paishu(int i, int j) {//用于计算一个顺子中已有牌数，i为牌种，j为数字，表示顺子中间的牌，返回该顺子中已有的牌数
    int n = 0;
    if (shu[i * 9 + j - 1] > 0) {
        ++n;
    }
    if (shu[i * 9 + j] > 0) {
        ++n;
    }
    if (shu[i * 9 + j + 1] > 0) {
        ++n;
    }
    return n;
}



void jiaquan(int i, int j) {//当数牌只有一张时，增加1000权重

    if (shu[i * 9 + j] == 1) {
        shu_quan_[i * 9 + j][0] += 1000;
        shu_quan_[i * 9 + j][1] += 1000;
    }
}


void jiaquan2(int i, int j) {//当数牌只有一张时，增加1000权重
    if (shu[i * 9 + j] == 1) {
        shu_quan_[i * 9 + j][0] += 100;
        shu_quan_[i * 9 + j][1] += 100;
    }
}



void dingfan() {
    int yiyou;
    //计算对子数和各花色牌数
    for (int i = 0; i < 3; ++i) {
        for (int j = 1; j <= 9; ++j) {
            paizhong[i] += shu[i * 9 + j];
            if (shu[i * 9 + j] >= 2) {
                ++total_duizi;
                ++duizi[i];
            }
        }
    }
    for (int i = 1; i <= 4; ++i) {
        paizhong[3] += feng[i];
        if (feng[i] >= 2) {
            ++total_duizi;
            ++duizi[3];
        }
    }
    for (int i = 1; i <= 3; ++i) {
        paizhong[4] += jian[i];
        if (jian[i] >= 2) {
            ++total_duizi;
            ++duizi[4];
        }
    }

    //判断碰碰胡，三个以上对子
    if (total_duizi >= 4) {

        fanzhong[0] = 1;
        for (int i = 0; i < 3; ++i) {
            for (int j = 1; j <= 9; ++j) {
                if (shu[i * 9 + j] >= 2) {
                    shu_quan_[i * 9 + j][0] += 1000;
                    shu_quan_[i * 9 + j][1] += 1000;
                }
            }
        }
        for (int i = 1; i <= 4; ++i) {
            if (feng[i] >= 2) {
                feng_quan_[i] += 1000;
            }
        }
        for (int i = 1; i <= 3; ++i) {
            if (jian[i] >= 2) {
                jian_quan_[i] += 1000;
            }
        }
        return;
    }

    if (total_duizi == 3) {
        for (int i = 0; i < 3; ++i) {
            for (int j = 1; j <= 9; ++j) {
                if (shu[i * 9 + j] >= 2) {
                    shu_quan_[i * 9 + j][0] += 100;
                    shu_quan_[i * 9 + j][1] += 100;
                }
            }
        }
        for (int i = 1; i <= 4; ++i) {
            if (feng[i] >= 2) {
                feng_quan_[i] += 100;
            }
        }
        for (int i = 1; i <= 3; ++i) {
            if (jian[i] >= 2) {
                jian_quan_[i] += 100;
            }
        }
    }

    //判断五门齐，风箭各一对,数牌至少有两种有一对或能吃
    yiyou = 0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 1; j <= 9; ++j) {
            if (shu[i * 9 + j] >= 2) {
                ++yiyou;
                break;
            }
            else if (j > 1 && j < 9 && shunzi_paishu(i, j) >= 2) {
                ++yiyou;
                break;
            }
        }
    }
    if (yiyou >= 2 && duizi[3] >= 1 && duizi[4] >= 1) {
        fanzhong[0] = 2;
        for (int i = 1; i <= 4; ++i) {
            if (feng[i] >= 2) {
                feng_quan_[i] += 1000;
                break;
            }
        }
        for (int i = 1; i <= 3; ++i) {
            if (jian[i] >= 2) {
                jian_quan_[i] += 1000;
                break;
            }
        }
        return;
    }

    //判断混一色，某一色数牌和字牌共10张以上
    for (int i = 0; i < 3; ++i) {
        if (paizhong[i] >= (10 - paizhong[3] - paizhong[4])) {

            fanzhong[0] = 3;
            fanzhong[1] = i;
            for (int j = 1; j <= 9; ++j) {
                shu_quan_[i * 9 + j][0] += 1000;
                shu_quan_[i * 9 + j][1] += 1000;
            }
            for (int i = 1; i <= 4; ++i) {
                if (feng[i] >= 2) {
                    feng_quan_[i] += 1000;
                }
                else {
                    feng_quan_[i] += 500;
                }
            }
            for (int i = 1; i <= 3; ++i) {
                if (jian[i] >= 2) {
                    jian_quan_[i] += 1000;
                }
                else {
                    jian_quan_[i] += 500;
                }
            }
            return;
        }
    }

    //判断清龙，9张需有7张
    int qinglong = 7;
    for (int i = 0; i < 3; ++i) {
        yiyou = 0;
        yiyou += shunzi_paishu(i, 2);
        yiyou += shunzi_paishu(i, 5);
        yiyou += shunzi_paishu(i, 8);
        if (yiyou >= qinglong) {
            fanzhong[0] = 4;
            fanzhong[1] = i;
            for (int j = 1; j <= 9; ++j) {
                jiaquan(i, j);
            }
            return;
        }

        if (yiyou == 6) {
            for (int j = 1; j <= 9; ++j) {
                jiaquan2(i, j);
            }
        }

    }
    //判断花龙，9张需有7张
    int hualong = 7;
    for (int i = 0; i < 3; ++i) {
        yiyou = 0;
        yiyou += shunzi_paishu(i, 2);
        yiyou += shunzi_paishu((i + 1) % 3, 5);
        yiyou += shunzi_paishu((i + 2) % 3, 8);
        if (yiyou >= hualong) {
            fanzhong[0] = 5;
            fanzhong[1] = i;
            for (int m = 1; m <= 3; ++m) {
                jiaquan(i, m);
                jiaquan((i + 1) % 3, m + 3);
                jiaquan((i + 2) % 3, m + 6);
            }
            return;
        }

        if (yiyou == 6) {
            for (int m = 1; m <= 3; ++m) {
                jiaquan2(i, m);
                jiaquan2((i + 1) % 3, m + 3);
                jiaquan2((i + 2) % 3, m + 6);
            }
        }

        yiyou = 0;
        yiyou += shunzi_paishu(i, 8);
        yiyou += shunzi_paishu((i + 1) % 3, 5);
        yiyou += shunzi_paishu((i + 2) % 3, 2);
        if (yiyou >= hualong) {
            fanzhong[0] = 5;
            fanzhong[1] = i + 3;
            for (int m = 1; m <= 3; ++m) {
                jiaquan(i, m + 6);
                jiaquan((i + 1) % 3, m + 3);
                jiaquan((i + 2) % 3, m);
            }
            return;
        }

        if (yiyou == 6) {
            for (int m = 1; m <= 3; ++m) {
                jiaquan2(i, m + 6);
                jiaquan2((i + 1) % 3, m + 3);
                jiaquan2((i + 2) % 3, m);
            }
        }

    }
    //判断三色三同顺，9张需有7张
    int santongshun = 7;
    for (int j = 2; j <= 8; ++j) {
        yiyou = 0;
        for (int i = 0; i < 3; ++i) {

            yiyou += shunzi_paishu(i, j);

        }
        if (yiyou >= santongshun) {
            fanzhong[0] = 6;
            fanzhong[2] = j;
            for (int i = 0; i < 3; ++i) {
                jiaquan(i, j - 1);
                jiaquan(i, j);
                jiaquan(i, j + 1);
            }
            return;
        }

        if (yiyou == 6) {
            for (int i = 0; i < 3; ++i) {
                jiaquan2(i, j - 1);
                jiaquan2(i, j);
                jiaquan2(i, j + 1);
            }
        }

    }
    //判断三色三步高，9张需有7张
    int sanbugao = 7;
    for (int j = 3; j <= 7; ++j) {
        for (int i = 0; i < 3; ++i) {
            yiyou = 0;
            yiyou += shunzi_paishu(i, j - 1);
            yiyou += shunzi_paishu((i + 1) % 3, j);
            yiyou += shunzi_paishu((i + 2) % 3, j + 1);
            if (yiyou >= sanbugao) {
                fanzhong[0] = 5;
                fanzhong[1] = i;
                fanzhong[2] = j;
                for (int m = j - 1; m <= j + 1; ++m) {
                    jiaquan(i, m - 1);
                    jiaquan((i + 1) % 3, m);
                    jiaquan((i + 2) % 3, m + 1);
                }
                return;
            }

            if (yiyou == 6) {
                for (int m = j - 1; m <= j + 1; ++m) {
                    jiaquan2(i, m - 1);
                    jiaquan2((i + 1) % 3, m);
                    jiaquan2((i + 2) % 3, m + 1);
                }
            }

        }
        for (int i = 0; i < 3; ++i) {
            yiyou = 0;
            yiyou += shunzi_paishu(i, j + 1);
            yiyou += shunzi_paishu((i + 1) % 3, j);
            yiyou += shunzi_paishu((i + 2) % 3, j - 1);
            if (yiyou >= sanbugao) {
                fanzhong[0] = 5;
                fanzhong[1] = i + 3;
                fanzhong[2] = j;
                for (int m = j - 1; m <= j + 1; ++m) {
                    jiaquan(i, m + 1);
                    jiaquan((i + 1) % 3, m);
                    jiaquan((i + 2) % 3, m - 1);
                }
                return;
            }

            if (yiyou == 6) {
                for (int m = j - 1; m <= j + 1; ++m) {
                    jiaquan2(i, m + 1);
                    jiaquan2((i + 1) % 3, m);
                    jiaquan2((i + 2) % 3, m - 1);
                }
            }

        }
    }
}

int maxquan(int n, int k) {//返回第k大的吃权重，k为1-4
    int num[5];
    for (int i = 0; i < 4; ++i) {
        num[i] = shu_quan[n][i + 2];
    }
    sort(num, num + 4);
    return num[4 - k];
}

void paiquanzhong() {
    int paishu, max_quan;
    for (int i = 0; i < 3; ++i) {

        for (int j = 1; j <= 9; ++j) {
            if (shu_quan[i * 9 + j][0] == 0 || shu[i * 9 + j] == 0) {//没牌或权重都为0，只考虑牌本身权重
                shu_quan_[i * 9 + j][0] = 0;
                if (j == 1 || j == 9) {
                    shu_quan_[i * 9 + j][0] += 1;
                    shu_quan_[i * 9 + j][1] += 1;
                }
                else if (j == 2 || j == 8) {
                    shu_quan_[i * 9 + j][0] += 2;
                    shu_quan_[i * 9 + j][1] += 2;
                }
                else {
                    shu_quan_[i * 9 + j][0] += 3;
                    shu_quan_[i * 9 + j][1] += 3;
                }
                continue;
            }
            if (shu[i * 9 + j] == 1) {//只有一张牌，只能吃一次，选权重最大的吃
                shu_quan_[i * 9 + j][0] = maxquan(i * 9 + j, 1) * 10;
                shu_quan_[i * 9 + j][1] = maxquan(i * 9 + j, 1) * 10;
            }
            else if (shu[i * 9 + j] == 2) {//两张牌，可碰或吃两次
                if (shu_quan[i * 9 + j][1] > (maxquan(i * 9 + j, 1) + maxquan(i * 9 + j, 2))) {//碰的权重大，鸣牌权重同碰，用牌损失碰-单个吃
                    shu_quan_[i * 9 + j][0] = shu_quan[i * 9 + j][1] * 10;
                    shu_quan_[i * 9 + j][1] = (shu_quan[i * 9 + j][1] - maxquan(i * 9 + j, 1)) * 10;
                }
                else if (shu_quan[i * 9 + j][1] <= (maxquan(i * 9 + j, 1) + maxquan(i * 9 + j, 2))) {//吃的权重大，鸣牌权重同吃，同时判断是单张吃还是两张都吃，二者权重相等按吃的权重大算
                    if (maxquan(i * 9 + j, 2) == 0) {//如果是单张吃,鸣牌权重同最大吃，用牌不损失权重
                        shu_quan_[i * 9 + j][0] = maxquan(i * 9 + j, 1) * 10;
                        shu_quan_[i * 9 + j][1] = 0;
                    }
                    else {//如果是两张吃，鸣牌权重为二者和，用牌损失第二高权重的吃
                        shu_quan_[i * 9 + j][0] = (maxquan(i * 9 + j, 1) + maxquan(i * 9 + j, 2)) * 10;
                        shu_quan_[i * 9 + j][1] = maxquan(i * 9 + j, 2);
                    }
                }
            }
            else if (shu[i * 9 + j] == 3) {//三张牌，可碰加吃或三张吃，其他情况的权重绝不会比这两种大，不讨论
                if (shu_quan[i * 9 + j][1] > (maxquan(i * 9 + j, 2) + maxquan(i * 9 + j, 3))) {//碰加吃的权重大，此处两边都有最大权重的吃，因此不计算
                    if (maxquan(i * 9 + j, 1) == 0) {//没有能吃的，鸣牌权重等于碰，且用牌不损失
                        shu_quan_[i * 9 + j][0] = (shu_quan[i * 9 + j][1] + maxquan(i * 9 + j, 1)) * 10;
                        shu_quan_[i * 9 + j][1] = 0;
                    }
                    else {//有能吃的，鸣牌权重等于碰加吃，用牌权重是鸣牌权重减去两个吃和碰里较大的
                        shu_quan_[i * 9 + j][0] = shu_quan[i * 9 + j][1] * 10;
                        if (shu_quan[i * 9 + j][1] > (maxquan(i * 9 + j, 1) + maxquan(i * 9 + j, 2))) {//碰的权重更大
                            shu_quan_[i * 9 + j][1] = shu_quan_[i * 9 + j][0] - shu_quan[i * 9 + j][1] * 10;
                        }
                        else {//两个吃权重更大
                            shu_quan_[i * 9 + j][1] = shu_quan_[i * 9 + j][0] - (maxquan(i * 9 + j, 1) + maxquan(i * 9 + j, 2)) * 10;
                        }
                    }
                }
                else {//三个吃的权重大，鸣牌权重为三者和，根据吃的实际数量算用牌权重
                    shu_quan_[i * 9 + j][0] = (maxquan(i * 9 + j, 1) + maxquan(i * 9 + j, 2) + maxquan(i * 9 + j, 3)) * 10;
                    if (maxquan(i * 9 + j, 3) == 0) {//如果只用两张或更少吃，用牌不损失权重
                        shu_quan_[i * 9 + j][1] = 0;
                    }
                    else {//如果三张吃，用牌损失第三大的吃权重
                        shu_quan_[i * 9 + j][1] = maxquan(i * 9 + j, 3) * 10;
                    }
                }
            }
            if (j == 1 || j == 9) {//加上牌本身的权重
                shu_quan_[i * 9 + j][0] += 1;
                shu_quan_[i * 9 + j][1] += 1;
            }
            else if (j == 2 || j == 8) {
                shu_quan_[i * 9 + j][0] += 2;
                shu_quan_[i * 9 + j][1] += 2;
            }
            else {
                shu_quan_[i * 9 + j][0] += 3;
                shu_quan_[i * 9 + j][1] += 3;
            }
        }
    }

    for (int i = 1; i <= 4; ++i) {

        feng_quan_[i] += feng_quan[i] * 10;

    }

    for (int i = 1; i <= 3; ++i) {

        jian_quan_[i] += jian_quan[i] * 10;

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

            feng_ting[i] = 1;
            if (feng[i] > 2) {
                feng_ting[i] = 1;

            }
            feng_quan[i] += feng_remain[i];
            feng_quan[i] += feng_remain[i];
        }
    }
    for (int i = 1; i <= 3; ++i) {
        if (jian[i] > 1) {

            jian_ting[i] = 1;
            if (jian[i] > 2) {
                jian_ting[i] = 1;

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

/*void HU() {
    if (hand.size == 2) {
        if (hand.front == hand.back)return 1;
        else return 0;
    }
    else
        return 0;
}
/*if (HU) {
    request.push_pack("HU")
}*/

string quanzhongzuixiao(string &a) {
    int shu_min = 1, feng_min = 1, jian_min = 1;//最小值下标
    int shu_temp = shu_quan[1][0], feng_temp = feng_quan[1], jian_temp = jian_quan[1];//最小值
    char kind, num;
    for (int i = 2; i < 28; i++) {
        if (shu_quan[i][0] < shu_temp) {//>=使得权重相同时，先打序号大的牌
            shu_temp = shu_quan[i][0];
            shu_min = i;
        }
    }
    for (int i = 2; i < 4; i++) {
        if (feng_quan[i] <= feng_temp) {
            feng_temp = feng_quan[i];
            feng_min = i;
        }
    }
    for (int i = 2; i < 3; i++) {
        if (jian_quan[i] <= jian_temp) {
            jian_temp = jian_quan[i];
            jian_min = i;
        }
    }
    if (shu_temp <= feng_temp) {//权重相同，优先出数牌
        if (shu_temp <= jian_temp) {
            if ((shu_min - 1) / 9 == 0) {
                kind = 'B';
            }
            else if ((shu_min - 1) / 9 == 1) {
                kind = 'T';
            }
            else
                kind = 'W';
            num = (char)shu_min;
        }
        else {
            kind = 'J';
            num = (char)jian_min;
        }
    }
    else if (feng_temp < jian_temp) {
        kind = 'F';
        num = (char)feng_min;
    }
    else {
        kind = 'J';
        num = (char)jian_min;
    }
    a = kind + num;
    return a;
}

int hua_num;

/*void HU() {
    if (hand.size == 2) {
        if (hand.front == hand.back)return 1;
        else return 0;
    }
    else
        return 0;
}
/*if (HU) {
    request.push_pack("HU")
}*/

int hua_num;

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
    hua_num = 0;

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
    } //保存之前的信息，之后做处理

    getline(cin, stmp);
    request.push_back(stmp);

    if (turnID < 2) { // round 0，1，不需要做任何处理，直接输出pass

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

        string chupai;
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
                sout << quanzhongzuixiao(chupai);
                response.push_back(sout.str());
                hand.erase(find(hand.begin(), hand.end(), chupai));//从手牌中删除打出的牌
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

                else if (op == "BUHUA") { // 补花
                    sin >> stmp;
                    if (now_id == myPlayerID) {

                        hand.erase(find(hand.begin(), hand.end(), stmp));
                    }
                    hua_num += 1;
                }

                else if (op == "PENG") {

                    sin >> stmp;
                    if (now_id == myPlayerID) { // 先清除打出去的牌
                        hand.erase(find(hand.begin(), hand.end(), stmp));
                        sout << quanzhongzuixiao(chupai);
                        response.push_back(sout.str());
                        hand.erase(find(hand.begin(), hand.end(), chupai));//从手牌中删除打出的牌
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
                        sout << quanzhongzuixiao(chupai);
                        response.push_back(sout.str());
                        hand.erase(find(hand.begin(), hand.end(), chupai));//从手牌中删除打出的牌
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
                else if (op == "BUGANG") {
                    sin >> stmp;
                    if (now_id == myPlayerID) {

                        hand.erase(find(hand.begin(), hand.end(), stmp));
                        ke_zi.erase(find(ke_zi.begin(), ke_zi.end(), stmp));
                        gang_zi.push_back(stmp);
                        sout << quanzhongzuixiao(chupai);
                        response.push_back(sout.str());
                        hand.erase(find(hand.begin(), hand.end(), chupai));//从手牌中删除打出的牌
                    }
                    else {
                        del_remain(stmp, 1);
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
        paiquanzhong();
        dingfan();

        // 进行当前轮操作
        sin.clear();
        sin.str(request[turnID]);
        sin >> itmp;

        // 算一下番
        /*
        pack:玩家的明牌，每组第一个string为"PENG" "GANG" "CHI" 三者之一，第二个- string为牌代码（吃牌表示中间牌代码），第三个int碰、杠时表示上家、对家、下家供牌，吃时123表示第几张是上家供牌。
        hand:玩家的暗牌，string为牌代码
        winTile:和的那张牌代码
        flowerCount:补花数
        isZIMO:是否为自摸和牌
        isJUEZHANG:是否为和绝张
        isGANG:关于杠，复合点和时为枪杠和，复合自摸则为杠上开花
        isLast:是否为牌墙最后一张，复合自摸为妙手回春，否则为海底捞月
        menFeng:门风，0123表示东南西北
        quanFeng:圈风，0123表示东南西北
        */
        vector<pair<string, pair<string, int> > > pack;
        MahjongInit();
        pair<string, pair<string, int> > p = { "GANG",{"W1",1} };
        pack.push_back(p);
        string winTile;
        int flowerCount;
        int isZIMO;
        int isJUEZHANG;
        int isGANG = 0; //这里还需要做判断
        int isLast;
        int menFeng = myPlayerID;
        int quanFeng = quan;

        int sum_fan = 0;
        int can_hu = 0;
        try {
            auto re = MahjongFanCalculator(pack, hand, winTile, flowerCount, isZIMO, isJUEZHANG, isGANG, isLast, menFeng, quanFeng);
            for (auto i : re) {
                sum_fan += i.first;
            }
            if ((sum_fan - hua_num) >= 8) {
                can_hu = 1;
            }
        }
        catch (const string & error) {

            can_hu = 0;
        }
        if (can_hu == 1) {
            sout << "HU";
            response.push_back(sout.str());
            return 0;
        }

        bool will_pass = 1; // 本轮操作是否输出pass
        if (itmp == 2) { // 如果当前轮是自己摸牌,则随机出牌
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
                    //在此加入是否进行下一步操作
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

    //cout << response[turnID] << endl;
    return 0;
}
