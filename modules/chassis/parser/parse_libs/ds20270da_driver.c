#include "ds20270da_driver.h"

static struct ServoMotorDes* servo_fn_des_ptr_array[98];
static struct ServoMotorDes* servo_dn_des_ptr_array[22];
static struct ServoMotorDes* servo_pn_des_ptr_array[46];

static int32_t l_wheel_diameter_mm = -1;
static int32_t l_wheel_distance_mm = -1;
static int32_t l_accelerate_time_ms = -1;
static int32_t l_decelerate_time_ms = -1;
static int32_t l_speed_report_period_ms = -1;
static int32_t l_status_report_period_ms = -1;

// 1, FN
GEN_SERVO_MOTOR_FN_DES(0, 0, 0x0, 0, 0x0, 100, 800, "电机额定转速")
GEN_SERVO_MOTOR_FN_DES(1, 0, 0x1, 0, 0x0, 1, 10000, "电机额定转矩")
GEN_SERVO_MOTOR_FN_DES(2, 0, 0x2, 0, 0x1, 1, 10000, "电机额定电流")
GEN_SERVO_MOTOR_FN_DES(3, 0, 0x3, 0, 0x2, 1, 50, "电机极对数")
GEN_SERVO_MOTOR_FN_DES(4, 0, 0x4, 0, 0x3, 1, 10000, "电机电感")
GEN_SERVO_MOTOR_FN_DES(5, 0, 0x5, 0, 0x4, 1, 10000, "电机电阻")
GEN_SERVO_MOTOR_FN_DES(6, 0, 0x6, 0, 0x5, 1, 10, "编码器类型(0：2500 线增量式码盘 2：伺服电动轮)")
GEN_SERVO_MOTOR_FN_DES(7, 0, 0x7, 0, 0x6, 1, 10000, "编码器线数")
GEN_SERVO_MOTOR_FN_DES(8, 0, 0x8, 0, 0x7, -1024, 1024, "编码器线数")
GEN_SERVO_MOTOR_FN_DES(9, 1, 0x0, 0, 0x8, 0x8, 1, "轴使能")
GEN_SERVO_MOTOR_FN_DES(10, 1, 0x1, 1, 0x0, 0, 1, "轴允许")
GEN_SERVO_MOTOR_FN_DES(11, 1, 0x2, 1, 0x1, 0, 20, "控制模式选择(1：内部速度模式 2：位置模式 3：转矩模式 20：CANopen 模式)")
GEN_SERVO_MOTOR_FN_DES(12, 1, 0x6, 1, 0x2, 0, 1, "正传限位功能选择")
GEN_SERVO_MOTOR_FN_DES(13, 1, 0x7, 1, 0x6, 0, 1, "反转限位功能选择")
GEN_SERVO_MOTOR_FN_DES(14, 1, 0x8, 1, 0x7, 0, 1, "编码器输入UVW逻辑设定")
GEN_SERVO_MOTOR_FN_DES(15, 1, 0x9, 1, 0x8, 0, 1, "编码器输入AB逻辑设定")
GEN_SERVO_MOTOR_FN_DES(16, 1, 0xa, 1, 0x9, 0, 17, "编码器滤波系数")
GEN_SERVO_MOTOR_FN_DES(17, 1, 0x10, 1, 0xa, 0, 10000, "使能后，电机制动器释放延时时间")
GEN_SERVO_MOTOR_FN_DES(18, 1, 0x11, 1, 0x10, 0, 10000, "失能后，电机制动器闭合延时时间")
GEN_SERVO_MOTOR_FN_DES(19, 1, 0x12, 1, 0x11, 0, 300, "电机制动器动作速度阈值")
GEN_SERVO_MOTOR_FN_DES(20, 1, 0x13, 1, 0x12, 0, 300, "制动器动作滤波器")
GEN_SERVO_MOTOR_FN_DES(21, 1, 0x14, 1, 0x13, 0, 11111, "报警使能寄存器1(万位：EH 千位：PE 百位：ST 十位：EC 个位：LU)")
GEN_SERVO_MOTOR_FN_DES(22, 1, 0x15, 1, 0x14, 0, 11111, "报警使能寄存器2(百位：OU 十位：OL 个位：IC)")
GEN_SERVO_MOTOR_FN_DES(23, 1, 0x16, 1, 0x15, 0, 11111, "报警使能寄存器3")
GEN_SERVO_MOTOR_FN_DES(24, 1, 0x17, 1, 0x16, 0, 1, "柔性阻尼停车(0：无功能 1：启动)")
GEN_SERVO_MOTOR_FN_DES(25, 1, 0x18, 1, 0x17, 0, 2000, "电机")
GEN_SERVO_MOTOR_FN_DES(26, 1, 0x19, 1, 0x18, 100, 9999, "密码")
GEN_SERVO_MOTOR_FN_DES(27, 1, 0x1a, 1, 0x19, 0, 20, "EC报警滤波")
GEN_SERVO_MOTOR_FN_DES(28, 1, 0x1b, 1, 0x1a, 1, 10000, "电动轮温度设定阈值")
GEN_SERVO_MOTOR_FN_DES(29, 1, 0x1c, 1, 0x1b, 0, 1000, "温度设定电源电压0.01V")
GEN_SERVO_MOTOR_FN_DES(30, 2, 0x0, 1, 0x1c, 1, 8000, "位置环比例增益")
GEN_SERVO_MOTOR_FN_DES(31, 2, 0x1, 2, 0x0, 1, 8000, "位置环积分增益")
GEN_SERVO_MOTOR_FN_DES(32, 2, 0x4, 2, 0x1, 1, 9999, "电子齿轮比分母")
GEN_SERVO_MOTOR_FN_DES(33, 2, 0x5, 2, 0x4, 1, 9999, "电子齿轮比第一分子")
GEN_SERVO_MOTOR_FN_DES(34, 2, 0x6, 2, 0x5, 1, 9999, "电子齿轮比第二分子")
GEN_SERVO_MOTOR_FN_DES(35, 2, 0x7, 2, 0x6, 1, 9999, "电子齿轮比第三分子")
GEN_SERVO_MOTOR_FN_DES(36, 2, 0x8, 2, 0x7, 1, 9999, "电子齿轮比第四分子")
GEN_SERVO_MOTOR_FN_DES(37, 2, 0xb, 2, 0x8, 0, 17, "脉冲指令干扰滤波")
GEN_SERVO_MOTOR_FN_DES(38, 2, 0xc, 2, 0xb, 8, 8192, "脉冲指令平滑滤波时间常数")
GEN_SERVO_MOTOR_FN_DES(39, 2, 0xe, 2, 0xc, 0, 3000, "位置到达偏差")
GEN_SERVO_MOTOR_FN_DES(40, 2, 0xf, 2, 0xe, 0, 30000, "位置超差报警范围")
GEN_SERVO_MOTOR_FN_DES(41, 2, 0x10, 2, 0xf, 0, 1, "编码器反馈方向取反")
GEN_SERVO_MOTOR_FN_DES(42, 3, 0x0, 2, 0x10, 1, 10000, "速度环第一比例增益")
GEN_SERVO_MOTOR_FN_DES(43, 3, 0x1, 3, 0x0, 1, 5000, "速度环第一积分增益")
GEN_SERVO_MOTOR_FN_DES(44, 3, 0x2, 3, 0x1, 0, 1000, "速度环第一微分增益")
GEN_SERVO_MOTOR_FN_DES(45, 3, 0x3, 3, 0x2, 0, 1000, "速度环第二比例增益")
GEN_SERVO_MOTOR_FN_DES(46, 3, 0x4, 3, 0x3, 0, 1000, "速度环第二积分增益")
GEN_SERVO_MOTOR_FN_DES(47, 3, 0x5, 3, 0x4, 0, 1000, "速度环第二微分增益")
GEN_SERVO_MOTOR_FN_DES(48, 3, 0x6, 3, 0x5, 1, 1000, "速度环第三比例增益")
GEN_SERVO_MOTOR_FN_DES(49, 3, 0x7, 3, 0x6, 0, 1000, "速度环第三积分增益")
GEN_SERVO_MOTOR_FN_DES(50, 3, 0x8, 3, 0x7, 0, 1000, "速度环第三微分增益")
GEN_SERVO_MOTOR_FN_DES(51, 3, 0x9, 3, 0x8, 1, 1000, "速度环第四比例增益")
GEN_SERVO_MOTOR_FN_DES(52, 3, 0xa, 3, 0x9, 0, 1000, "速度环第四积分增益")
GEN_SERVO_MOTOR_FN_DES(53, 3, 0xb, 3, 0xa, 0, 1000, "速度环第四微分增益")
GEN_SERVO_MOTOR_FN_DES(54, 3, 0xd, 3, 0xb, 0, 3000, "速度环第一、二增益切换点速度")
GEN_SERVO_MOTOR_FN_DES(55, 3, 0xe, 3, 0xd, 0, 3000, "速度环第三、四增益切换点速度")
GEN_SERVO_MOTOR_FN_DES(56, 3, 0x10, 3, 0xe, 100, 5000, "负载惯量系数")
GEN_SERVO_MOTOR_FN_DES(57, 3, 0x11, 3, 0x10, 0, 4, "速度模式运行方式选择(0:4段速度模式 1:8段速度模式 2:双按钮启停模式 3:双按钮正反转模式)")
GEN_SERVO_MOTOR_FN_DES(58, 3, 0x14, 3, 0x11, 0, 1000, "模拟速度指令输入增益")
GEN_SERVO_MOTOR_FN_DES(59, 3, 0x15, 3, 0x14, -2000, 2000, "模拟速度指令零漂调整")
GEN_SERVO_MOTOR_FN_DES(60, 3, 0x16, 3, 0x15, 32, 1024, "模拟速度指令滤波系数")
GEN_SERVO_MOTOR_FN_DES(61, 3, 0x17, 3, 0x16, -3000, 3000, "模拟速度指令输入零速阈值")
GEN_SERVO_MOTOR_FN_DES(62, 3, 0x18, 3, 0x17, -6000, 6000, "第一段内部速度")
GEN_SERVO_MOTOR_FN_DES(63, 3, 0x19, 3, 0x18, -6000, 6000, "第二段内部速度")
GEN_SERVO_MOTOR_FN_DES(64, 3, 0x1a, 3, 0x19, -6000, 6000, "第三段内部速度")
GEN_SERVO_MOTOR_FN_DES(65, 3, 0x1b, 3, 0x1a, -6000, 6000, "第四段内部速度")
GEN_SERVO_MOTOR_FN_DES(66, 3, 0x1c, 3, 0x1b, -6000, 6000, "第五段内部速度")
GEN_SERVO_MOTOR_FN_DES(67, 3, 0x1d, 3, 0x1c, -6000, 6000, "第六段内部速度")
GEN_SERVO_MOTOR_FN_DES(68, 3, 0x1e, 3, 0x1d, -6000, 6000, "第七段内部速度")
GEN_SERVO_MOTOR_FN_DES(69, 3, 0x1f, 3, 0x1e, -6000, 6000, "第八段内部速度")
GEN_SERVO_MOTOR_FN_DES(70, 3, 0x20, 3, 0x1f, 0, 5000, "加速时间")
GEN_SERVO_MOTOR_FN_DES(71, 3, 0x21, 3, 0x20, 0, 5000, "减速时间")
GEN_SERVO_MOTOR_FN_DES(72, 3, 0x23, 3, 0x21, 0, 5000, "系统最大运行速度设定")
GEN_SERVO_MOTOR_FN_DES(73, 3, 0x24, 3, 0x23, 8, 8192, "速度反馈滤波系数")
GEN_SERVO_MOTOR_FN_DES(74, 3, 0x25, 3, 0x24, 0, 3000, "零速输出判定阈值")
GEN_SERVO_MOTOR_FN_DES(75, 3, 0x26, 3, 0x25, 0, 3000, "速度到达设定值")
GEN_SERVO_MOTOR_FN_DES(76, 3, 0x27, 3, 0x26, 0, 300, "失速报警等级")
GEN_SERVO_MOTOR_FN_DES(77, 3, 0x28, 3, 0x27, 0, 1, "速度方向选择")
GEN_SERVO_MOTOR_FN_DES(78, 3, 0x29, 3, 0x28, 0, 2000, "零速输出滤波")
GEN_SERVO_MOTOR_FN_DES(79, 4, 0x0, 3, 0x29, 1, 5000, "电流环第一比例增益")
GEN_SERVO_MOTOR_FN_DES(80, 4, 0x1, 4, 0x0, 1, 5000, "电流环第一积分增益")
GEN_SERVO_MOTOR_FN_DES(81, 4, 0x2, 4, 0x1, 1, 5000, "电流环第一微分增益")
GEN_SERVO_MOTOR_FN_DES(82, 4, 0x3, 4, 0x2, 1, 5000, "电流环第二比例增益")
GEN_SERVO_MOTOR_FN_DES(83, 4, 0x4, 4, 0x3, 1, 5000, "电流环第二积分增益")
GEN_SERVO_MOTOR_FN_DES(84, 4, 0x5, 4, 0x4, 0, 1000, "电流环第二微分增益")
GEN_SERVO_MOTOR_FN_DES(85, 4, 0x8, 4, 0x5, -1000, 1000, "模拟转矩输入零漂")
GEN_SERVO_MOTOR_FN_DES(86, 4, 0x9, 4, 0x8, 10, 100, "模拟转矩输入增益")
GEN_SERVO_MOTOR_FN_DES(87, 4, 0xa, 4, 0x9, 0, 1, "模拟转矩指令方向")
GEN_SERVO_MOTOR_FN_DES(88, 4, 0xb, 4, 0xa, 0, 300, "模拟转矩控制方式最大转矩")
GEN_SERVO_MOTOR_FN_DES(89, 4, 0xc, 4, 0xb, 32, 1024, "模拟转矩滤波系数")
GEN_SERVO_MOTOR_FN_DES(90, 4, 0x15, 4, 0xc, 32, 2048, "转矩指令滤波系数")
GEN_SERVO_MOTOR_FN_DES(91, 4, 0x22, 4, 0x15, 0, 1, "转矩限制使能")
GEN_SERVO_MOTOR_FN_DES(92, 4, 0x24, 4, 0x22, 0, 300, "正转(CCW)转矩限制设定)")
GEN_SERVO_MOTOR_FN_DES(93, 4, 0x25, 4, 0x24, 0, 300, "反转(CCW)转矩限制设定)")
GEN_SERVO_MOTOR_FN_DES(94, 4, 0x26, 4, 0x25, -6000, 6000, "转矩模式内部速度限制值(rpm)")
GEN_SERVO_MOTOR_FN_DES(95, 4, 0x2a, 4, 0x26, 0, 10000, "最大电流输出设定")
GEN_SERVO_MOTOR_FN_DES(96, 4, 0x2d, 4, 0x2a, 0, 300, "过载保护等级 单位:0.01倍")
GEN_SERVO_MOTOR_FN_DES(97, 4, 0x2e, 4, 0x2d, 10, 2000, "过载保护时间 单位:0.1s")

