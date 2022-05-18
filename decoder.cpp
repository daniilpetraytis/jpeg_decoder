#include "decoder.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "huffman.h"
#include <algorithm>
#include <cmath>

int curs = 0;

struct Channel {
    std::vector<int> kvant_table;
    HuffmanTree huffman_ac;
    HuffmanTree huffman_dc;
    int h;
    int v;
};
template <class T>
std::vector<std::vector<T>> Func(std::vector<T> x) {
    std::vector<std::vector<T>> res = {{x[0], x[1], x[5], x[6], x[14], x[15], x[27], x[28]},
                                        {x[2], x[4], x[7], x[13], x[16], x[26], x[29], x[42]},
                                        {x[3], x[8], x[12], x[17], x[25], x[30], x[41], x[43]},
                                        {x[9], x[11], x[18], x[24], x[31], x[40], x[44], x[53]},
                                        {x[10], x[19], x[23], x[32], x[39], x[45], x[52], x[54]},
                                        {x[20], x[22], x[33], x[38], x[46], x[51], x[55], x[60]},
                                        {x[21], x[34], x[37], x[47], x[50], x[56], x[59], x[61]},
                                        {x[35], x[36], x[48], x[49], x[57], x[58], x[62], x[63]}};
    return res;
}

std::vector<int> make_table(int id, std::vector<Channel> all, std::vector<int> potok) {
    std::vector<int> x;
    // find dc
    int k = -1;
    while (all[id].huffman_dc.Move(curs, k)) {
        ++curs;
    }
    ++curs;
    bool flag = false;
    int stepen = pow(2, k - 1);
    int dc = 0;
    for (int p = 0; p != k; ++p) {
        if (p == 0 && potok[curs] == 0) {
            flag = true;
            dc += stepen * ((potok[curs] + 1) % 2);
            stepen /= 2;
            ++curs;
        } else {
            if (flag) {
                dc += stepen * ((potok[curs] + 1) % 2);
                stepen /= 2;
                ++curs;
            } else {
                dc += stepen * potok[curs];
                stepen /=2;
                ++curs;
            }
        }
    }
    if (flag) {
        x.push_back(-1 * dc);
    } else {
        x.push_back(dc);
    }
    //find ac;
    while (x.size() < 64) {
        while (all[id].huffman_ac.Move(potok[curs], k)) {
            ++curs;
        }
        ++curs;
        for (int w = 0; w != k / 16; ++w) {
            x.push_back(0);
        }
        if (k / 16 == 0 && k % 16 == 0) {
            while (x.size() != 64) {
                x.push_back(0);
            }
            return x;
        }
        int ac = 0;
        for (int p = 0; p != k % 16; ++p) {
            if (p == 0 && potok[curs] == 0) {
                flag = true;
                ac += stepen * ((potok[curs] + 1) % 2);
                stepen /= 2;
                ++curs;
            } else {
                if (flag) {
                    ac += stepen * ((potok[curs] + 1) % 2);
                    stepen /= 2;
                    ++curs;
                } else {
                    ac += stepen * potok[curs];
                    stepen /=2;
                    ++curs;
                }
            }
        }
        if (flag) {
            x.push_back(-1 * ac);
        } else {
            x.push_back(ac);
        }
    }
    return x;
}


