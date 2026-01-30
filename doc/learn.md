

• 下面是一份“由浅到深”的学习清单，结合这个项目的真实能力点，尽量做到每一项都有明确目标和验收方式。

阶段 1：熟悉工程与最小运行

- 目标：能编译、运行、调试 NEMU 和 testcase
- 练习
    - make run USERPROG=obj/testcase/add
      make run-c USERPROG=obj/testcase/hello
      make run USERPROG=obj/testcase/hello AUTO=1
      make run USERPROG=obj/game/game
    - si / info r / x / p / bt
- 验收
    - 能解释 GOOD TRAP 的含义
    - 能用观察点定位一个变量变化

阶段 2：指令执行与调试

- 目标：理解“取指 → 译码 → 执行”的路径
- 练习
    - 跟踪 cpu_exec → exec → opcode_table
    - 自己添加一个简单指令（例如 setnz 或 cmovz）
- 验收
    - 新增指令通过一个小 testcase（自写或现有）

阶段 3：内存系统

- 目标：理解物理/线性/虚拟地址转换
- 练习
    - 跟踪 swaddr_read → lnaddr_read → hwaddr_read
    - 加断点看分页未开启/开启时的行为差异
- 验收
    - 能解释一次 page translate 的完整过程

阶段 4：系统调用与中断

- 目标：理解 int 0x80 的软件中断路径
- 练习
    - 跟踪 int → IDT → irq_handle → do_syscall
    - 新增一个 syscall（比如 SYS_write 输出字符）
- 验收
    - 一个用户程序调用你的 syscall 能正确返回

阶段 5：设备与 I/O

- 目标：理解端口 I/O 与设备模型
- 练习
    - 跟踪 outb/inb → NEMU pio_write/pio_read
    - 让串口输出路径完整跑通
- 验收
    - 用户程序能稳定输出字符串到终端

阶段 6：简化内核

- 目标：理解 kernel loader 与内存管理
- 练习
    - 理解 kernel/src/elf/elf.c 的装载流程
    - 手动修改 loader，让它加载自己的 ELF
- 验收
    - 自己的 ELF 能被加载并执行

阶段 7：提升指令覆盖

- 目标：补齐缺失指令，提升 testcases 覆盖率
- 练习
    - 逐个排查 invalid opcode
    - 实现缺失指令（如 setcc/bitwise）
- 验收
    - make test 全部通过

阶段 8：可选挑战（偏进阶）

- 完整中断：实现 INTR 线路、硬中断、8259 逻辑
- 设备扩展：VGA/键盘/IDE 更完整的可编程性
- 性能优化：cache/TLB 策略调优并测量效果