// 2, DN
GEN_SERVO_MOTOR_DN_DES(0, 0x0, 0x0, -1, -1, "电机反馈转速（rpm）")
GEN_SERVO_MOTOR_DN_DES(1, 0x1, 0x0, -1, -1, "位置脉冲频率（KHz）")
GEN_SERVO_MOTOR_DN_DES(2, 0x2, 0x1, -1, -1, "电机有效转矩率（‰）")
GEN_SERVO_MOTOR_DN_DES(3, 0x3, 0x2, -1, -1, "位置偏差")
GEN_SERVO_MOTOR_DN_DES(4, 0x4, 0x3, -1, -1, "电机转子位置（单圈）")
GEN_SERVO_MOTOR_DN_DES(5, 0x5, 0x4, -1, -1, "模拟输入通道输入电压（0.01V）")
GEN_SERVO_MOTOR_DN_DES(6, 0x7, 0x5, -1, -1, "速度指令（rpm）")
GEN_SERVO_MOTOR_DN_DES(7, 0x9, 0x7, -1, -1, "系统运行模式")
GEN_SERVO_MOTOR_DN_DES(8, 0xa, 0x9, -1, -1, "母线电压（1V）")
GEN_SERVO_MOTOR_DN_DES(9, 0xb, 0xa, -1, -1, "U 相电流中点")
GEN_SERVO_MOTOR_DN_DES(10, 0xc, 0xb, -1, -1, "V 相电流中点")
GEN_SERVO_MOTOR_DN_DES(11, 0xd, 0xc, -1, -1, "W 相电流中点")
GEN_SERVO_MOTOR_DN_DES(12, 0x12, 0xd, -1, -1, "报警故障码（详见第五章）")
GEN_SERVO_MOTOR_DN_DES(13, 0x13, 0x12, -1, -1, "位置指令脉冲总数低 16 位")
GEN_SERVO_MOTOR_DN_DES(14, 0x14, 0x13, -1, -1, "位置指令脉冲总数高 16 位")
GEN_SERVO_MOTOR_DN_DES(15, 0x15, 0x14, -1, -1, "位置反馈总数低 16 位")
GEN_SERVO_MOTOR_DN_DES(16, 0x16, 0x15, -1, -1, "位置反馈总数高 16 位")
GEN_SERVO_MOTOR_DN_DES(17, 0x17, 0x16, -1, -1, "编码器反馈 UVW")
GEN_SERVO_MOTOR_DN_DES(18, 0x18, 0x17, -1, -1, "软件版本号（年）")
GEN_SERVO_MOTOR_DN_DES(19, 0x19, 0x18, -1, -1, "软件版本号（月日）")
GEN_SERVO_MOTOR_DN_DES(20, 0x1a, 0x19, -1, -1, "软件序列号")
GEN_SERVO_MOTOR_DN_DES(21, 0x1b, 0x1a, -1, -1, "温度传感器温度值（℃）")

