package main

import (
	"bytes"
	"fmt"
	"io"
	"math"
	"math/rand"
	"net"
	"strconv"
	"sync"
	"time"
)

const (
	SERVER_NETWORK = "tcp"
	SERVER_ADDRESS = "127.0.0.1:8085"
	DELIMITER      = '\t'  //数据结束符
)

var wg sync.WaitGroup

func main() {
// 并行运行 服务端 和客户端
	wg.Add(2)
	go serverGo()
	//服务端先建立监听
	time.Sleep(500 * time.Millisecond)
	go clientGo()

	wg.Wait()
}
//服务端 监听 接受连接 读取 发送 数据 
func serverGo() {
	defer wg.Done()
	var listener net.Listener
    // 监听端口
	listener, err := net.Listen(SERVER_NETWORK, SERVER_ADDRESS)
	if err != nil {
		fmt.Printf("Listen error: %s\n", err)
		return
	}
   //在函数返回时 关闭 监听
	defer listener.Close()
	fmt.Printf("listener for the server, address: %s\n", SERVER_ADDRESS)

	for {
	  //等待连接
		conn, err := listener.Accept()
		if err != nil {
			fmt.Printf("Accept Error:%s\n", err)
		}
		fmt.Printf("Established a connection with a client:%s\n", conn.RemoteAddr())
		//当有新连接时，并行处理连接数据
		go handleConn(conn)
	}
}

func handleConn(conn net.Conn) {
	defer conn.Close()
	for {
		conn.SetReadDeadline(time.Now().Add(10 * time.Second))
      //读取客户端 发送的数据
		strReq, err := read(conn)
		if err != nil {
			if err == io.EOF {
				fmt.Printf("The connection is closed by another side (Server)\n")
			} else {
				fmt.Printf("Read Error: %s (Server)\n", err)
			}
			break
		}
		fmt.Printf("Received request: %s (Server)\n", strReq)

		i32Req, err := strconv.Atoi(strReq)
		if err != nil {
			n, err := write(conn, err.Error())
			if err != nil {
				fmt.Printf("Write Error (written %d bytes):%s (Server)\n", n, err)

			}
			fmt.Printf("Sent response (written) %d bytes) :%s (Server)\n", n, err)
			continue
		}
		//计算立方根
		f64Resp := math.Cbrt(float64(i32Req))
		resMsg := fmt.Sprintf("The cube root of %d is %f", i32Req, f64Resp)
        //发送数据到客户端
		n, err := write(conn, resMsg)
		if err != nil {
			fmt.Printf("Write Error (written %d bytes):%s (Server)\n", n, err)

		}
		fmt.Printf("Sent response (written) %d bytes) :%s (Server)\n", n, resMsg)
	}
}
func read(conn net.Conn) (string, error) {
    //分配 1 个字节，因此不需要 在循环时每次初始化该内存
	readBytes := make([]byte, 1)
	var buffer bytes.Buffer
	for {
		_, err := conn.Read(readBytes)
		if err != nil {
			return "", err
		}

		readByte := readBytes[0]
		
		if readByte == DELIMITER {
			break
		}
		buffer.WriteByte(readByte)
	}
	return buffer.String(), nil
}
func write(conn net.Conn, msg string) (int, error) {
	var buffer bytes.Buffer
	buffer.WriteString(msg)
	buffer.WriteByte(DELIMITER)
	return conn.Write(buffer.Bytes())
}
//客户端 连接 发送读取 数据
func clientGo() {
	defer wg.Done()
	conn, err := net.DialTimeout(SERVER_NETWORK, SERVER_ADDRESS, 2*time.Second)
	if err != nil {
		fmt.Printf("Dial Error: %s\n", err)
		return
	}
	defer conn.Close()

	fmt.Printf("Connected to server (remote address: %s,local address:%s)\n", conn.RemoteAddr(), conn.LocalAddr())

	time.Sleep(200 * time.Millisecond)

	requestNum := 5
	conn.SetDeadline(time.Now().Add(5 * time.Millisecond))

	for i := 0; i < requestNum; i++ {
		i32Req := rand.Int31()
		n, err := write(conn, fmt.Sprintf("%d", i32Req))
		if err != nil {
			fmt.Printf("Write Error: %s \n", err)
			continue
		}
		fmt.Printf("Sent request (written %d bytes): %d \n", n, i32Req)
	}

	for j := 0; j < requestNum; j++ {
		strResp, err := read(conn)

		if err != nil {
			if err == io.EOF {
				fmt.Printf("The connection is close by another side\n")

			} else {
				fmt.Printf("Read Error: %s\n", err)

			}
			break
		}
		fmt.Printf("Received response: %s\n", strResp)
	}
}