Image Decode(std::istream& input) {
    (void)input;
    std::vector<Channel> all(3);
    Image res;
    //beginning
    char ff = input.get();
    char d8 = input.get();
    std::map<int, std::vector<int>> kvant_tables; //номер и сама таблица квантования
    int x1 = input.get(), x2;
    while (input.peek() != EOF) {
        std::string com;
        x2 = input.get();
        if (x1 == 255 && x2 == 254) {
            int s1 = input.get(), s2 = input.get();
            s2 += s1 * 256 - 2;
            for (int i = 0; i != s2; ++i) {
                char elem = input.get();
                com += elem;
            }
            res.SetComment(com);
            break;
        } else {
            x1 = x2;
        }
    }
    x1 = input.get();
    x2 = input.get();
    while (x1 == 255 && x2 == 219) {
        int s1 = input.get(), s2 = input.get();
        s2 += s1 * 256;
        int h = input.get();
        int l = h / 16;
        int id = h % 16;
        std::vector<int> kv;
        for (int i = 0; i != s2 - 3; ++i) {
            if (l == 0) {
                h = input.get();
                kv.push_back(h);
            } else {
                h = input.get() * 256 + input.get();
                kv.push_back(h);
            }
        }
        kvant_tables[id] = kv;
        x1 = input.get();
        x2 = input.get();
    }
    int length_baseline = input.get() * 256 + input.get(), precision_baseline = input.get();
    int height = input.get() * 256 + input.get(), width = input.get() * 256 + input.get();
    res.SetSize(width, height);
    int num_of_channels = input.get();
    int h_max = 0, v_max = 0;
    for (int i = 0; i != num_of_channels; ++i) {
        int num_of_channel = input.get();
        int pror = input.get();
        int h = pror / 16, v = pror % 16;
        h_max = std::max(h, h_max);
        v_max = std::max(v, v_max);
        int id_table_kvant = input.get();
        all[num_of_channel - 1].h = h;
        all[num_of_channel - 1].v = v;
        all[num_of_channel - 1].kvant_table = kvant_tables[id_table_kvant];
    }
    x1 = input.get();
    x2 = input.get();
    std::map<int, std::vector<std::vector<uint8_t>>> huffman_ac, huffman_dc;
    while (x1 == 255 && x2 == 196) {
        int s1 = input.get(), s2 = input.get();
        s2 += s1 * 256;
        int x = input.get();
        int huffman_class = x / 16;
        int id_huffman_table = x % 16;
        std::vector<uint8_t> count_term_on_levels;
        int summ_nodes = 0;
        for (int i = 0; i != 16; ++i) {
            count_term_on_levels.push_back(input.get());
            summ_nodes += count_term_on_levels[count_term_on_levels.size() - 1];
        }
        long long int koeff = 1;
        std::vector<uint8_t> values;
        for (int i = 0; i != 16; ++i) {
            for (int j = 0; j != count_term_on_levels[i]; ++j) {
                int ko = koeff;
                int summary = 0;
                for (int m = 0; m != j / 2 + 1; ++m) {
                    summary += input.get() * ko;
                    ko /= 256;
                }
                values.push_back(summary);
            }
            if (i % 2 == 1) {
                koeff *= 256;
            }
        }
        if (huffman_class == 0) {
            huffman_dc[id_huffman_table] = {count_term_on_levels, values};

        } else {
            huffman_ac[id_huffman_table] = {count_term_on_levels, values};
        }
        x1 = input.get();
        x2 = input.get();
    }
    int l = input.get() * 256 + input.get();
    num_of_channels = input.get();
    for (int i = 0; i != num_of_channels; ++i) {
        int id_of_channel = input.get();
        int x = input.get();
        std::cout << huffman_dc[x / 16][0].size() << " " << huffman_dc[x / 16][1].size();
        all[id_of_channel - 1].huffman_dc.Build(huffman_dc[x / 16][0], huffman_dc[x / 16][1]);
        all[id_of_channel - 1].huffman_ac.Build(huffman_ac[x % 16][0], huffman_ac[x % 16][1]);
    }
    int skipping;
    for (int i = 0; i != 3; ++i) {
        skipping = input.get();
    }
    std::vector<int> potok;
    int prev = input.get();
    int now = input.get();
    while (input.peek() != EOF) {
        std::vector<int> helper;
        while (prev != 0) {
            helper.push_back(prev % 2);
            prev /= 2;
        }
        for (int i = helper.size() - 1; i >= 0; --i) {
            potok.push_back(helper[i]);
        }
        prev = now;
        now = input.get();
    }
    int count_y = (h_max / all[1].h) * (v_max / all[1].v);
    while (curs < potok.size()) {
        std::vector<std::vector<int>> tables_y;
        std::vector<int> table_cb, table_cr;
        for (int j = 0; j != count_y; ++j) {
            all[0].huffman_dc.Move(1, x2);
        }
        table_cb = make_table(1, all, potok);

        //making cb table
    }
    return res;
}