// 3, PN
GEN_SERVO_MOTOR_PN_DES(0, 0, 0x0, 0, 0x0, 0, 2199, "数字输出 1")
GEN_SERVO_MOTOR_PN_DES(1, 0, 0x1, 0, 0x0, 0, 2199, "数字输出 2")
GEN_SERVO_MOTOR_PN_DES(2, 0, 0x2, 0, 0x1, 0, 2199, "数字输出 3")
GEN_SERVO_MOTOR_PN_DES(3, 0, 0x3, 0, 0x2, 0, 2199, "数字输出 4")
GEN_SERVO_MOTOR_PN_DES(4, 1, 0x0, 0, 0x3, 0, 3199, "数字输入 1")
GEN_SERVO_MOTOR_PN_DES(5, 1, 0x1, 1, 0x0, 0, 3199, "数字输入 2")
GEN_SERVO_MOTOR_PN_DES(6, 1, 0x2, 1, 0x1, 0, 3199, "数字输入 3")
GEN_SERVO_MOTOR_PN_DES(7, 1, 0x3, 1, 0x2, 0, 3199, "数字输入 4")
GEN_SERVO_MOTOR_PN_DES(8, 1, 0x4, 1, 0x3, 0, 3199, "数字输入 5")
GEN_SERVO_MOTOR_PN_DES(9, 1, 0x5, 1, 0x4, 0, 3199, "数字输入 6")
GEN_SERVO_MOTOR_PN_DES(10, 1, 0x6, 1, 0x5, 0, 3199, "数字输入 7")
GEN_SERVO_MOTOR_PN_DES(11, 1, 0x7, 1, 0x6, 0, 3199, "数字输入 8")
GEN_SERVO_MOTOR_PN_DES(12, 4, 0x0, 1, 0x7, 0, 8192, "母线电压低通滤波系数")
GEN_SERVO_MOTOR_PN_DES(13, 4, 0x3, 4, 0x0, 0, 1200, "过压报警门槛")
GEN_SERVO_MOTOR_PN_DES(14, 4, 0x4, 4, 0x3, 0, 1200, "欠压报警门槛")
GEN_SERVO_MOTOR_PN_DES(15, 4, 0x5, 4, 0x4, 0, 1200, "泄放开始门槛")
GEN_SERVO_MOTOR_PN_DES(16, 4, 0x6, 4, 0x5, 0, 1200, "泄放停止门槛")
GEN_SERVO_MOTOR_PN_DES(17, 4, 0x7, 4, 0x6, -1, 1000, "泄放平均功率保护门槛")
GEN_SERVO_MOTOR_PN_DES(18, 4, 0x8, 4, 0x7, -1, 1000, "泄放瞬时功率保护门槛")
GEN_SERVO_MOTOR_PN_DES(19, 4, 0x9, 4, 0x8, 1, 2000, "过压报警滤波")
GEN_SERVO_MOTOR_PN_DES(20, 4, 0xa, 4, 0x9, 1, 2000, "欠压报警滤波")
GEN_SERVO_MOTOR_PN_DES(21, 5, 0x0, 4, 0xa, 0, 255, "DS232 通讯站号")
GEN_SERVO_MOTOR_PN_DES(22, 5, 0x1, 5, 0x0, 0, 6, "DS232 通讯波特率")
GEN_SERVO_MOTOR_PN_DES(23, 5, 0x2, 5, 0x1, 0, 6, "DS232 通讯协议")
GEN_SERVO_MOTOR_PN_DES(24, 5, 0x3, 5, 0x2, 0, 255, "DS485 通讯站号")
GEN_SERVO_MOTOR_PN_DES(25, 5, 0x4, 5, 0x3, 0, 6, "DS485 通讯波特率")
GEN_SERVO_MOTOR_PN_DES(26, 5, 0x5, 5, 0x4, 0, 6, "DS485 通讯协议")
GEN_SERVO_MOTOR_PN_DES(27, 5, 0x6, 5, 0x5, 0, 9999, "CAN 通讯站号")
GEN_SERVO_MOTOR_PN_DES(28, 5, 0x7, 5, 0x6, 0, 1000, "CAN 通讯波特率")
GEN_SERVO_MOTOR_PN_DES(29, 5, 0x8, 5, 0x7, 0, 6, "CAN 通讯协议")
GEN_SERVO_MOTOR_PN_DES(30, 5, 0x9, 5, 0x8, 0, 1111, "屏蔽选择")
GEN_SERVO_MOTOR_PN_DES(31, 5, 0xa, 5, 0x9, 0, 1, "通讯写 EPROM 保护开关")
GEN_SERVO_MOTOR_PN_DES(32, 5, 0xb, 5, 0xa, 0, 9999, "CAN 看门狗时间")
GEN_SERVO_MOTOR_PN_DES(33, 5, 0xc, 5, 0xb, 0, 2, "通讯断线动作定义, 0：报警 1：关使能 2：零速")
GEN_SERVO_MOTOR_PN_DES(34, 5, 0xd, 5, 0xc, 0, 1, "CAN 差速上报标志")
GEN_SERVO_MOTOR_PN_DES(35, 5, 0xe, 5, 0xd, 0, 9999, "232 通讯断线检测时间")
GEN_SERVO_MOTOR_PN_DES(36, 5, 0xf, 5, 0xe, 0, 9999, "485 通讯断线检测时间")
GEN_SERVO_MOTOR_PN_DES(37, 6, 0x0, 5, 0xf, 0, 9999, "密码")
GEN_SERVO_MOTOR_PN_DES(38, 6, 0x2, 6, 0x0, 0, 1, "通讯报警清除")
GEN_SERVO_MOTOR_PN_DES(39, 6, 0x3, 6, 0x2, 0, 9999, "驱动器复位")
GEN_SERVO_MOTOR_PN_DES(40, 6, 0x4, 6, 0x3, 0, 1, "绕组短接")
GEN_SERVO_MOTOR_PN_DES(41, 6, 0xa, 6, 0x4, 0, 9999, "小车轮径（0.1mm）")
GEN_SERVO_MOTOR_PN_DES(42, 6, 0xb, 6, 0xa, 0, 20000, "小车轮距（0.1mm）")
GEN_SERVO_MOTOR_PN_DES(43, 6, 0xc, 6, 0xb, 0, 1, "小车左右轮定义")
GEN_SERVO_MOTOR_PN_DES(44, 6, 0xd, 6, 0xc, 0, 9999, "柔性阻尼停车，0：无功能 1：报警后启动")
GEN_SERVO_MOTOR_PN_DES(45, 6, 0xe, 6, 0xd, 0, 9999, "柔性阻尼停车, 0：无功能 1：双轴启动")

