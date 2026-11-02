/*********************************************************
 * config.h - Configuration data for the driver.c program.
 * 中文：config.h：driver.c 程序的配置数据。
 *********************************************************/
#ifndef _CONFIG_H_
#define _CONFIG_H_

/* 
 * CPEs for the baseline (naive) version of the rotate function that
 * was handed out to the students. Rd is the measured CPE for a dxd
 * image. Run the driver.c program on your system to get these
 * numbers.  
 * 中文：发给学生的朴素 rotate 基线的 CPE（每元素周期数）。Rd 是 d×d 图像的实测 CPE；在
 * 自己的系统上运行 driver.c 程序可获得这些值。
 */
#define R64    14.7
#define R128   40.1
#define R256   46.4
#define R512   65.9
#define R1024  94.5

/* 
 * CPEs for the baseline (naive) version of the smooth function that
 * was handed out to the students. Sd is the measure CPE for a dxd
 * image. Run the driver.c program on your system to get these
 * numbers.  
 * 中文：发给学生的朴素 smooth 基线的 CPE。Sd 是 d×d 图像的实测 CPE；在自己的系统上运行
 *  driver.c 程序可获得这些值。
 */
#define S32   695
#define S64   698
#define S128  702
#define S256  717
#define S512  722


#endif /* _CONFIG_H_  中文：头文件保护宏 _CONFIG_H_ 结束。 */
