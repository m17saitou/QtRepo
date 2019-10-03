//今後また増えるかもしれない
#pragma once
class bit_calculation
{
public:
    static inline bool low_return_bit(int x, int bit)
    { //下からx番目のbitが立っているかどうか返す
        bit = bit & (0x1 << x);
        return (bit != 0);
    }
    static inline bool low_return_bit(int x, uint16_t bit)
    { //下からx番目のbitが立っているかどうか返す
        bit = bit & (0x1 << x);
        return (bit != 0);
    }
    static bool high_return_bit(int x, int y, int bit)
    { //xで敵か味方か判断,yでそのチームの下から何番目かを判断
        //この時,x=1のときは味方、x=-1のときは敵とする.
        if (x == 1)
            bit = bit & 0x00FF;
        else if (x == -1)
        {
            bit = bit & 0xFF00;
            bit = bit >> 8;
        }
        bit = bit & (0x1 << y);
        return (bit != 0);
    }
    static inline int low_add_bit(int ori_bit, int num)
    { //ori_bitの下からnum番目のbitを立たせる(numは0から数える)
        //もしnum番目のbitが立っていたらori_bitをそのまま返す
        int bit = 0x0001;
        bit = bit << num;
        bit = ori_bit | bit;
        return bit;
    }
    static int high_add_bit(int x, int ori_bit, int num)
    { //xで敵か味方か判断,この時,x=1のときは味方、x=-1のときは敵とする.
        //そのチームのnum番目のbitを立てる,もし立っていた場合はori_bitをそのまま返す
        int bit;
        if (x == 1)
        {
            bit = 0x0001;
            bit = bit << num;
            bit = bit | ori_bit;
        }
        else if (x == -1)
        {
            bit = 0x0100;
            bit = bit << num;
            bit = bit | ori_bit;
        }
        return bit;
    }

     static uint16_t high_add_bit(int x, uint16_t ori_bit, int num)
    { //xで敵か味方か判断,この時,x=1のときは味方、x=-1のときは敵とする.
        //そのチームのnum番目のbitを立てる,もし立っていた場合はori_bitをそのまま返す
        uint16_t bit;
        if (x == 1)
        {
            bit = 0x0001;
            bit = bit << num;
            bit = bit | ori_bit;
        }
        else if (x == -1)
        {
            bit = 0x0100;
            bit = bit << num;
            bit = bit | ori_bit;
        }
        return bit;
    }
};