static int find_servo_motor_fn_des(int code_m, int param_xyz)
{
    for (int i = 0; i < 98; i++) {
        if (servo_fn_des_ptr_array[i]->code_m == code_m &&
                servo_fn_des_ptr_array[i]->param_xyz == param_xyz) {
            return i;
        }
    }

    printf("error for finding servo FN des!\n");

    return -1;
}

static int find_servo_motor_fn_des_by_addr(int addr_l, int addr_h, bool* is_ax0)
{
    for (int i = 0; i < 98; i++) {
        if ((servo_fn_des_ptr_array[i]->ax0.func_addr & 0xff) == addr_l &&
                ((servo_fn_des_ptr_array[i]->ax0.func_addr & 0xff00) >> 8) == addr_h) {
            *is_ax0 = true;
            return i;
        } else if ((servo_fn_des_ptr_array[i]->ax1.func_addr & 0xff) == addr_l &&
             ((servo_fn_des_ptr_array[i]->ax1.func_addr & 0xff00) >> 8) == addr_h) {
            *is_ax0 = false;
            return i;
        }
    }

    //printf("error for finding servo FN des by addr!\n");

    return -1;
}

static int find_servo_motor_dn_des_by_addr(int addr_l, int addr_h, bool* is_ax0)
{
    for (int i = 0; i < 22; i++) {
        if ((servo_dn_des_ptr_array[i]->ax0.func_addr & 0xff) == addr_l &&
                ((servo_dn_des_ptr_array[i]->ax0.func_addr & 0xff00) >> 8) == addr_h) {
            *is_ax0 = true;
            return i;
        } else if ((servo_dn_des_ptr_array[i]->ax1.func_addr & 0xff) == addr_l &&
             ((servo_dn_des_ptr_array[i]->ax1.func_addr & 0xff00) >> 8) == addr_h) {
            *is_ax0 = false;
            return i;
        }
    }

    //printf("error for finding servo DN des by addr!\n");

    return -1;
}

