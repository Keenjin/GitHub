package main

import (
	"fmt"
	"strings"
)

func processListString(list []string, chain []func(string) string) {
	for index, str := range list {
		result := str
		for _, proc := range chain {
			result = proc(result)
		}
		list[index] = result
	}
}

func main() {
	list := []string{
		"    arabvd ",
		" bbdsard  ",
		" ccskhgmc ",
		"   ddkjbd ",
	}

	chain := []func(string) string{
		strings.ToUpper,
		strings.TrimSpace,
	}

	processListString(list, chain)
	for _, str := range list {
		fmt.Println(str)
	}
}
