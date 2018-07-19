package main

import (
	"bytes"
	"fmt"
	"io"
	//"math"
	"math/rand"
	"net"
	//"strconv"
	"time"
)

const (
	SERVER_NETWORK = "tcp"
	SERVER_ADDRESS = "127.0.0.1:8085"
	DELIMITER      = '\t'
)

func main() {
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

func read(conn net.Conn) (string, error) {
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