static int find_servo_motor_pn_des_by_addr(int addr_l, int addr_h, bool* is_ax0)
{
    for (int i = 0; i < 46; i++) {
        if ((servo_pn_des_ptr_array[i]->ax0.func_addr & 0xff) == addr_l &&
                ((servo_pn_des_ptr_array[i]->ax0.func_addr & 0xff00) >> 8) == addr_h) {
            *is_ax0 = true;
            return i;
        } else if ((servo_pn_des_ptr_array[i]->ax1.func_addr & 0xff) == addr_l &&
             ((servo_pn_des_ptr_array[i]->ax1.func_addr & 0xff00) >> 8) == addr_h) {
            *is_ax0 = false;
            return i;
        }
    }

    //printf("error for finding servo PN des by addr!\n");

    return -1;
}

static int find_servo_motor_dn_des(int param_xyz)
{
    for (int i = 0; i < 22; i++) {
        if (servo_dn_des_ptr_array[i]->param_xyz == param_xyz) {
            return i;
        }
    }

    printf("error for finding servo DN des!\n");

    return -1;
}

static int find_servo_motor_pn_des(int code_m, int param_xyz)
{
    for (int i = 0; i < 46; i++) {
        if (servo_pn_des_ptr_array[i]->code_m == code_m &&
                servo_pn_des_ptr_array[i]->param_xyz == param_xyz) {
            return i;
        }
    }

    printf("error for finding servo PN des!\n");

    return -1;
}

static void show_servo_motor_fn_des(int code_m, int param_xyz)
{
    do {
        struct ServoMotorDes* x =
            servo_fn_des_ptr_array[
            find_servo_motor_fn_des(code_m, param_xyz)];

        if (x == NULL) {
            printf("get null servo FN des!\n");
            return;
        }

        printf("F0/1.%d.%#x, seq: %d, address: %#x/%#x, "
                "value: %d/%d(limit: %d ~ %d), des: %s\n",
                code_m, param_xyz,
                x->seq, x->ax0.func_addr,
                x->ax1.func_addr, x->ax0.reg_value,
                x->ax1.reg_value, x->lower_limit,
                x->upper_limit, x->func_str);
    } while (0);
}

static void show_servo_motor_dn_des(int param_xyz)
{
    do {
        struct ServoMotorDes* x =
            servo_dn_des_ptr_array[
            find_servo_motor_dn_des(param_xyz)];

        if (x == NULL) {
            printf("get null servo DN des!\n");
            return;
        }

        printf("D0/1.%#x, seq: %d, address: %#x/%#x, "
                "value: %d/%d, des: %s\n",
                param_xyz,
                x->seq, x->ax0.func_addr,
                x->ax1.func_addr, x->ax0.reg_value,
                x->ax1.reg_value,
                x->func_str);
    } while (0);
}

static void show_servo_motor_pn_des(int code_m, int param_xyz)
{
    do {
        struct ServoMotorDes* x =
            servo_pn_des_ptr_array[
            find_servo_motor_pn_des(code_m, param_xyz)];

        if (x == NULL) {
            printf("get null servo PN des!\n");
            return;
        }

        printf("P0/1.%d.%#x, seq: %d, address: %#x, "
                "value: %d(limit: %d ~ %d), des: %s\n",
                code_m, param_xyz, x->seq,
                x->ax0.func_addr,
                x->ax0.reg_value,
                x->lower_limit,
                x->upper_limit,
                x->func_str);
    } while (0);
}

void init_servo_motor_fn_des()
{
    struct ServoMotorDes* x = &f_4_0x2e;

    for (int i = 97; i >= 0; i--) {
        servo_fn_des_ptr_array[i] = x;
        x = x->des_last;
    }
}

void init_servo_motor_dn_des()
{
    struct ServoMotorDes* x = &d_0x1b;

    for (int i = 21; i >= 0; i--) {
        servo_dn_des_ptr_array[i] = x;
        x = x->des_last;
    }
}

void init_servo_motor_pn_des()
{
    struct ServoMotorDes* x = &p_6_0xc;

    for (int i = 45; i >= 0; i--) {
        servo_pn_des_ptr_array[i] = x;
        x = x->des_last;
    }
}

void show_all_servo_motor_fn_des()
{
    for (int i = 0; i < 98; i++) {
        struct ServoMotorDes* x = servo_fn_des_ptr_array[i];
        show_servo_motor_fn_des(x->code_m, x->param_xyz);
    }
}

void show_all_servo_motor_dn_des()
{
    for (int i = 0; i < 22; i++) {
        struct ServoMotorDes* x = servo_dn_des_ptr_array[i];
        show_servo_motor_dn_des(x->param_xyz);
    }
}

void show_all_servo_motor_pn_des()
{
    for (int i = 0; i < 46; i++) {
        struct ServoMotorDes* x = servo_pn_des_ptr_array[i];
        show_servo_motor_pn_des(x->code_m, x->param_xyz);
    }
}

void get_servo_motor_all_fn_addrs(int16_t* addr, int* size)
{
    *size = 98 * 2;

    for (int i = 0; i < 98; i++) {
        struct ServoMotorDes* x = servo_fn_des_ptr_array[i];
        addr[2 * i] = x->ax0.func_addr;
        addr[2 * i + 1] = x->ax1.func_addr;
    }
}

void get_servo_motor_all_dn_addrs(int16_t* addr, int* size)
{
    *size = 22 * 2;

    for (int i = 0; i < 22; i++) {
        struct ServoMotorDes* x = servo_dn_des_ptr_array[i];
        addr[2 * i] = x->ax0.func_addr;
        addr[2 * i + 1] = x->ax1.func_addr;
    }
}

void get_servo_motor_all_pn_addrs(int16_t* addr, int* size)
{
    *size = 46;

    for (int i = 0; i < 46; i++) {
        struct ServoMotorDes* x = servo_pn_des_ptr_array[i];
        addr[i] = x->ax0.func_addr;
    }
}

void config_servo_motor_parameters(const int32_t wheel_diameter_mm,
        const int32_t wheel_distance_mm,
        const int32_t accelerate_time_ms,
        const int32_t decelerate_time_ms,
        const int32_t speed_report_period_ms,
        const int32_t status_report_period_ms)
{
    l_wheel_diameter_mm = wheel_diameter_mm;
    l_wheel_distance_mm = wheel_distance_mm;
    l_accelerate_time_ms = accelerate_time_ms;
    l_decelerate_time_ms = decelerate_time_ms;
    l_speed_report_period_ms = speed_report_period_ms;
    l_status_report_period_ms = status_report_period_ms;
}

