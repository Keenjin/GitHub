package main

import "fmt"

const (
	a int = iota
	b
	c
	d
)

const (
	aa = 1 << iota
	ab
	ac
	ad
)

func main() {
	fmt.Println(a)
	fmt.Println(b)
	fmt.Println(c)
	fmt.Println(d)

	fmt.Println(aa)
	fmt.Println(ab)
	fmt.Println(ac)
	fmt.Println(ad)
}
