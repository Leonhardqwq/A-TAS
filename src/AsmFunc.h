/*
 * @Description：A-TAS汇编函数
 */
#pragma once
#ifndef __ASM_FUNC_H__
#define __ASM_FUNC_H__
#include "avz.h"

// 设定音乐
inline void SetMusic(int musicid) {
    asm volatile("movl %[musicid], %%edi;"
                 "movl 0x6A9EC0, %%eax;"
                 "movl 0x83C(%%eax), %%eax;"
                 "movl $0x45B750, %%edx;"
                 "calll *%%edx;"
        :
        : [musicid] "rm"(musicid)
        : "eax", "edx", "edi");
}

// 在当帧更新植物动画的颜色
inline void UpdateReanimColor(int plant_index) {
    asm volatile("movl 0x6A9EC0, %%eax;"
                 "movl 0x768(%%eax), %%eax;"
                 "movl 0xAC(%%eax), %%eax;"
                 "movl $0x14C, %%ecx;"
                 "imull %[plant_index], %%ecx;"
                 "addl %%ecx, %%eax;"
                 "pushl %%eax;"
                 "movl $0x4635C0, %%edx;"
                 "calll *%%edx;"
        :
        : [plant_index] "m"(plant_index)
        : "eax", "ecx", "edx");
}

// 设定Dance
inline void SetDance(bool state) {
    asm volatile("movl 0x6A9EC0, %%eax;"
                 "movl 0x768(%%eax), %%ecx;"
                 "pushl %%ecx;"
                 "movl %[state], %%ebx;"
                 "movl $0x41AFD0, %%eax;"
                 "calll *%%eax;"
        :
        : [state] "rm"(unsigned(state))
        : "eax", "ebx", "ecx");
}

// -1 = 正常，0 = 加速，1 = 减速
static int DCState = -1;
inline void DanceCheat() {
    if (DCState != -1)
        SetDance(DCState);
}

// 生成字幕
enum CaptionStyle {
    LOWERMIDDLE = 1, // y 400 ~ 510
    LOWERMIDDLESTAY,
    LOWERPART, // y 480 ~ 580 约5s
    LATER,
    LATERSTAY,
    BOTTOM,     // y 530 ~ 585 约15s
    BOTTOMFAST, // y 530 ~ 585 约5s
    STAY,
    TALLFAST,
    TALL10SEC,
    TALL8SEC,
    CENTER,
    CENTERFAST,
    BOTTOMWHITE,
    CENTERRED,
    TOPYELLOW,
    ZENGARDEN,
};

struct CaptionSet {
    CaptionStyle style = BOTTOMFAST;
    int duration = 100;
};

template <typename... CaptionArgs>
void CreateCaption(const std::string& content, CaptionSet captionSet = {}, CaptionArgs... args) {
    // if (AGetMainObject() == nullptr)
    //     return; // 防崩溃代码
    std::string _content = content;
    std::initializer_list<int> {(__StringConvert(_content, args), 0)...};
    const char* caption = _content.c_str();
    uint32_t str[7] {};
    str[1] = (uint32_t)caption;
    str[5] = (uint32_t)strlen(caption);
    str[6] = 16 + str[5];
    void* _str = str;
    asm volatile("movl 0x6A9EC0, %%esi;"
                 "movl 0x768(%%esi), %%esi;"
                 "movl 0x140(%%esi), %%esi;"
                 "movl %[style], %%ecx;"
                 "movl %[_str], %%edx;"
                 "movl $0x459010, %%eax;"
                 "calll *%%eax;"
        :
        : [_str] "m"(_str), [style] "m"(captionSet.style)
        : "eax", "ecx", "edx", "esi");
    AGetMainObject()->Words()->DisappearCountdown() = captionSet.duration;
}

// 冰道覆盖
inline bool isIceTrailCover(int Row, int Col) {
    bool IceCovered;
    asm volatile("movl %[Row], %%eax;"
                 "pushl %[Col];"
                 "movl 0x6A9EC0, %%esi;"
                 "movl 0x768(%%esi), %%esi;"
                 "movl $0x40DFC0, %%ecx;"
                 "calll *%%ecx;"
                 "mov %%eax, %[IceCovered];"
        : [IceCovered] "=rm"(IceCovered)
        : [Row] "rm"(Row - 1), [Col] "rm"(Col - 1)
        : "eax", "ecx", "esi");
    return IceCovered;
}

#endif //!__ASM_FUNC_H__