void parse_servo_motor_info(const uint8_t* addr, size_t len,
        int* speed_left, int* speed_right,
        int* encoder_left, int* encoder_right,
        int* status_left, int* status_right)
{
#if 1
    if (addr[2] == DS_MOTOR_READ_BOTH_CMD ||
            addr[2] == DS_MOTOR_WRITE_BOTH_CMD ||
            addr[2] == DS_MOTOR_WRITE_CANOPEN_1 ||
            addr[2] == DS_MOTOR_WRITE_CANOPEN_2 ||
            addr[2] == DS_MOTOR_WRITE_CANOPEN_4) {
        //ignore read/write echo messages
        /*
        printf("can echo!!: [ 0x%02x 0x%02x | 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ]\n",
                addr[0], addr[1], addr[2], addr[3], addr[4],
                addr[5], addr[6], addr[7], addr[8], addr[9]);
                */
        return;
    }
#endif

    if (len != 10 && len != 6 && len != 5) {
        printf("parse invalid data len: %2ld [ ", len);
        for (size_t l = 0; l < len ; l++) {
            printf("0x%02x ", addr[l]);
        }
        printf("]\n");
        return;
    }

    int id = addr[0] | (addr[1] << 8);

    //speed & encoder
    if (id == 0x1081) {
        //speed & encoder for axle-1 : right
        int speed = addr[2] | (addr[3] << 8) | (addr[4] << 16) | (addr[5] << 24);
        int encoder = addr[6] | (addr[7] << 8) | (addr[8] << 16) | (addr[9] << 24);
        *speed_right = speed;
        *encoder_right = encoder;
        return;
    } else if (id == 0x1082) {
        //speed & encoder for axle-2 : left
        int speed = addr[2] | (addr[3] << 8) | (addr[4] << 16) | (addr[5] << 24);
        int encoder = addr[6] | (addr[7] << 8) | (addr[8] << 16) | (addr[9] << 24);
        *speed_left = speed;
        *encoder_left = encoder;
        return;
    } else if (id == 0x2081) {
        //status for axle-1 : right
        int status = addr[2] | (addr[3] << 8) | (addr[4] << 16) | (addr[5] << 24);
        *status_right = status;
        return;
    } else if (id == 0x2082) {
        int status = addr[2] | (addr[3] << 8) | (addr[4] << 16) | (addr[5] << 24);
        //status for axle-2 : left
        *status_left = status;
        return;
    } else if (id == 0x4001 || id == 0x4002) {
        //ignore speed control echo
        return;
    }

    if (addr[2] == DS_MOTOR_ACK_READ_CMD) {
#if 0
        printf("READ ACK: [ 0x%02x 0x%02x | 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ]\n",
                addr[0], addr[1], addr[2], addr[3], addr[4],
                addr[5], addr[6], addr[7], addr[8], addr[9]);
#endif
        //addr[3] : addr Low
        //addr[4] : addr High
        int i = 0;
        bool is_ax0 = false;
        if ((i = find_servo_motor_fn_des_by_addr(addr[3], addr[4], &is_ax0)) >= 0) {
            struct ServoMotorDes* x = servo_fn_des_ptr_array[i];
            if (is_ax0)
                x->ax0.reg_value = addr[6] | (addr[7] << 8);
            else
                x->ax1.reg_value = addr[6] | (addr[7] << 8);
            //printf("fn ~ i: %d, ax0: %#x, ax1: %#x\n", i, x->ax0.reg_value, x->ax1.reg_value);
        } else if ((i = find_servo_motor_dn_des_by_addr(addr[3], addr[4], &is_ax0)) >= 0) {
            struct ServoMotorDes* x = servo_dn_des_ptr_array[i];
            if (is_ax0)
                x->ax0.reg_value = addr[6] | (addr[7] << 8);
            else
                x->ax1.reg_value = addr[6] | (addr[7] << 8);
            printf("dn ~ i: %d, ax0: %#x, ax1: %#x\n", i, x->ax0.reg_value, x->ax1.reg_value);
        } else if ((i = find_servo_motor_pn_des_by_addr(addr[3], addr[4], &is_ax0)) >= 0) {
            struct ServoMotorDes* x = servo_pn_des_ptr_array[i];
            if (is_ax0)
                x->ax0.reg_value = addr[6] | (addr[7] << 8);
            else
                x->ax1.reg_value = addr[6] | (addr[7] << 8);
            //printf("pn ~ i: %d, ax0: %#x, ax1: %#x\n", i, x->ax0.reg_value, x->ax1.reg_value);
        }
    } else if (addr[2] == DS_MOTOR_ACK_WRITE_CMD){
#if 1
        printf("WRITE ACK: [ 0x%02x 0x%02x | 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ]\n",
                addr[0], addr[1], addr[2], addr[3], addr[4],
                addr[5], addr[6], addr[7], addr[8], addr[9]);
#endif
    } else if (addr[2] == DS_MOTOR_WRITE_DIFF_SPEED){
#if 1
        printf("WRITE DIFF SPEED ACK: [ 0x%02x 0x%02x | 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ]\n",
                addr[0], addr[1], addr[2], addr[3], addr[4],
                addr[5], addr[6], addr[7], addr[8], addr[9]);
#endif
    } else if (addr[2] == 0x2b && addr[3] == 0x40) {
        //axle operation echo
    } else {
        printf("DATA UNKNOWN: [ 0x%02x 0x%02x | 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ]\n",
                addr[0], addr[1], addr[2], addr[3], addr[4],
                addr[5], addr[6], addr[7], addr[8], addr[9]);
    }
}

//control APIs
void config_servo_motor_init(int16_t* addr, int* value, int* size)
{
    //speed 0
    addr[*size] = f_3_0x18.ax0.func_addr;
    value[*size] = 0;
    *size += 1;

    //diff speed report
    addr[*size] = p_5_0xd.ax0.func_addr;
    value[*size] = 0x00010000; //enable
    *size += 1;

    //wheel diameter
    addr[*size] = p_6_0xa.ax0.func_addr;
    value[*size] = l_wheel_diameter_mm | (l_wheel_diameter_mm << 16); //1411 (0.1mm)
    *size += 1;

    //wheels distance
    addr[*size] = p_6_0xb.ax0.func_addr;
    value[*size] = l_wheel_distance_mm | (l_wheel_distance_mm << 16);//5310 (0.1mm)
    *size += 1;

    //left & right wheel define
    addr[*size] = p_6_0xc.ax0.func_addr;
    value[*size] = 0x00000000;
    *size += 1;

    //speed 0
    addr[*size] = f_3_0x18.ax0.func_addr;
    value[*size] = 0;
    *size += 1;
}

