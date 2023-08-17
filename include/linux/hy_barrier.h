/**
 *
 * Release under GPLv-3.0.
 * 
 * @file    hy_barrier.h
 * @brief   
 * @author  gnsyxiang <gnsyxiang@163.com>
 * @date    08/12 2021 14:08
 * @version v0.0.1
 * 
 * @since    note
 * @note     note
 * 
 *     change log:
 *     NO.     Author              Date            Modified
 *     00      zhenquan.qiu        08/12 2021      create the file
 * 
 *     last modified: 08/12 2021 14:08
 */
#ifndef __LIBHY_UTILS_INCLUDE_HY_BARRIER_H_
#define __LIBHY_UTILS_INCLUDE_HY_BARRIER_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined _WIN32 || defined __CYGWIN__
#else
#ifdef __GNUC__
#if __GNUC__ >= 4 || defined(__arm__)
/* Optimization barrier */
/* The "volatile" is due to gcc bugs */
#ifndef HY_BARRIER
// __asm__用于指示编译器在此插入汇编语句
// __volatile__用于告诉编译器，严禁将此处的汇编语句与其它的语句重组合优化
// memory告诉编译器汇编代码会使内存里面的值更改，编译器应使用内存里的新值而非寄存器里保存的老值。
// "":::表示这是个空指令。barrier()不用在此插入一条串行化汇编指令。
#define HY_BARRIER()   __asm__ __volatile__("": : :"memory")
#endif

#ifndef HY_SMP_MB
#define HY_SMP_MB()    HY_BARRIER()
#endif

#ifndef HY_SMP_WMB
#define HY_SMP_WMB()   HY_BARRIER()
#endif

#ifndef HY_SMP_RMB
#define HY_SMP_RMB()   HY_BARRIER()
#endif
#else
#error "gcc version too low"
#endif
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif

