package main

import "fmt"
import "math"

var a int
var b string
var c float32
var d float64
var e []int
var f func() bool
var g bool
var h struct {
	a int
}

func main() {
	fmt.Println(a)
	fmt.Println(b)
	fmt.Println(c)
	fmt.Println(d)
	fmt.Println(e)
	//fmt.Println(f)
	fmt.Println(g)
	fmt.Println(h)

	i, _ := test()
	fmt.Println(i)
	fmt.Printf("%.2f", math.Pi)
}

func test() (int, int) {
	return 110, 120
}