void config_servo_motor_disable(int16_t* addr, int* value, int* size)
{
    //speed 0
    addr[*size] = f_3_0x18.ax0.func_addr;
    value[*size] = 0;
    *size += 1;

    //axle disable
    addr[*size] = f_1_0x0.ax0.func_addr;
    value[*size] = 0;
    *size += 1;
}

void config_servo_motor_stop(int16_t* addr, int* value, int* size)
{
    addr[*size] = f_3_0x18.ax0.func_addr;
    value[*size] = 0;
    *size += 1;
}

void config_servo_motor_wheel_speed(int16_t* addr, int* value, int* size)
{
    addr[*size] = f_3_0x18.ax0.func_addr;
    *size += 1;
}

void config_servo_motor_wheel_reverse(int16_t* addr, int* value, int* size)
{
    //left & right wheel define
    addr[*size] = p_5_0xd.ax0.func_addr;
    *size += 1;
}

//canopen config
void config_servo_motor_canopen_enable(int16_t* addr, int* value, int* size)
{
    //can open mode
    addr[*size] = f_1_0x2.ax0.func_addr;
    value[*size] = 0x00140014;
    *size += 1;
}

void config_servo_motor_canopen_mode(uint8_t* cmd, int16_t* addr, uint8_t* sub_id, int* value, int* size)
{
    //mode config, speed mode: PV(0x03)
    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_1;
    addr[*size] = 0x6060;
    sub_id[*size] = 0x00;
    value[*size] = 0x00030000;
    *size += 1;
}

//config accelerate / moderate time
void config_servo_motor_acc_moderate_time(uint8_t* cmd, int16_t* addr, uint8_t* sub_id, int* value, int* size)
{
    //acc time
    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_4;
    addr[*size] = 0x6083;
    sub_id[*size] = 0x00;
    value[*size] = l_accelerate_time_ms << 16;
    *size += 1;

    //moderate time
    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_4;
    addr[*size] = 0x6084;
    sub_id[*size] = 0x00;
    value[*size] = l_decelerate_time_ms << 16;
    *size += 1;
}

//config tpdo & rpdo
void config_servo_motor_tpdo_rpdo(uint8_t* cmd, int16_t* addr, uint8_t* sub_id, int* value, int* size)
{
    //tpdo1, speed & encoder
    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_1;
    addr[*size] = 0x1a00;
    sub_id[*size] = 0x00;
    value[*size] = 0x00000000;
    *size += 1;

    //speed dictionary
    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_4;
    addr[*size] = 0x1a00;
    sub_id[*size] = 0x01;
    value[*size] = 0x00206069;
    *size += 1;

    //encoder dictionary
    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_4;
    addr[*size] = 0x1a00;
    sub_id[*size] = 0x02;
    value[*size] = 0x00206063;
    *size += 1;

    /*
    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_1;
    addr[*size] = 0x1a00;
    sub_id[*size] = 0x00;
    value[*size] = 0x00020000;
    *size += 1;
    */

    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_1;
    addr[*size] = 0x1800;
    sub_id[*size] = 0x02;
    value[*size] = 0x00fe0000;
    *size += 1;

    //speed & encoder period
    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_1;
    addr[*size] = 0x1800;
    sub_id[*size] = 0x03;
    value[*size] = l_speed_report_period_ms << 16;
    *size += 1;
    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_1;
    addr[*size] = 0x1800;
    sub_id[*size] = 0x05;
    value[*size] = l_speed_report_period_ms << 16;
    *size += 1;

    //tpdo2, status
    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_1;
    addr[*size] = 0x1a01;
    sub_id[*size] = 0x00;
    value[*size] = 0x00000000;
    *size += 1;

    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_4;
    addr[*size] = 0x1a01;
    sub_id[*size] = 0x01;
    value[*size] = 0x00106041;
    *size += 1;

    /*
    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_4;
    addr[*size] = 0x1a01;
    sub_id[*size] = 0x02;
    value[*size] = 0x00105002;
    *size += 1;
    */

    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_1;
    addr[*size] = 0x1a01;
    sub_id[*size] = 0x00;
    value[*size] = 0x00020000;
    *size += 1;

    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_1;
    addr[*size] = 0x1801;
    sub_id[*size] = 0x02;
    value[*size] = 0x00fe0000;
    *size += 1;

    //status period
    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_1;
    addr[*size] = 0x1801;
    sub_id[*size] = 0x03;
    value[*size] = l_status_report_period_ms << 16;
    *size += 1;

    //status period
    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_1;
    addr[*size] = 0x1801;
    sub_id[*size] = 0x05;
    value[*size] = l_status_report_period_ms << 16;
    *size += 1;

    //rpdo3
    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_1;
    addr[*size] = 0x1602;
    sub_id[*size] = 0x00;
    value[*size] = 0x00000000;
    *size += 1;

    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_4;
    addr[*size] = 0x1602;
    sub_id[*size] = 0x01;
    value[*size] = 0x001060ff;
    *size += 1;

    cmd[*size] = DS_MOTOR_WRITE_CANOPEN_1;
    addr[*size] = 0x1602;
    sub_id[*size] = 0x00;
    value[*size] = 0x00010000;
    *size += 1;
}
