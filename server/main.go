package main

import (
	"flag"
	"fmt"
	"net"

	"github.com/pkg/errors"
)

func main() {
	var address string
	flag.StringVar(&address, "l", "0.0.0.0:8241", "listen address")
	flag.Parse()

	run(address)
}

func run(address string) error {
	listener, err := net.Listen("tcp", address)
	if err != nil {
		return errors.WithStack(err)
	}
	defer listener.Close()

	for {
		conn, err := listener.Accept()
		if err != nil {
			return errors.WithStack(err)
		}
		go handleConnection(conn)
	}
}

func handleConnection(conn net.Conn) {
	defer conn.Close()

	buffer := make([]byte, 1024)
	for {
		length, err := conn.Read(buffer)
		if err != nil {
			//if err != io.EOF {
			fmt.Println("failed to read:", err)
			//}
			return
		}

		if _, err := conn.Write(buffer[:length]); err != nil {
			fmt.Println("Failed to write:", err)
			return
		}
	}
}
