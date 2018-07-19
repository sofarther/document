package main

import (
	"fmt"
	"os"
	"os/signal"
	"sync"
	"syscall"
	"time"
)

func t1() {

	sigRev := make(chan os.Signal, 1)

	sigs := []os.Signal{syscall.SIGINT, syscall.SIGQUIT}

	signal.Notify(sigRev, sigs...)

	for sig := range sigRev {
		fmt.Printf("Received a signal: %s\n", sig)
	}
}

func handleSignal() {
   // 定义通道，通道类型只接受 os.Signal 类型， 长度为 1
	sigRev1 := make(chan os.Signal, 1)
    //定义 自行处理的信号
	sigs1 := []os.Signal{syscall.SIGINT, syscall.SIGQUIT}

	fmt.Printf("Set notification for %s... [singRecv1]\n", sigs1)
	//signal.Notify()将 系统发送到当前进程 的信号 通知调用该函数 的调用方
	//并 将 当前进程接受到的信号 放入到 通道 中
	//在接受到 自行处理的信号后，os/singal 包的程序，将其封装为 syscall.Signal 类型
	//并 放入到signal 接受通道中,
	//当 省略 第二个参数时则 会将 所有 除 SIGKILL 和 SIGSTOP 信号外的全部信号 进行自行处理
	signal.Notify(sigRev1, sigs1...)

	//可以指定 多个通道 接受 信号，
	//同一信号  会被 多个通道接受，并分别处理
	sigRev2 := make(chan os.Signal, 1)

	sigs2 := []os.Signal{syscall.SIGQUIT}

	fmt.Printf("Set notification for %s... [singRecv2]\n", sigs2)
	signal.Notify(sigRev2, sigs2...)

	var wg sync.WaitGroup
    // 添加 2 的差量，
	// 每段 并发程序 在调用 Done()方法时，则 差量将减一，
	// Wait()会阻塞等待，直到 差量 为0
	wg.Add(2)
    // 循环读取 通道中接受到的 信号，
	// 当 通道为空时，则阻塞等待，
	//直到 该通道 关闭，则 循环退出
	go func() {
		for sig := range sigRev1 {
			fmt.Printf("Received a signal: %s\n", sig)
		}
		//此时 通道已关闭
		fmt.Println("End.[sigRecv1]")
		wg.Done()
	}()
	go func() {
		for sig := range sigRev2 {
			fmt.Printf("Received a signal: %s\n", sig)
		}
		fmt.Println("End.[sigRecv2]")
		wg.Done()
	}()
   //关闭 一个通道，此时，当发送 SIGQUIT信号时，只有一个通道接受
	//由于
	fmt.Println("Wait for 2 seconds")

	time.Sleep(2 * time.Second)
	fmt.Printf("Stop notification")
	signal.Stop(sigRev1)
	close(sigRev1)

	fmt.Println("Done [sigRecv1]")
   //等待直到 差量 为0， 
   //由于 sigRecv1通道已关闭，而 sigRecv2 ，没有将 SIGINT 信号作为自行处理信号
   //因此 系统默认处理 SIGINT 信号，即退出 程序
	wg.Wait()
}

func sendSignal() {

	cmds := []*exec.Cmd{
		exec.Command("ps", "aux"),
		exec.Command("grep", "cpt6_signal"),

		exec.Command("grep", "-v", "grep"),
		exec.Command("grep", "-v", "run"),
		exec.Command("awk", "{print $2}"),
	}

	output, err := runCmds(cmds)

	fmt.Printf("%v\n", output)
	//	fmt.Println(err)
	//获取当前进程的PID
	// 转为 int 类型，返回两个参数，都需接受
	pid, err := strconv.Atoi(output[1])
	if err != nil {
		fmt.Println("err", output[1])
	}
	fmt.Println(pid)
	//通过PID 获取 进程对象
	proc, err := os.FindProcess(pid)
	//向指定对象发送信号
	err = proc.Signal(syscall.SIGINT)
	if err != nil {
		fmt.Println(err)
	}
}
func main() {
	go func() {
		time.Sleep(5 * time.Second)
		sendSignal()
	}()
	handleSignal()
}

//处理多个系统命令，通过管道 连接
func runCmds(cmds []*exec.Cmd) ([]string, error) {
	l := len(cmds)
	res := make([]string, 0)
	var stdin io.WriteCloser
	var stdout io.ReadCloser
	var err error

	if l == 0 {
		return res, err
	}
	for i := 0; i < l; i++ {
		fmt.Printf("i=%d\n", i)
		stdout, err = cmds[i].StdoutPipe()
		if err != nil {
			return res, err
		}
		if err = cmds[i].Start(); err != nil {
			return res, err
		}
		//当前命令执行后，需关闭当前命令的输入端
		//否则将 一直 等待 上级管道的输出
		if i != 0 {
			if err := stdin.Close(); err != nil {
				return res, err
			}
		}
		if i != l-1 {
			outputBuf := bufio.NewReader(stdout)
			stdin, err = cmds[i+1].StdinPipe()
			if err != nil {
				return res, err
			}
			outputBuf.WriteTo(stdin)
		}
	}
	//处理最后的一个命令的输出结果
	outputBuf := bufio.NewReader(stdout)
	//通过 循环读取执行结果时，不能提前 调用 Wait() 等待执行结束
	// 而是同步获取执行结果
	/*
		if err := cmds[l-1].Wait(); err != nil {
			fmt.Println("fali wait")
			return res, err
		}
	*/
	for {
		line, _, err := outputBuf.ReadLine()

		if err != nil {
			if err == io.EOF {
				break
			} else {
				return res, err
			}
		}
		res = append(res, string(line))
	}
	return res, err
}
//可用
func runCmds_v1(cmds []*exec.Cmd) ([]string, error) {
	l := len(cmds)
	res := make([]string, 1)
	var stdin io.WriteCloser
	var err error

	if l == 0 {
		return res, err
	}
	for i := 0; i < l-1; i++ {
		fmt.Printf("i=%d\n", i)
		stdout, err := cmds[i].StdoutPipe()
		if err != nil {
			return res, err
		}
		if err := cmds[i].Start(); err != nil {
			return res, err
		}
		if i != 0 {
			if err := stdin.Close(); err != nil {
				return res, err
			}
		}
		outputBuf := bufio.NewReader(stdout)
		stdin, err = cmds[i+1].StdinPipe()
		if err != nil {
			return res, err
		}
		outputBuf.WriteTo(stdin)
	}
	fmt.Println("handle last ")
	stdout, err := cmds[l-1].StdoutPipe()
	if err != nil {
		fmt.Println("fail get stdout")
		return res, err
	}
	if err := cmds[l-1].Start(); err != nil {
		fmt.Println("fail start")
		return res, err
	}
	if err := stdin.Close(); err != nil {
		fmt.Println("fali close")
		return res, err
	}
	outputBuf := bufio.NewReader(stdout)
	/*
		if err := cmds[l-1].Wait(); err != nil {
			fmt.Println("fali wait")
			return res, err
		}
	*/
	for {
		line, _, err := outputBuf.ReadLine()

		if err != nil {
			if err == io.EOF {
				break
			} else {
				fmt.Println("handle res")
				return res, err
			}
		}
		res = append(res, string(line))
	}
	return res, err
}