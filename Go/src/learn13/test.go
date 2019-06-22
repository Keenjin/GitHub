package main

import (
	"container/list"
	"fmt"
)

func main() {
	l := list.New()
	l.PushBack("aaa")
	l.PushBack("bbb")
	l.PushFront("ccc")
	for i := l.Front(); i != nil; i = i.Next() {
		fmt.Println(i.Value)
	}
}
