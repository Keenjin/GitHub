package main

import "fmt"

func main() {
	m := make(map[string]int)
	m["sss"] = 1
	m["ssa"] = 2
	m["a"] = 3
	for k, v := range m {
		fmt.Println(k, v)
	}

	arr := [3]int{}
	arr[0] = 1
	arr[1] = 2
	for k, v := range arr {
		fmt.Println(k, v)
	}

	arr2 := make([]int, 3)
	arr2[0] = 1
	arr2[1] = 2
	for k, v := range arr2 {
		fmt.Println(k, v)
	}
}
