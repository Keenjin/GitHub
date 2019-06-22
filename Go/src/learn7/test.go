package main

import (
	"flag"
	"fmt"
)

func main() {
	var mode = flag.String("mode", "", "处理模式")
	flag.Parse()
	fmt.Println(*mode)

	a := new(int)
	*a = 1000
	fmt.Println(*a)
}
