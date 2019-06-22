package main

import (
	"learn18-rpc/def"
	"log"
	"net/rpc"
)

func main() {
	//make connection to rpc server
	client, err := rpc.DialHTTP("tcp", ":1234")
	if err != nil {
		log.Fatalf("Error in dialing. %s", err)
	}
	//make arguments object
	args := &def.Args{
		A: 2,
		B: 3,
	}
	//this will store returned result
	var result def.Result
	//call remote procedure with args
	err = client.Call("Arith.Multiply", args, &result)
	if err != nil {
		log.Fatalf("error in Arith, %s", err.Error())
	}
	//we got our result in result
	log.Printf("%d*%d=%d\n", args.A, args.B, result)
}
