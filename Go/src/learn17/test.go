package main

import (
	"fmt"
	"path"
)

func main() {
	fmt.Println("a")
	fmt.Println(path.Dir("."))
	fmt.Println(path.Join(".", "a", "b.txt"))
}
